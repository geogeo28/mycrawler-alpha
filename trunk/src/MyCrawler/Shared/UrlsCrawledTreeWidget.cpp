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

#include "UrlsCrawledTreeWidget.h"

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
static const int ColumnSortedIndex = -1;

void MCUrlsCrawledTreeWidget::cleanAll_() {

}

MCUrlsCrawledTreeWidget::MCUrlsCrawledTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCUrlsCrawledTreeWidget::~MCUrlsCrawledTreeWidget() {
  cleanAll_();
}

void MCUrlsCrawledTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex, Qt::AscendingOrder);
  setPersistentColumnIndex(UrlColumn);

  // Signals/slots connections
  qRegisterMetaType<MCUrlInfo>("MCUrlInfo");

  QObject::connect(MCApp->urlsCrawled(), SIGNAL(urlAdded(MCUrlInfo)), this, SLOT(slotAddUrl_(MCUrlInfo)), Qt::QueuedConnection);
  QObject::connect(MCApp->urlsCrawled(), SIGNAL(urlRemoved(MCUrlInfo)), this, SLOT(slotRemoveUrl_(MCUrlInfo)), Qt::QueuedConnection);
}

void MCUrlsCrawledTreeWidget::slotAddUrl_(MCUrlInfo urlInfo) {
  static int lastOrder = 0;

  Assert(urlInfo.isValid() == true);
  QTreeWidgetItem* item = new QTreeWidgetItem(this);

  // Set backtracker
  urlInfo.setData("ItemCrawled", qVariantFromValue<QTreeWidgetItem*>(item));

  // Set columns data
  item->setData(OrderColumn, Qt::UserRole, ++lastOrder);
  item->setData(DepthColumn, Qt::UserRole, urlInfo.depth());
  item->setData(UrlColumn,   Qt::UserRole, qVariantFromValue<MCUrlInfo>(urlInfo));

  // Set columns content
  item->setText(HashSignatureColumn, urlInfo.hash().toHex());
  item->setText(OrderColumn,         QString::number(lastOrder));
  item->setText(UrlColumn,           urlInfo.url().toString(QUrl::None));
}

void MCUrlsCrawledTreeWidget::slotRemoveUrl_(MCUrlInfo urlInfo) {
  Assert(urlInfo.isValid() == true);

  QTreeWidgetItem* item = qVariantValue<QTreeWidgetItem*>(urlInfo.data("ItemCrawled"));
  AssertCheckPtr(item);

  delete item;
}
