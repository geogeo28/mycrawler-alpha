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
#include "UrlsPoolManager.h"
#include "NetworkManager.h"
#include "Client.h"

MCUrlsPoolManager::MCUrlsPoolManager(
  CNetworkManager* networkManager,
  const MCUrlsCollection* urlsCrawledCollection, const MCUrlsCollection* urlsNeighborCollection,
  int urlCount, quint32 packetAverageSize,
  QObject* parent
)
  : QObject(parent),
    m_pNetworkManager(networkManager),
    m_pUrlsCrawledCollection(urlsCrawledCollection),
    m_pUrlsNeighborCollection(urlsNeighborCollection),
    m_nUrlCount(urlCount),
    m_u32PacketAverageSize(packetAverageSize),
    m_bTransferRequest(false)
{
  AssertCheckPtr(networkManager);
  AssertCheckPtr(urlsCrawledCollection);
  AssertCheckPtr(urlsNeighborCollection);
  Assert(packetAverageSize >= 2048);

  QObject::connect(m_pUrlsCrawledCollection, SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(addUrlToTransfer(MCUrlInfo)), Qt::QueuedConnection);
  QObject::connect(m_pUrlsNeighborCollection, SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(addUrlToTransfer(MCUrlInfo)), Qt::QueuedConnection);
  QObject::connect(m_pNetworkManager, SIGNAL(allDone()), this, SLOT(networkManagerAllDone_()), Qt::QueuedConnection);
}

void MCUrlsPoolManager::addUrlToTransfer(MCUrlInfo urlInfo) {
  m_lstUrlsToTransfer.append(urlInfo);
  emit urlToTransferAdded(urlInfo);

  // Push a transfer request
  if (m_lstUrlsToTransfer.count() == urlCount()) {
    m_pNetworkManager->setProcessingPendingRequests(false); // Stop network manager
    m_bTransferRequest = true;

    // All threads in network manager are stopped
    if (m_pNetworkManager->threadsInProcessing() == 0) {
      networkManagerAllDone_();
    }
  }
}

void MCUrlsPoolManager::networkManagerAllDone_() {
  // A transfer request was set
  if (m_bTransferRequest == true) {
    m_bTransferRequest = false;

    // Send data
    flush_();

    // Restart network manager
    m_pNetworkManager->setProcessingPendingRequests(true);
  }
  // No pending request
  else if (m_pNetworkManager->pendingRequests().isEmpty()) {
    flush_();
  }
}

void MCUrlsPoolManager::sendDataNodes_(int n) {
  MC_DATASTREAM_WRITE(buffer, bytes, out);
  int nPackaged = 0;

  QListIterator<MCUrlInfo> it(m_lstUrlsToTransfer);
  for (int nTotal = 0; (it.hasNext() && (nTotal < n)); ++nTotal) {
    const MCUrlInfo& urlInfo = it.next();
    out << urlInfo.isCrawled();
    out << urlInfo.depth();
    out << urlInfo.url();

    ++nPackaged;

    if ((quint32)buffer.bytesAvailable() >= packetAverageSize()) {
      MCClient::instance()->sendDataNodes(nPackaged, bytes);

      bytes.clear();
      buffer.reset();
      nPackaged = 0;
    }
  }
  // The buffer contains some nodes to transfer
  if (nPackaged != 0) {
    MCClient::instance()->sendDataNodes(nPackaged, bytes);
  }
}

void MCUrlsPoolManager::sendLinkNodes_(int n) {
  MC_DATASTREAM_WRITE(buffer, bytes, out);

  QListIterator<MCUrlInfo> it(m_lstUrlsToTransfer);
  for (int nTotal = 0; (it.hasNext() && nTotal < n); ++nTotal) {
    const MCUrlInfo& parent = it.next();
    if (parent.isCrawled() == false) {
      continue;
    }

    out << parent.hash();

    int nPackaged = 0;
    QListIterator<MCUrlInfo> itSucc(parent.successors());
    while (itSucc.hasNext()) {
      const MCUrlInfo& succ = itSucc.next();
      out << succ.hash();
      ++nPackaged;

      if ((quint32)buffer.bytesAvailable() >= packetAverageSize()) {
        MCClient::instance()->sendLinkNodes(nPackaged, bytes);

        bytes.clear();
        buffer.reset();
        nPackaged = 0;

        // Prepare the next packet
        out << parent.hash();
      }
    }

    // The buffer contains some links to transfer
    if (nPackaged != 0) {
      MCClient::instance()->sendLinkNodes(nPackaged, bytes);
    }

    // Prepare buffer for the next node
    bytes.clear();
    buffer.reset();
  }
}

void MCUrlsPoolManager::flush_() {
  int n = m_lstUrlsToTransfer.count();
  sendDataNodes_(n);
  sendLinkNodes_(n);
  m_lstUrlsToTransfer.clear();
  emit allUrlsToTransferRemoved();
}
