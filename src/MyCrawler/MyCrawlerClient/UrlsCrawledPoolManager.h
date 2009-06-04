/****************************************************************************
 * @(#) Communication between the list of Urls crawled and the server
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

#ifndef URLSCRAWLEDPOOLMANAGER_H
#define URLSCRAWLEDPOOLMANAGER_H

#include <QObject>
#include <QList>

#include "UrlsCollection.h"
#include "UrlInfo.h"

class MCUrlsCrawledPoolManager : public QObject
{
    Q_OBJECT

public:
    MCUrlsCrawledPoolManager(
      const MCUrlsCollection* urlsCrawledCollection,
      int urlCount = 50, quint32 packetAverageSize = 256000,
      QObject* parent = NULL
    );

public:
    int urlCount() const { return m_nUrlCount; }
    quint32 packetAverageSize() const { return m_u32PacketAverageSize; }

private slots:
    void urlCrawledAdded_(MCUrlInfo urlInfo);

private:
    void sendDataNodes_(int n);
    void sendLinkNodes_(int n);
    void flush_();

private:
    const MCUrlsCollection* m_pUrlsCrawledCollection;
    int m_nUrlCount;
    quint32 m_u32PacketAverageSize;
    QList<MCUrlInfo> m_lstUrlsToTransfer;
};

#endif // URLSCRAWLEDPOOLMANAGER_H
