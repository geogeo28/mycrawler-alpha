/****************************************************************************
 * @(#) Url Info.
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

#ifndef URLINFO_H
#define URLINFO_H

#include <QSharedData>
#include <QUrl>
#include <QList>

class QVariant;

class MCUrlInfoPrivate;

class MCUrlInfo
{
public:
    MCUrlInfo();
    explicit MCUrlInfo(const QUrl& url, quint32 depth = 0);
    explicit MCUrlInfo(const QString& url, quint32 depth = 0);

    MCUrlInfo(const MCUrlInfo &other);
    MCUrlInfo& operator=(const MCUrlInfo& urlInfo);
    ~MCUrlInfo();

    bool isValid() const;

    QUrl url() const;
    QByteArray hash() const;
    quint32 depth() const;
    void setDepth(quint32 depth);

    void addSuccessor(MCUrlInfo urlInfo); // This method don't check if urlInfo is already a successor
    void addAncestor(MCUrlInfo urlInfo); // This method don't check if urlInfo is already an ancestor

    QList<MCUrlInfo> successors() const;
    QList<MCUrlInfo> ancestors() const;

    bool isCrawled() const;
    void setCrawled(bool crawled);

    MCUrlInfo clone() const;

    void setData(const QString& name, const QVariant& data);
    QVariant data(const QString& name) const;

public:
    bool operator==(const MCUrlInfo& urlInfo) const { return (this->hash() == urlInfo.hash()); }
    bool operator!=(const MCUrlInfo& urlInfo) const { return !(*this == urlInfo); }

public:
    static QUrl decodedUrl(const QUrl& url);
    static QUrl decodedUrl(const QString& url);
    static QUrl absoluteUrl(const QString& base, const QString& relative);

private:
    QExplicitlySharedDataPointer<MCUrlInfoPrivate> d;
    friend class MCUrlInfoPrivate;
};

Q_DECLARE_METATYPE(MCUrlInfo);

#endif // URLINFO_H
