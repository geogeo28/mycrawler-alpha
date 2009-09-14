/****************************************************************************
 * Copyright (C) 2009 by ANNEHEIM Geoffrey and PORTEJOIE Julien
 * Contact: geoffrey.anneheim@gmail.com / julien.portejoie@gmail.com
 *
 * GNU General Public License Usage
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3.0 as published
 * by the Free Software Foundation and appearing in the file LICENSE.TXT
 * included in the packaging of this file.  Please review the following
 * information to ensure the GNU General Public License version 3.0
 * requirements will be met: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id$
 ****************************************************************************/

#include <QRegExp>
#include <QSet>
#include <QByteArray>

#include "Debug/Exception.h"

#include "NetworkManager.h"
#include "UrlsCollection.h"

#include "Crawl.h"
#include "Client.h"

static QRegExp RegExpHTTPLink("<a\\s+href=[\"']?([^\"'>]+)[\"'>]", Qt::CaseInsensitive);

static const char* BlackList[] = {
  "*.google.*",
  "*.youtube.*",
  "*.forum-discussions.*",
  "*.yahoo.*",
  "*.web-tricheur.*",
  "*.pixelinvaders.*"
  "*.alvasoft.*",
  "*.witi.*",
  "*.pockett.*",
  "*.vive-internet-gratuit.*"
};
static const int BlackListCount = sizeof(BlackList) / sizeof(char*);

MCCrawl::MCCrawl(
  CNetworkManager* networkManager,
  MCUrlsCollection* urlsInQueue, MCUrlsCollection* urlsNeighbor, MCUrlsCollection* urlsCrawled,
  quint32 maxDepth,
  QObject* parent
)
  : QObject(parent),
    m_pNetworkManager(networkManager),
    m_pUrlsInQueue(urlsInQueue),
    m_pUrlsNeighbor(urlsNeighbor),
    m_pUrlsCrawled(urlsCrawled),
    m_u32MaxDepth(maxDepth),
    m_seedUrlDepth(0)
{
  AssertCheckPtr(urlsInQueue);
  AssertCheckPtr(urlsNeighbor);
  AssertCheckPtr(urlsCrawled);
  AssertCheckPtr(networkManager);

  qRegisterMetaType<MCUrlInfo>("MCUrlInfo");

  QObject::connect(m_pUrlsInQueue, SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(queueUrlAdded_(MCUrlInfo)));
  QObject::connect(m_pNetworkManager, SIGNAL(responseHeaderReceived(int,const NetworkManagerThread*)), this, SLOT(networkManagerResponseHeaderReceived_(int,const NetworkManagerThread*)));
  QObject::connect(m_pNetworkManager, SIGNAL(finished(const NetworkManagerThread*)), this, SLOT(networkManagerFinished_(const NetworkManagerThread*)));
  QObject::connect(m_pNetworkManager, SIGNAL(allDone()), this, SLOT(networkManagerAllDone_()), Qt::QueuedConnection);

  QObject::connect(MCClient::instance(), SIGNAL(stateChanged(MCClient::State)), this, SLOT(clientStateChanged_(MCClient::State)));
  QObject::connect(MCClient::instance(), SIGNAL(seedUrlReceived(MCUrlInfo)), this, SLOT(clientSeedUrlReceived_(MCUrlInfo)));
}

void MCCrawl::queueUrlAdded_(MCUrlInfo urlInfo) {
  //if (m_bStarted == true) {
  m_pNetworkManager->addPendingRequest(urlInfo);

  if (MCClient::instance()->state() == MCClient::IdleState) {
    MCClient::instance()->setState(MCClient::CrawlState);
  }

  //}
}

void MCCrawl::networkManagerResponseHeaderReceived_(int statusCode, const NetworkManagerThread* thread) {
  Q_UNUSED(statusCode);

  QString contentType = thread->reply()->header(QNetworkRequest::ContentTypeHeader).toString();
  if (!contentType.startsWith("text", Qt::CaseInsensitive)) {
    thread->reply()->abort();
  }
}

void MCCrawl::networkManagerFinished_(const NetworkManagerThread* networkThread) {
  QNetworkReply* reply = networkThread->reply();
  AssertCheckPtr(reply);

  MCUrlInfo currentUrl = networkThread->urlInfo();
  m_pUrlsInQueue->removeUrl(currentUrl);

  // TODO : Avoid to loose somes links
  m_pUrlsNeighbor->removeUrl(currentUrl);

  // Error during downloading the page, don't continue
  if (reply->error() != QNetworkReply::NoError) {
    return;
  }

  analyzeContent_(reply, currentUrl);
}

void MCCrawl::networkManagerAllDone_() {
  if (m_pNetworkManager->pendingRequests().isEmpty() == true) {
    MCClient::instance()->setState(MCClient::IdleState);
  }
  else if (m_pNetworkManager->processingPendingRequests() == false) {
    MCClient::instance()->setState(MCClient::CrawlState);
  }
}

void MCCrawl::clientStateChanged_(MCClient::State state) {
  switch (state) {
    case MCClient::UnavailableState:
    case MCClient::SendNodesState:
      m_pNetworkManager->setProcessingPendingRequests(false);
      break;

    case MCClient::IdleState:
    {
      if (m_pNetworkManager->pendingRequests().isEmpty() == true) {
        MCClient::instance()->sendSeedUrlRequest();
      }
      else {
        MCClient::instance()->setState(MCClient::CrawlState);
      }
    }
    break;

    case MCClient::CrawlState:
      m_pNetworkManager->setProcessingPendingRequests(true);
      break;

    default:;
  }
}

void MCCrawl::clientSeedUrlReceived_(MCUrlInfo urlInfo) {
  Assert(urlInfo.isValid() == true);
  m_seedUrlDepth = urlInfo.depth();
}

void MCCrawl::analyzeContent_(QIODevice* device, MCUrlInfo parentUrl){
  // Url parent is now crawled
  parentUrl.setCrawled(true);
  m_pUrlsCrawled->addUrl(parentUrl);
  
  // Current depth of sub links
  quint32 currentDepth = parentUrl.depth() + 1;
  QSet<QByteArray> lstUrlsInPage; // Urls previously analyzed in the page
  
  // Extract page content
  QTextStream textStream(device);
  QString str = textStream.readAll();

  int pos = 0;
  while ((pos = RegExpHTTPLink.indexIn(str, pos))!=-1) {
    QApplication::processEvents();

    QString parsedUrl = RegExpHTTPLink.cap(1).simplified();
    pos += RegExpHTTPLink.matchedLength();

    if (parsedUrl.startsWith("#") || parsedUrl.startsWith("javascript:", Qt::CaseInsensitive) || parsedUrl.startsWith("mailto:", Qt::CaseInsensitive))
      continue;

    // Create an absolute url
    QUrl url = QUrl(parsedUrl);
    if (QUrl(parsedUrl).isRelative()) {
      url = MCUrlInfo::absoluteUrl(parentUrl.url().toString(QUrl::None), parsedUrl);
    }

    // Only the HTTP protocol is supported
    if (url.scheme().toLower() != "http") {
      continue;
    }

    // In the black list
    QString host = url.host().toLower();
    int i = 0;
    for (; i < BlackListCount; ++i) {
      const char* pattern = BlackList[i];
      QRegExp reg(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
      if (reg.exactMatch(host)) {
        ILogger::Trace() << host;
        break;
      }
    }
    if (i != BlackListCount) {
      continue;
    }

    // Create a new url info
    MCUrlInfo currentUrl(url, currentDepth);
    if (currentUrl.isValid() == false) {
      ILogger::Notice() << "Invalid url : " << url.toString(QUrl::None);
      continue;
    }

    // Recursif link (parent <=> current)
    if (currentUrl.hash() == parentUrl.hash()) {
      continue;
    }

    // Check if the url was already analyzed in the page
    if (lstUrlsInPage.contains(currentUrl.hash()) == true) {
      continue;
    }

    // Add url in the list of urls already analyzed in the page
    lstUrlsInPage.insert(currentUrl.hash());

    // Url already crawled
    if (m_pUrlsCrawled->urlExists(currentUrl) == true) {
      MCUrlInfo urlCrawled = m_pUrlsCrawled->urlInfo(currentUrl.hash());
      parentUrl.addSuccessor(urlCrawled);
      continue;
    }

    // Url already crawled
    MCUrlInfo tmpUrl = m_pUrlsCrawled->urlInfo(currentUrl.hash());
    if (tmpUrl.isValid() == false) {
      // Url already in the list of neighbor
      tmpUrl = m_pUrlsNeighbor->urlInfo(currentUrl.hash());
      // Url already in the queueing requests
      if (tmpUrl.isValid() == false) {
        tmpUrl = m_pUrlsInQueue->urlInfo(currentUrl.hash());
      }
    }

    // Current url is now the url found in one list of urls
    if (tmpUrl.isValid() == true) {
      currentUrl = tmpUrl;
    }

    // Create edge
    parentUrl.addSuccessor(currentUrl);

    // Url already presents in one list of urls
    if (tmpUrl.isValid() == true) {
      continue;
    }

    // Neighbor url
    if (currentDepth >= (m_seedUrlDepth + maxDepth())) {
      m_pUrlsNeighbor->addUrl(currentUrl);
    }
    // Add in pending requests
    else {
      m_pUrlsInQueue->addUrl(currentUrl);
    }
  }
}
