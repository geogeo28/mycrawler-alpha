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

#include <QList>
#include <QDataStream>

#include "Debug/Exception.h"

#include "Macros.h"
#include "UrlsCrawledPoolManager.h"
#include "Client.h"

MCUrlsCrawledPoolManager::MCUrlsCrawledPoolManager(
  const MCUrlsCollection* urlsCrawledCollection,
  int urlCount, quint32 packetAverageSize,
  QObject* parent
)
  : QObject(parent),
    m_pUrlsCrawledCollection(urlsCrawledCollection),
    m_nUrlCount(urlCount),
    m_u32PacketAverageSize(packetAverageSize)
{
  AssertCheckPtr(urlsCrawledCollection);
  Assert(urlCount > 0);
  Assert(packetAverageSize >= 2048);

  QObject::connect(m_pUrlsCrawledCollection, SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(urlCrawledAdded_(MCUrlInfo)));
}

void MCUrlsCrawledPoolManager::urlCrawledAdded_(MCUrlInfo urlInfo) {
  m_lstUrlsToTransfer.append(urlInfo);
  if (m_lstUrlsToTransfer.count() == urlCount()) {
    flush_();
  }
}

void MCUrlsCrawledPoolManager::sendDataNodes_(int n) {
  MC_DATASTREAM_WRITE(bytes, data);
  int nPackaged = 0;

  QListIterator<MCUrlInfo> it(m_lstUrlsToTransfer);
  for (int nTotal = 0; (it.hasNext() && (nTotal < n)); ++nTotal) {
    const MCUrlInfo& urlInfo = it.next();
    data << urlInfo.url().toString(QUrl::None);
    data << urlInfo.depth();
    ++nPackaged;

    if ((quint32)bytes.size() >= packetAverageSize()) {
      MCClient::instance()->sendDataNodes(nPackaged, bytes);
      bytes.clear();
      nPackaged = 0;
    }
  }
  // The buffer contains some nodes to transfer
  if (nPackaged != 0) {
    MCClient::instance()->sendDataNodes(nPackaged, bytes);
  }
}

void MCUrlsCrawledPoolManager::sendLinkNodes_(int n) {
  MC_DATASTREAM_WRITE(bytes, data);

  QListIterator<MCUrlInfo> it(m_lstUrlsToTransfer);
  for (int nTotal = 0; (it.hasNext() && nTotal < n); ++nTotal) {
    const MCUrlInfo& parent = it.next();
    data << parent.hash();

    ILogger::Trace() << "Start " << QString(parent.hash().toHex());

    int nPackaged = 0;
    QListIterator<MCUrlInfo> itSucc(parent.successors());
    while (itSucc.hasNext()) {
      const MCUrlInfo& succ = itSucc.next();
      data << succ.hash();
      ++nPackaged;

      ILogger::Trace() << "Link " << QString(succ.hash().toHex());

      if ((quint32)bytes.size() >= packetAverageSize()) {
        MCClient::instance()->sendLinkNodes(nPackaged, bytes);
        ILogger::Trace() << "Send";

        bytes.clear();
        nPackaged = 0;

        // Prepare the next packet
        data << parent.hash();
      }
    }

    // The buffer contains some links to transfer
    if (nPackaged != 0) {
      MCClient::instance()->sendLinkNodes(nPackaged, bytes);
      ILogger::Trace() << "Send";

      bytes.clear();
    }
  }
}

void MCUrlsCrawledPoolManager::flush_() {
  int n = m_lstUrlsToTransfer.size();
  sendDataNodes_(n);
  sendLinkNodes_(n);
  m_lstUrlsToTransfer.clear();
}
