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

static QRegExp RegExpHTTPLink("<a\\s+href=[\"']?([^\"'>]+)[\"'>]", Qt::CaseInsensitive);

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
    m_bStarted(false)
{
  AssertCheckPtr(urlsInQueue);
  AssertCheckPtr(urlsNeighbor);
  AssertCheckPtr(urlsCrawled);
  AssertCheckPtr(networkManager);

  qRegisterMetaType<MCUrlInfo>("MCUrlInfo");

  QObject::connect(m_pUrlsInQueue, SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(queueUrlAdded_(MCUrlInfo)), Qt::DirectConnection);
  QObject::connect(m_pNetworkManager, SIGNAL(finished(const NetworkManagerThread*)), this, SLOT(networkManagerFinished_(const NetworkManagerThread*)));
}

/*void MCCrawl::start() {
  if (m_pNetworkManager->pendingRequestsCollection().isEmpty()) {
    return;
  }

  m_bStarted = true;
  m_pNetworkManager->setProcessingPendingRequests(true);
}*/

/*void MCCrawl::stop() {
  m_bStarted = false;
  m_pNetworkManager->setProcessingPendingRequests(false);
}*/

void MCCrawl::queueUrlAdded_(MCUrlInfo urlInfo) {
  //if (m_bStarted == true) {
  m_pNetworkManager->addPendingRequest(urlInfo);
  //}
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
    if (currentDepth >= maxDepth()) {
      m_pUrlsNeighbor->addUrl(currentUrl);
    }
    // Add in pending requests
    else {
      m_pUrlsInQueue->addUrl(currentUrl);
    }
  }
}
