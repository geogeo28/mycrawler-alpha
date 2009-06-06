/****************************************************************************
 * @(#) Communication between the list of urls and the server
 *
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

#ifndef URLSPOOLMANAGER_H
#define URLSPOOLMANAGER_H

#include <QObject>
#include <QList>

#include "UrlsCollection.h"
#include "UrlInfo.h"

class CNetworkManager;

class MCUrlsPoolManager : public QObject
{
    Q_OBJECT

public:
    MCUrlsPoolManager(
      CNetworkManager* networkManager,
      const MCUrlsCollection* urlsCrawledCollection, const MCUrlsCollection* urlsNeighborCollection,
      int urlCount = 50, quint32 packetAverageSize = 256000,
      QObject* parent = NULL
    );

public:
    int urlCount() const { return m_nUrlCount; }
    quint32 packetAverageSize() const { return m_u32PacketAverageSize; }

signals:
    void urlToTransferAdded(MCUrlInfo urlInfo);
    void allUrlsToTransferRemoved();

public slots:
    void addUrlToTransfer(MCUrlInfo urlInfo);

private slots:
    void networkManagerAllDone_();

private:
    void sendDataNodes_(int n);
    void sendLinkNodes_(int n);
    void flush_();

private:
    CNetworkManager* m_pNetworkManager;
    const MCUrlsCollection* m_pUrlsCrawledCollection;
    const MCUrlsCollection* m_pUrlsNeighborCollection;
    QList<MCUrlInfo> m_lstUrlsToTransfer;
    int m_nUrlCount;
    quint32 m_u32PacketAverageSize;
    bool m_bTransferRequest;
};

#endif // URLSPOOLMANAGER_H
