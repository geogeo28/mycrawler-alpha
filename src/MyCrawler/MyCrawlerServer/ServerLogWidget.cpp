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
#include "ServerLogWidget.h"

#include "ServerApplication.h"

MCServerLogWidget::MCServerLogWidget(QWidget* parent)
  : QTreeWidget(parent)
{}

void MCServerLogWidget::setup() {
  headerItem()->setText(DateColumn,    "Date");
  headerItem()->setText(MessageColumn, "Message");  

  setColumnWidth(DateColumn, 65);
}

void MCServerLogWidget::write(Icon icon, const QString& message, const QColor& color, QFont::Weight fontWeight) {
  QString img;
  switch (icon) {
    case ErrorIcon:       img = ":/Log/ErrorIcon"; break;
    case InformationIcon: img = ":/Log/InformationIcon"; break;

    default:;
  }

  QTreeWidgetItem* item = new QTreeWidgetItem(this);  

  item->setText(DateColumn, QTime::currentTime().toString());

  QFont font;
  font.setWeight(fontWeight);

  item->setData(MessageColumn, Qt::DecorationRole, QIcon(img));
  item->setTextColor(MessageColumn, color);
  item->setFont(MessageColumn, font);
  item->setText(MessageColumn, message);
}

void MCServerLogWidget::copyToClipboard() {
  int nItems = topLevelItemCount();
  if (nItems == 0) { return; }

  QString s;
  for (int i = 0; i < nItems; ++i) {
    const QTreeWidgetItem* item = topLevelItem(i);
    s += item->text(DateColumn) + " " + item->text(MessageColumn) + "\n";
  }

  MCApp->clipboard()->setText(s, QClipboard::Clipboard);
}

QStyleOptionViewItem MCServerLogWidget::viewOptions() const {
  QStyleOptionViewItem option = QAbstractItemView::viewOptions();
  option.decorationAlignment = Qt::AlignTop;
  option.displayAlignment = Qt::AlignLeft | Qt::AlignTop;
  return option;
}
