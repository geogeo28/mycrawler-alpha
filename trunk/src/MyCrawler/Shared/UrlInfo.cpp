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

#include <QByteArray>
#include <QCryptographicHash>

#include "Debug/Exception.h"

#include "UrlInfo.h"

class MCUrlInfoPrivate : public QSharedData
{
  public:
    MCUrlInfoPrivate();
    MCUrlInfoPrivate(const QUrl& url, int depth = 0);

    QUrl url;
    int depth;

    QByteArray hash;
};

MCUrlInfoPrivate::MCUrlInfoPrivate()
  : depth(0)
{}

MCUrlInfoPrivate::MCUrlInfoPrivate(const QUrl& url, int depth)
  : url(url),
    depth(depth)
{
  hash = QCryptographicHash::hash(url.toEncoded(QUrl::None), QCryptographicHash::Sha1);
}

MCUrlInfo::MCUrlInfo()
  : d(new MCUrlInfoPrivate)
{}

MCUrlInfo::MCUrlInfo(const QUrl& url, int depth)
  : d(new MCUrlInfoPrivate(url, depth))
{}

MCUrlInfo::MCUrlInfo(const QString& url, int depth)
  : d(new MCUrlInfoPrivate(MCUrlInfo::decodedUrl(url), depth))
{}

MCUrlInfo::MCUrlInfo(const MCUrlInfo &other)
  : CDataContainer(other),
    d(other.d)
{}

MCUrlInfo::MCUrlInfo& MCUrlInfo::operator=(const MCUrlInfo& urlInfo) {
  d = urlInfo.d;
  return *this;
}

MCUrlInfo::~MCUrlInfo()
{
  // QExplicitlySharedDataPointer takes care of deleting for us
}

bool MCUrlInfo::isValid() const {
  return d->url.isValid();
}

QUrl MCUrlInfo::url() const {
  return d->url;
}

QByteArray MCUrlInfo::hash() const {
  return d->hash;
}

int MCUrlInfo::depth() const {
  return d->depth;
}

void MCUrlInfo::setDepth(int depth) {
  Assert(depth >= 0);
  d->depth = depth;
}

MCUrlInfo MCUrlInfo::clone() const {
  MCUrlInfo copy(*this);
  copy.d.detach();
  return copy;
}

QUrl MCUrlInfo::decodedUrl(const QUrl& url) {
  QUrl tmpUrl = QUrl::fromEncoded(url.toString().toAscii());
  return QUrl(QUrl::fromPercentEncoding(tmpUrl.toString().toAscii()));
}

QUrl MCUrlInfo::decodedUrl(const QString& url) {
  return MCUrlInfo::decodedUrl(QUrl(url, QUrl::TolerantMode));
}

QUrl MCUrlInfo::absoluteUrl(const QString& base, const QString& relative) {
  return QUrl(base).resolved(QUrl(relative));
}