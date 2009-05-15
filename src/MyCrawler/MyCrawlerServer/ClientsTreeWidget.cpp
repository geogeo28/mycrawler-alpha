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

#include "Debug/Exception.h"

#include "ClientsTreeWidget.h"
#include "ServerApplication.h"

typedef struct {
  const char* name;
  bool hide;
} HeaderItem;

static HeaderItem Headers[] = {
  (HeaderItem){"Internal ID",  true},
  (HeaderItem){"MAC address",  true},
  (HeaderItem){"State",        false},
  (HeaderItem){"Peer address", false},
  (HeaderItem){"Peer Port",    false},
  (HeaderItem){"Host name",    true},
  (HeaderItem){"Host domain",  true},
  (HeaderItem){"IP",           true},
  (HeaderItem){"Broadcast",    true},
  (HeaderItem){"Netmask",      true},
  (HeaderItem){"Status",       false}
};
static const int HeaderSortIndex = -1;

void MCClientsTreeWidget::setupHeader_() {
  header()->setMovable(true);
  header()->setResizeMode(QHeaderView::Interactive);

  for (unsigned int i = 0; i < (sizeof(Headers) / sizeof(HeaderItem)); ++i) {
    const HeaderItem& item = Headers[i];

    headerItem()->setText(i, item.name);
    setColumnHidden(i, item.hide);
  }

  sortByColumn(HeaderSortIndex, Qt::AscendingOrder);
}

void MCClientsTreeWidget::loadSettings_() {
  // Columns
  MCSettings->beginGroup("ClientsTreeWidget");
    MCSettings->beginReadArray("Columns");
      for (int i = 0; i < columnCount(); ++i) {
        MCSettings->setArrayIndex(i);

        setColumnHidden(i, MCSettings->value("Hidden", isColumnHidden(i)).toBool());
        setColumnWidth(i, MCSettings->value("Width", columnWidth(i)).toInt());

        int visualIndex = MCSettings->value("Position", i).toInt();
        if (visualIndex != i) {
          header()->swapSections(header()->visualIndex(i), visualIndex);
        }
      }

      sortByColumn(
        MCSettings->value("ColumnSortedIndex", HeaderSortIndex).toInt(),
        (Qt::SortOrder)MCSettings->value("ColumnSortedOrder", Qt::AscendingOrder).toInt()
      );
    MCSettings->endArray();
  MCSettings->endGroup();
}

void MCClientsTreeWidget::saveSettings_() {
  // Columns
  MCSettings->beginGroup("ClientsTreeWidget");
    MCSettings->beginReadArray("Columns");
      for (int i = 0; i < columnCount(); ++i) {
        MCSettings->setArrayIndex(i);

        MCSettings->setValue("Hidden", isColumnHidden(i));
        MCSettings->setValue("Width", columnWidth(i));

        MCSettings->setValue("Position", header()->visualIndex(i));
      }

      MCSettings->setValue("ColumnSortedIndex", header()->sortIndicatorSection());
      MCSettings->setValue("ColumnSortedOrder", header()->sortIndicatorOrder());
    MCSettings->endArray();
  MCSettings->endGroup();
}

void MCClientsTreeWidget::cleanAll_() {

}

MCClientsTreeWidget::MCClientsTreeWidget(QWidget* parent)
  : QTreeWidget(parent)
{}

MCClientsTreeWidget::~MCClientsTreeWidget() {
  saveSettings_();
  cleanAll_();
}

void MCClientsTreeWidget::setup() {
  setupHeader_();
  loadSettings_();

  createContextMenu_();
}

void MCClientsTreeWidget::slotShowColumn_(bool show) {
  QAction* action = qobject_cast<QAction*>(sender());
  AssertCheckPtr(action);

  int index = action->property("ColumnIndex").toInt();
  setColumnHidden(index, !show);

  if ((show == true) && (columnWidth(index) == 0)) {
    setColumnWidth(index, header()->defaultSectionSize());
  }
}


void MCClientsTreeWidget::contextMenuEvent(QContextMenuEvent *event) {
  if (!m_pContextMenu.isNull()) {
    m_pContextMenu->exec(event->globalPos());
    event->accept();
  }
}

void MCClientsTreeWidget::createContextMenu_() {
  m_pContextMenu = new QMenu(this);

  // Context menu
  QMenu* menuColumns = new QMenu("Columns", this);
  m_pContextMenu->addMenu(menuColumns);

  // Columns
  for (int i = 0; i < columnCount(); ++i) {
    QAction* action = new QAction(Headers[i].name, this);
    action->setProperty("ColumnIndex", i);
    action->setCheckable(true);
    action->setChecked(!isColumnHidden(i));
    QObject::connect(action, SIGNAL(toggled(bool)), this, SLOT(slotShowColumn_(bool)));

    menuColumns->addAction(action);
  }
}
