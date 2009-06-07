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

#include <QUrl>

#include "Debug/Exception.h"
#include "Utilities/NetworkInfo.h"

#include "UrlsInProgressTreeWidget.h"
#include "Server.h"

Q_DECLARE_METATYPE(QTreeWidgetItem*);

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Thread ID",       -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Host Name",       -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Peer Address",    -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Peer Port",       -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Hash Signature",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Depth",           -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Url",             -1, false}
};
static const int ColumnsHeaderCount = 7;
static const int ColumnSortedIndex = -1;

void MCUrlsInProgressTreeWidget::cleanAll_() {

}

MCUrlsInProgressTreeWidget::MCUrlsInProgressTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCUrlsInProgressTreeWidget::~MCUrlsInProgressTreeWidget() {
  cleanAll_();
}

void MCUrlsInProgressTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex, Qt::AscendingOrder);
  setPersistentColumnIndex(UrlColumn);

  // Signals/slots connections
  QObject::connect(MCServer::instance(), SIGNAL(clientUrlInProgressAdded(MCClientThread*,MCUrlInfo)), this, SLOT(slotAddUrl_(MCClientThread*,MCUrlInfo)));
  QObject::connect(MCServer::instance(), SIGNAL(clientUrlInProgressRemoved(MCClientThread*,MCUrlInfo)), this, SLOT(slotRemoveUrl_(MCClientThread*,MCUrlInfo)));
}

void MCUrlsInProgressTreeWidget::slotAddUrl_(MCClientThread* client, MCUrlInfo urlInfo) {
  Assert(urlInfo.isValid() == true);
  QTreeWidgetItem* item = new QTreeWidgetItem(this);

  // Set backtracker
  urlInfo.setData("ItemInProgress", qVariantFromValue<QTreeWidgetItem*>(item));

  // Set columns data
  item->setData(ThreadIDColumn,      Qt::UserRole, client->id());
  item->setData(HostNameColumn,      Qt::UserRole, client->networkInfo().hostName());
  item->setData(PeerAddressColumn,   Qt::UserRole, client->peerAddress().toIPv4Address());
  item->setData(PeerPortColumn,      Qt::UserRole, client->peerPort());
  item->setData(HashSignatureColumn, Qt::UserRole, urlInfo.hash());
  item->setData(DepthColumn,         Qt::UserRole, urlInfo.depth());
  item->setData(UrlColumn,           Qt::UserRole, qVariantFromValue<MCUrlInfo>(urlInfo));

  // Set columns content
  item->setText(ThreadIDColumn,      "0x" + QString::number(client->id(), 16));
  item->setText(HostNameColumn,      client->networkInfo().hostName());
  item->setText(PeerAddressColumn,   client->peerAddress().toString());
  item->setText(PeerPortColumn,      QString::number(client->peerPort()));
  item->setText(HashSignatureColumn, urlInfo.hash().toHex());
  item->setText(DepthColumn,         QString::number(urlInfo.depth()));
  item->setText(UrlColumn,           urlInfo.url().toString(QUrl::None));

  // Set columns icons
  item->setIcon(HostNameColumn, QIcon(client->isLocalClient()?":/Clients/ConnectedLocalIcon":":/Clients/ConnectedRemoteIcon"));
}

void MCUrlsInProgressTreeWidget::slotRemoveUrl_(MCClientThread* client, MCUrlInfo urlInfo) {
  Q_UNUSED(client);

  Assert(urlInfo.isValid() == true);

  QTreeWidgetItem* item = qVariantValue<QTreeWidgetItem*>(urlInfo.data("ItemInProgress"));
  AssertCheckPtr(item);

  delete item;
}
