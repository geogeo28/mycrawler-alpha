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

#include <QMap>

#include "Debug/Exception.h"

#include "UrlsCollection.h"

class MCUrlsCollectionPrivate : public QSharedData
{
  public:
    typedef QHash<QByteArray, MCUrlInfo> UrlsContainer;
    typedef QHashIterator<QByteArray, MCUrlInfo> Iterator;

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
  removeAll();

  // QExplicitlySharedDataPointer takes care of deleting for us
}

bool MCUrlsCollection::isEmpty() const {
  return d->urls.isEmpty();
}

int MCUrlsCollection::count() const {
  return d->urls.count();
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

void MCUrlsCollection::removeAll() {
  MCUrlsCollectionPrivate::Iterator it(d->urls);
  while (it.hasNext()) {
    emit urlRemoved(it.next().value());
  }

  d->urls.clear();
}

MCUrlInfo MCUrlsCollection::urlInfo(const QByteArray& hash) const {
  return d->urls.value(hash);
}

MCUrlInfo MCUrlsCollection::takeOne() {
  MCUrlsCollectionPrivate::UrlsContainer::Iterator it = d->urls.begin();

  // Returns an invalid MCUrlInfo
  if (it == d->urls.end()) {
    return MCUrlInfo();
  }

  MCUrlInfo urlInfo = it.value();
  (void) d->urls.erase(it);
  emit urlRemoved(urlInfo);

  return urlInfo;
}

void MCUrlsCollection::merge(const MCUrlsCollection& urls) {
  // Warning : Multiple entries for a same Url must be impossible
  for (
    MCUrlsCollectionPrivate::UrlsContainer::Iterator it = urls.d->urls.begin();
    it != urls.d->urls.end();
    ++it
  )
  {
    MCUrlInfo urlInfo = it.value();
    d->urls.insert(urlInfo.hash(), urlInfo);
    emit urlAdded(urlInfo);
  }
}

QList<MCUrlInfo> MCUrlsCollection::urls() const {
  return d->urls.values();
}

MCUrlsCollection MCUrlsCollection::clone() const {
  MCUrlsCollection copy(*this);
  copy.d.detach();
  return copy;

  return *this;
}
