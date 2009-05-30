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

#include <QHash>

#include "Debug/Exception.h"

#include "UrlsCollection.h"

class MCUrlsCollectionPrivate : public QSharedData
{
  public:
    MCUrlsCollectionPrivate();

    QHash<QByteArray, MCUrlInfo> urls;
};

MCUrlsCollectionPrivate::MCUrlsCollectionPrivate()
{}

MCUrlsCollection::MCUrlsCollection(QObject* parent)
  : QObject(parent),
    d(new MCUrlsCollectionPrivate)
{}

MCUrlsCollection::MCUrlsCollection(const MCUrlsCollection &other)
  : QObject(other.parent()),
    d(other.d)
{}

MCUrlsCollection& MCUrlsCollection::operator=(const MCUrlsCollection& urlCollection) {
  d = urlCollection.d;
  return *this;
}

MCUrlsCollection::~MCUrlsCollection()
{
  // QExplicitlySharedDataPointer takes care of deleting for us
}

bool MCUrlsCollection::urlExists(const QByteArray& hash) const {
  return d->urls.contains(hash);
}

bool MCUrlsCollection::addUrl(MCUrlInfo urlInfo) {
  Assert(urlInfo.isValid());

  if (urlExists(urlInfo) == true) {
    return false;
  }

  d->urls.insert(urlInfo.hash(), urlInfo);
  emit urlAdded(urlInfo);
  return true;
}

bool MCUrlsCollection::addUrl(const QUrl& url) {
  Assert(url.isValid());

  return addUrl(MCUrlInfo(url));
}

bool MCUrlsCollection::removeUrl(const QByteArray& hash) {
  MCUrlInfo url = urlInfo(hash);
  if (url.isValid() == false) {
    return false;
  }

  d->urls.remove(hash);
  emit urlRemoved(url);

  return true;
}

MCUrlInfo MCUrlsCollection::urlInfo(const QByteArray& hash) const {
  return d->urls.value(hash);
}

MCUrlsCollection MCUrlsCollection::clone() const {
  MCUrlsCollection copy(*this);
  copy.d.detach();
  return copy;

  return *this;
}
