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

#include "Debug/Exception.h"

#include "UrlsInQueueTreeWidget.h"

#ifdef MYCRAWLER_SERVER
  #include "ServerApplication.h"
#else
  #include "ClientApplication.h"
#endif

Q_DECLARE_METATYPE(QTreeWidgetItem*);

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Hash Signature",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Order",           60, false},
  (MyQTreeWidgetHeaderItem){NULL, "Depth",           -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Url",             -1, false}
};
static const int ColumnsHeaderCount = 4;
static const int ColumnSortedIndex = MCUrlsInQueueTreeWidget::OrderColumn;

void MCUrlsInQueueTreeWidget::cleanAll_() {

}

MCUrlsInQueueTreeWidget::MCUrlsInQueueTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCUrlsInQueueTreeWidget::~MCUrlsInQueueTreeWidget() {
  cleanAll_();
}

void MCUrlsInQueueTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex, Qt::AscendingOrder);
  setPersistentColumnIndex(UrlColumn);

  // Signals/slots connections
  qRegisterMetaType<MCUrlInfo>("MCUrlInfo");

  QObject::connect(MCApp->urlsInQueue(), SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(slotAddUrl_(MCUrlInfo)), Qt::QueuedConnection);
  QObject::connect(MCApp->urlsInQueue(), SIGNAL(urlRemoved(MCUrlInfo)), this, SLOT(slotRemoveUrl_(MCUrlInfo)), Qt::QueuedConnection);
}

void MCUrlsInQueueTreeWidget::slotAddUrl_(MCUrlInfo urlInfo) {
  static int lastOrder = 0;

  Assert(urlInfo.isValid() == true);
  QTreeWidgetItem* item = new QTreeWidgetItem(this);

  // Set backtracker
  urlInfo.setData("ItemInQueue", qVariantFromValue<QTreeWidgetItem*>(item));

  // Set columns data
  item->setData(OrderColumn, Qt::UserRole, ++lastOrder);
  item->setData(DepthColumn, Qt::UserRole, urlInfo.depth());
  item->setData(UrlColumn,   Qt::UserRole, qVariantFromValue<MCUrlInfo>(urlInfo));

  // Set columns content
  item->setText(HashSignatureColumn, urlInfo.hash().toHex());
  item->setText(OrderColumn,         QString::number(lastOrder));
  item->setText(UrlColumn,           urlInfo.url().toString(QUrl::None));
}

void MCUrlsInQueueTreeWidget::slotRemoveUrl_(MCUrlInfo urlInfo) {
  Assert(urlInfo.isValid() == true);

  QTreeWidgetItem* item = qVariantValue<QTreeWidgetItem*>(urlInfo.data("ItemInQueue"));
  AssertCheckPtr(item);

  delete item;
}
