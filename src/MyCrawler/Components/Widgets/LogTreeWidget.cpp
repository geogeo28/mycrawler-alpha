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

#include <QTime>

#include "Debug/Logger.h"
#include "Core/Application.h"

#include "LogTreeWidget.h"

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Date",    120, false},
  (MyQTreeWidgetHeaderItem){NULL, "Message", -1,  false}
};
static const int ColumnsHeaderCount = 2;
static const int ColumnSortedIndex = -1;

void CLogTreeWidget::cleanAll_() {

}

CLogTreeWidget::CLogTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

CLogTreeWidget::~CLogTreeWidget() {
  cleanAll_();
}

void CLogTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex);
  setPersistentColumnIndex(MessageColumn);
}

void CLogTreeWidget::write(Icon icon, const QString& message, const QColor& color, QFont::Weight fontWeight) {
  QString img;
  switch (icon) {
    case ErrorIcon:       img = ":/Log/ErrorIcon"; break;
    case InformationIcon: img = ":/Log/InformationIcon"; break;

    default:;
  }

  QTreeWidgetItem* item = new QTreeWidgetItem(this);  

  item->setText(DateColumn, QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate));

  QFont font;
  font.setWeight(fontWeight);

  item->setData(MessageColumn, Qt::DecorationRole, QIcon(img));
  item->setTextColor(MessageColumn, color);
  item->setFont(MessageColumn, font);
  item->setText(MessageColumn, message);

  scrollToItem(item, QAbstractItemView::EnsureVisible);
}

void CLogTreeWidget::copyToClipboard() {
  int nItems = topLevelItemCount();
  if (nItems == 0) { return; }

  QString s;
  for (int i = 0; i < nItems; ++i) {
    const QTreeWidgetItem* item = topLevelItem(i);
    s += item->text(DateColumn) + " " + item->text(MessageColumn) + "\n";
  }

  QApplication::clipboard()->setText(s, QClipboard::Clipboard);
}

QStyleOptionViewItem CLogTreeWidget::viewOptions() const {
  QStyleOptionViewItem option = QAbstractItemView::viewOptions();
  option.decorationAlignment = Qt::AlignTop;
  option.displayAlignment = Qt::AlignLeft | Qt::AlignTop;
  return option;
}
