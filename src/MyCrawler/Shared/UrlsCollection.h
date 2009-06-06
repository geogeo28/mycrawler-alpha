/****************************************************************************
 * @(#) Urls collection.
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

#ifndef URLSCOLLECTION_H
#define URLSCOLLECTION_H

#include <QObject>
#include <QSharedData>
#include <QByteArray>

#include "UrlInfo.h"

class MCUrlsCollectionPrivate;

class MCUrlsCollection : public QObject
{
    Q_OBJECT

public:
    MCUrlsCollection(QObject* parent = NULL);
    MCUrlsCollection(const MCUrlsCollection &other);
    MCUrlsCollection& operator=(const MCUrlsCollection& urlCollection);
    ~MCUrlsCollection();

    bool isEmpty() const;
    int count() const;

    bool urlExists(const QByteArray& hash) const;
    bool urlExists(const MCUrlInfo& urlInfo) const { return urlExists(urlInfo.hash()); }

    bool addUrl(MCUrlInfo urlInfo);
    bool addUrl(const QUrl& url);
    bool removeUrl(const QByteArray& hash);
    bool removeUrl(const MCUrlInfo& urlInfo) { return removeUrl(urlInfo.hash()); }
    void removeAllUrls();

    MCUrlInfo urlInfo(const QByteArray& hash) const;
    MCUrlInfo takeOne();

    void merge(const MCUrlsCollection& urls);
    QList<MCUrlInfo> urls() const;

    MCUrlsCollection clone() const;

signals:
    void urlAdded(MCUrlInfo urlInfo);
    void urlRemoved(MCUrlInfo urlInfo);
    void allUrlsRemoved();

private:
    QExplicitlySharedDataPointer<MCUrlsCollectionPrivate> d;
    friend class MCUrlsCollectionPrivate;
};

#endif // URLSCOLLECTION_H
