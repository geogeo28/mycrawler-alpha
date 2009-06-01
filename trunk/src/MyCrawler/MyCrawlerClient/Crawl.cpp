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

#include "Debug/Exception.h"

#include "NetworkManager.h"
#include "UrlsCollection.h"

#include "Crawl.h"
#include "ClientApplication.h"

MCCrawl::MCCrawl(CNetworkManager* networkManager, quint32 depth, QObject* parent)
  : QObject(parent),
    m_pNetworkManager(networkManager),
    m_u32Depth(depth),
    m_bStarted(false)
{
  AssertCheckPtr(networkManager);

  qRegisterMetaType<MCUrlInfo>("MCUrlInfo");
  QObject::connect(&(m_pNetworkManager->pendingRequestsCollection()), SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(queueUrlAdded_(MCUrlInfo)), Qt::QueuedConnection);

  QObject::connect(m_pNetworkManager, SIGNAL(success(const NetworkManagerThread*)), this, SLOT(networkManagerFinished_(const NetworkManagerThread*)));
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
    m_pNetworkManager->setProcessingPendingRequests(true);
  //}
}

void MCCrawl::networkManagerFinished_(const NetworkManagerThread* networkThread) {
  QNetworkReply* reply = networkThread->reply();
  AssertCheckPtr(reply);

  // Error during downloading the page, don't continue
  if (reply->error() != QNetworkReply::NoError) {
    return;
  }

  analyzeContent_(reply, networkThread->urlInfo());
}

void MCCrawl::analyzeContent_(QIODevice* device, MCUrlInfo urlInfoParent){
  QTextStream textStream(device);
  QString str = textStream.readAll();

  QRegExp oRegex("<a\\s+href=[\"']?([^\"'>]+)[\"'>]", Qt::CaseInsensitive);  // Lien Url
  int pos = 0;
  while ((pos = oRegex.indexIn(str, pos))!=-1) {
    QString parsedUrl = oRegex.cap(1).simplified();
    pos += oRegex.matchedLength();

    if (parsedUrl.startsWith("#") || parsedUrl.startsWith("javascript:", Qt::CaseInsensitive) || parsedUrl.startsWith("mailto:", Qt::CaseInsensitive))
      continue;

    // TOTO : Pb url décodée plusieurs fois !
    QUrl url = QUrl(parsedUrl);
    if (QUrl(parsedUrl).isRelative()) {
      url = MCUrlInfo::absoluteUrl(urlInfoParent.url().toString(QUrl::None), parsedUrl);
    }

    MCApp->urlsCrawled()->addUrl(urlInfoParent);

    // Check depth
    if (urlInfoParent.depth() == m_u32Depth) {
      continue;
    }

    // Only the HTTP protocol is supported
    if (url.scheme().toLower() != "http") {
      continue;
    }

    // Create a new url info and add in queue
    MCUrlInfo newUrlInfo(url, urlInfoParent.depth() + 1, urlInfoParent);

    // Url already crawled
    if (MCApp->urlsCrawled()->urlExists(newUrlInfo) == true) {
      continue;
    }

    MCApp->urlsInQueue()->addUrl(newUrlInfo);
  }
}
