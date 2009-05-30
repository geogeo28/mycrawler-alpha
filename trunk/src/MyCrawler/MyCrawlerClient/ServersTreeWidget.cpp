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

#include <QHostAddress>

#include "Debug/Exception.h"

#include "ServersTreeWidget.h"
#include "ClientApplication.h"
#include "Client.h"
#include "ServerInfo.h"
#include "ServersList.h"

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Server name", 150, false},
  (MyQTreeWidgetHeaderItem){NULL, "IP",           90, false},
  (MyQTreeWidgetHeaderItem){NULL, "Port",         45, false},
  (MyQTreeWidgetHeaderItem){NULL, "Ping",         45, false},
  (MyQTreeWidgetHeaderItem){NULL, "Users",        45, false},
  (MyQTreeWidgetHeaderItem){NULL, "Max users",    45, false},
  (MyQTreeWidgetHeaderItem){NULL, "Priority",     80, false}
};
static const int ColumnsHeaderCount = 7;
static const int ColumnSortedIndex = -1;

void MCServersTreeWidget::cleanAll_() {

}

MCServersTreeWidget::MCServersTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCServersTreeWidget::~MCServersTreeWidget() {
  cleanAll_();
}

void MCServersTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex, Qt::AscendingOrder);
  setPersistentColumnIndex(NameColumn);

  // Construct the list of items based on the servers list
  foreach (const MCServerInfo& serverInfo, MCServersList::instance()->allServers()) {
    newServerItemFromServerInfo_(serverInfo);
  }

  // Signals/slots connections
  QObject::connect(MCServersList::instance(), SIGNAL(serverAdded(const MCServerInfo&)), this, SLOT(slotServerAdded(const MCServerInfo&)));
  QObject::connect(MCServersList::instance(), SIGNAL(serverRemoved(quint32)), this, SLOT(slotServerRemoved(quint32)));
  QObject::connect(MCServersList::instance(), SIGNAL(allServersRemoved()), this, SLOT(slotAllServersRemoved()));
  QObject::connect(MCServersList::instance(), SIGNAL(serverUpdated(const MCServerInfo&)), this, SLOT(slotServerUpdated(const MCServerInfo&)));

  QObject::connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(on_connectToServer()));
}

void MCServersTreeWidget::slotServerAdded(const MCServerInfo& serverInfo) {
  Assert(serverInfo.isValid());

  QTreeWidgetItem* item = newServerItem_();
  m_lstServersManaged.insert(serverInfo.ip().toIPv4Address(), item);
  MCServersTreeWidget::setServerItemFromServerInfo_(item, serverInfo);
}

void MCServersTreeWidget::slotServerRemoved(quint32 ip) {
  QTreeWidgetItem* item = m_lstServersManaged.value(ip);
  AssertCheckPtr(item);
  delete item;
  m_lstServersManaged.remove(ip);
}

void MCServersTreeWidget::slotAllServersRemoved() {
  clear();
  m_lstServersManaged.clear();
}

void MCServersTreeWidget::slotServerUpdated(const MCServerInfo& serverInfo) {
  quint32 ip = serverInfo.ip().toIPv4Address();

  QTreeWidgetItem* item = m_lstServersManaged.value(ip);
  if (item == NULL) { return; }

  MCServersTreeWidget::setServerItemFromServerInfo_(item, serverInfo);
}

void MCServersTreeWidget::on_connectToServer() {
  QTreeWidgetItem* item = currentItem();
  if (item == NULL) {
    return;
  }

  quint32 ip = qVariantValue<quint32>(item->data(IPColumn, Qt::UserRole));
  quint16 port = qVariantValue<quint16>(item->data(PortColumn, Qt::UserRole));
  QString name = item->text(NameColumn);

  MCClientApplication::instance()->mainWindow()->flushServersToConnectList();
  MCApp->loadSettingsProxyConfiguration();
  MCClient::instance()->connectToHost(MCServerInfo(QHostAddress(ip), port, name));
}

void MCServersTreeWidget::on_priorityChanged(QAction* action) {
  AssertCheckPtr(action);

  MCServerInfo::Priority priority = qVariantValue<MCServerInfo::Priority>(action->data());
  foreach (QTreeWidgetItem* item, selectedItems()) {
    MCServerInfo::Priority previewPriority = qVariantValue<MCServerInfo::Priority>(item->data(PriorityColumn, Qt::UserRole));

    if (previewPriority == priority) {
      continue;
    }

    // Change priority
    quint32 ip = qVariantValue<quint32>(item->data(IPColumn, Qt::UserRole));
    MCServerInfo& serverInfo = MCServersList::instance()->fromIp(ip);
    if (serverInfo.isValid()) {
      serverInfo.setPriority(priority);
      item->setData(PriorityColumn, Qt::UserRole, qVariantFromValue<MCServerInfo::Priority>(priority));
      item->setText(PriorityColumn, MCServerInfo::priorityToString(priority));
    }
  }
}

void MCServersTreeWidget::on_remove() {
  // No item selected
  if (currentItem() == NULL) {
    return;
  }

  foreach (QTreeWidgetItem* item, selectedItems()) {
    quint32 ip = qVariantValue<quint32>(item->data(IPColumn, Qt::UserRole));
    MCServersList::instance()->removeServer(ip);
  }
}

void MCServersTreeWidget::on_removeAll() {
  int buttonSelected = QMessageBox::question(
    NULL, QApplication::applicationName(),
    "Do you really want to remove all servers ?",
    QMessageBox::Yes | QMessageBox::No
  );

  if (buttonSelected == QMessageBox::No) { return; }

  MCServersList::instance()->removeAllServers();
}

void MCServersTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
  QMenu menu(this);

  // Create the menu priority
  QMenu* mnuPriority = new QMenu("Priority", &menu);

  QAction* actionPriorities[3] = {
    new QAction("Low", mnuPriority),
    new QAction("Normal", mnuPriority),
    new QAction("High", mnuPriority)
  };

  // Create a group of priorities
  for (int i = 0; i < 3; ++i) {
    actionPriorities[i]->setData(qVariantFromValue<MCServerInfo::Priority>((MCServerInfo::Priority)i));
    mnuPriority->addAction(actionPriorities[i]);
  }

  // Set the current priority of the single selected item
  if (selectedItems().count() == 1) {
    int idx = static_cast<int>(qVariantValue<MCServerInfo::Priority>(currentItem()->data(PriorityColumn, Qt::UserRole)));

    Assert((idx >= 0) && (idx < 3));
    QAction* action = actionPriorities[idx];
    action->setCheckable(true);
    action->setChecked(true);
  }
  QObject::connect(mnuPriority, SIGNAL(triggered(QAction*)), this, SLOT(on_priorityChanged(QAction*)));

  // Construct others actions
  QAction* actionConnect = new QAction("Connect", &menu);
  QObject::connect(actionConnect, SIGNAL(triggered()), this, SLOT(on_connectToServer()));

  QAction* actionRemove = new QAction("Remove the server", &menu);
  QObject::connect(actionRemove, SIGNAL(triggered()), this, SLOT(on_remove()));

  QAction* actionRemoveAll = new QAction("Remove all servers", &menu);
  QObject::connect(actionRemoveAll, SIGNAL(triggered()), this, SLOT(on_removeAll()));

  // No item selected
  if (selectedItems().count() == 0) {
    actionConnect->setEnabled(false);
    mnuPriority->setEnabled(false);
    actionRemove->setEnabled(false);
  }

  menu.addAction(actionConnect);
  menu.addMenu(mnuPriority);
  menu.addAction(actionRemove);
  menu.addAction(actionRemoveAll);

  menu.exec(event->globalPos());
  event->accept();
}

void MCServersTreeWidget::unsetServerItemValues_(QTreeWidgetItem* item) {
  AssertCheckPtr(item);

  // Set columns content
  item->setIcon(NameColumn,     QIcon(":/Servers/ServerIcon"));

  item->setText(PingColumn,     QString());
  item->setText(UsersColumn,    "?");
  item->setText(MaxUsersColumn, "?");

  item->setText(PriorityColumn, "Normal");
}

void MCServersTreeWidget::setServerItemFromServerInfo_(QTreeWidgetItem* item, const MCServerInfo& serverInfo) {
  AssertCheckPtr(item);

  // Set columns data
  item->setData(IPColumn,       Qt::UserRole, qVariantFromValue<quint32>(serverInfo.ip().toIPv4Address()));
  item->setData(PortColumn,     Qt::UserRole, qVariantFromValue<quint16>(serverInfo.port()));
  item->setData(PingColumn,     Qt::UserRole, serverInfo.ping());
  item->setData(UsersColumn,    Qt::UserRole, serverInfo.users());
  item->setData(MaxUsersColumn, Qt::UserRole, serverInfo.maxUsers());
  item->setData(PriorityColumn, Qt::UserRole, qVariantFromValue<MCServerInfo::Priority>(serverInfo.priority()));

  // Set columns content
  item->setText(NameColumn,     serverInfo.name());
  item->setText(IPColumn,       serverInfo.ip().toString());
  item->setText(PortColumn,     QString::number(serverInfo.port()));
  item->setText(PingColumn,     (serverInfo.ping() == -1 ? QString() : QString::number(serverInfo.ping())));
  item->setText(UsersColumn,    (serverInfo.users() == -1 ? "?" : QString::number(serverInfo.users())));
  item->setText(MaxUsersColumn, (serverInfo.maxUsers() == -1 ? "?" : QString::number(serverInfo.maxUsers())));
  item->setText(PriorityColumn, MCServerInfo::priorityToString(serverInfo.priority()));
}

QTreeWidgetItem* MCServersTreeWidget::newServerItem_() {
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  MCServersTreeWidget::unsetServerItemValues_(item);

  return item;
}

void MCServersTreeWidget::newServerItemFromServerInfo_(const MCServerInfo& serverInfo) {
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  m_lstServersManaged.insert(serverInfo.ip().toIPv4Address(), item);
  MCServersTreeWidget::unsetServerItemValues_(item);

  // Set columns content
  MCServersTreeWidget::setServerItemFromServerInfo_(item, serverInfo);
}
