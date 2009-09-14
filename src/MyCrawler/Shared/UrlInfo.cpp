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
#include <QVariant>

#include "Debug/Exception.h"
#include "Core/DataContainer.h"

#include "UrlInfo.h"

static MCUrlInfo InvalidUrlInfo;

class MCUrlInfoPrivate : public QSharedData
{
  public:
    MCUrlInfoPrivate();
    MCUrlInfoPrivate(const QUrl& url, quint32 depth = 0);

    QUrl url;
    QByteArray hash;
    quint32 depth;

    QList<MCUrlInfo> successors;
    QList<MCUrlInfo> ancestors;

    bool isCrawled;

    CDataContainer dataContainer;
};

MCUrlInfoPrivate::MCUrlInfoPrivate()
  : depth(0),
    isCrawled(false)
{}

MCUrlInfoPrivate::MCUrlInfoPrivate(const QUrl& url, quint32 depth)
  : url(url),
    depth(depth),
    isCrawled(false)
{
  hash = QCryptographicHash::hash(url.toEncoded(QUrl::None), QCryptographicHash::Sha1);
}

MCUrlInfo::MCUrlInfo()
  : d(new MCUrlInfoPrivate)
{}

MCUrlInfo::MCUrlInfo(const QUrl& url, quint32 depth)
  : d(new MCUrlInfoPrivate(url, depth))
{}

MCUrlInfo::MCUrlInfo(const QString& url, quint32 depth)
  : d(new MCUrlInfoPrivate(MCUrlInfo::decodedUrl(url), depth))
{}

MCUrlInfo::MCUrlInfo(const MCUrlInfo &other)
  : d(other.d)
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

quint32 MCUrlInfo::depth() const {
  return d->depth;
}

void MCUrlInfo::setDepth(quint32 depth) {
  //Assert(depth >= 0);
  d->depth = depth;
}

void MCUrlInfo::addSuccessor(MCUrlInfo urlInfo) {
  d->successors.append(urlInfo);
  urlInfo.d->ancestors.append(*this);
}

void MCUrlInfo::addAncestor(MCUrlInfo urlInfo) {
  d->ancestors.append(urlInfo);
  urlInfo.d->successors.append(*this);
}

QList<MCUrlInfo> MCUrlInfo::successors() const {
  return d->successors;
}

QList<MCUrlInfo> MCUrlInfo::ancestors() const {
  return d->ancestors;
}

bool MCUrlInfo::isCrawled() const {
  return d->isCrawled;
}

void MCUrlInfo::setCrawled(bool crawled) {
  d->isCrawled = crawled;
}

MCUrlInfo MCUrlInfo::clone() const {
  MCUrlInfo copy(*this);
  copy.d.detach();
  return copy;
}

void MCUrlInfo::setData(const QString& name, const QVariant& data) {
  d->dataContainer.setData(name, data);
}

QVariant MCUrlInfo::data(const QString& name) const {
  return d->dataContainer.data(name);
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
