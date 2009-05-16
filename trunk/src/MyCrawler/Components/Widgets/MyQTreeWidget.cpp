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

#include <QMenu>
#include <QAction>
#include <QSettings>

#include "Debug/Exception.h"

#include "MyQTreeWidget.h"

MyQTreeWidget::MyQTreeWidget(QWidget* parent)
  : QTreeWidget(parent),
    m_nColumnSortedIndex(-1)
{
  QObject::connect(this->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(slotSortIndicatorChanged_(int,Qt::SortOrder)));
}

MyQTreeWidget::~MyQTreeWidget()
{}

void MyQTreeWidget::showColumnFromAction(bool show) {
  QAction* action = qobject_cast<QAction*>(sender());
  AssertCheckPtr(action);

  int index = action->property("ColumnIndex").toInt();
  setColumnHidden(index, !show);

  if ((show == true) && (columnWidth(index) == 0)) {
    setColumnWidth(index, header()->defaultSectionSize());
  }
}

void MyQTreeWidget::slotSortIndicatorChanged_(int logicalIndex, Qt::SortOrder order) {
  // Sorting disabled
  if ((m_nColumnSortedIndex == logicalIndex) && (order == Qt::AscendingOrder)) {
    sortByColumn(-1, Qt::DescendingOrder);
    m_nColumnSortedIndex = -1;

    return;
  }

  m_nColumnSortedIndex = logicalIndex;
}

void MyQTreeWidget::setupHeader(const MyQTreeWidgetHeaderItem headers[], int nColumns) {
  for (int i = 0; i < nColumns; ++i) {
    const MyQTreeWidgetHeaderItem& item = headers[i];

    if (item.name != NULL) { headerItem()->setText(i, item.name); }
    if (item.icon != NULL) { headerItem()->setIcon(i, QIcon(item.icon)); }
    if (item.width > -1)   { setColumnWidth(i, item.width); }

    setColumnHidden(i, item.hidden);
  }
}

QMenu* MyQTreeWidget::columnsMenu() {
  QMenu* menu = new QMenu("Columns", this);

  // Columns
  for (int i = 0; i < columnCount(); ++i) {
    QAction* action = new QAction(headerItem()->icon(i), headerItem()->text(i), this);
    action->setProperty("ColumnIndex", i);
    action->setCheckable(true);
    action->setChecked(!isColumnHidden(i));
    QObject::connect(action, SIGNAL(toggled(bool)), this, SLOT(showColumnFromAction(bool)));

    menu->addAction(action);
  }

  return menu;
}

void MyQTreeWidget::setupContextMenu() {
  QMenu *menu = new QMenu(this);
  menu->addMenu(columnsMenu());

  setContextMenu(menu);
}

void MyQTreeWidget::contextMenuEvent(QContextMenuEvent *event) {
  if (!m_pContextMenu.isNull()) {
    m_pContextMenu->exec(event->globalPos());
    event->accept();
    return;
  }
  
  event->ignore();
}
