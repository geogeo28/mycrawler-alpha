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

#include "ClientsTreeWidget.h"
#include "ServerApplication.h"

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Internal ID",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "MAC address",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "State",        -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Peer address", -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Peer Port",    -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Host name",    -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Host domain",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "IP",           -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Broadcast",    -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Netmask",      -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Status",       -1, false}
};
static const int ColumnsHeaderCount = 11;
static const int ColumnSortedIndex = -1;

void MCClientsTreeWidget::loadSettings_() {
  loadLayout(MCSettings, "MCClientsTreeWidget");
}

void MCClientsTreeWidget::saveSettings_() {
  saveLayout(MCSettings, "MCClientsTreeWidget");
}

void MCClientsTreeWidget::cleanAll_() {

}

MCClientsTreeWidget::MCClientsTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCClientsTreeWidget::~MCClientsTreeWidget() {
  saveSettings_();
  cleanAll_();
}

void MCClientsTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex);
  loadSettings_();
  setupContextMenu();
}
