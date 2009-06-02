/****************************************************************************
 * @(#) Crawler coordinator.
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

#ifndef CRAWL_H
#define CRAWL_H

#include <QObject>

#include "UrlInfo.h"
#include "UrlsCollection.h"

class NetworkManagerThread;
class CNetworkManager;

class MCCrawl : public QObject
{
    Q_OBJECT

public:
    MCCrawl(
      MCUrlsCollection* urlsInQueue, MCUrlsCollection* urlsNeighbor, MCUrlsCollection* urlsCrawled,
      CNetworkManager* networkManager, quint32 depth = 5,
      QObject* parent = NULL
    );

public:
    MCUrlsCollection* urlsInQueue() const { return m_pUrlsInQueue; }   // Urls ready to be crawled
    MCUrlsCollection* urlsNeighbor() const { return m_pUrlsNeighbor; } // Urls with a depth equal to m_u32Depth
    MCUrlsCollection* urlsCrawled() const { return m_pUrlsCrawled; } // Urls already crawled

public slots:
    //void start();
    //void stop();

private slots:
    void queueUrlAdded_(MCUrlInfo urlInfo);
    void networkManagerFinished_(const NetworkManagerThread* networkThread);

private:
    void analyzeContent_(QIODevice* device, MCUrlInfo urlInfoParent);

private:
    MCUrlsCollection* m_pUrlsInQueue;
    MCUrlsCollection* m_pUrlsNeighbor;
    MCUrlsCollection* m_pUrlsCrawled;

    CNetworkManager* m_pNetworkManager;
    quint32 m_u32Depth;
    bool m_bStarted;
};

#endif // CRAWL_H
