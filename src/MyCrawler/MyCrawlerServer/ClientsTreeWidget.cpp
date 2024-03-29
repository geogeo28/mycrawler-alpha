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

#include "ClientsTreeWidget.h"
#include "ServerApplication.h"
#include "Server.h"
#include "ServerHistory.h"

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Thread ID",    -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "MAC Address",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "State",        -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Peer Address", -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Peer Port",    -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Host Name",    -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Host Domain",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "IP",           -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Gateway",      -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Broadcast",    -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Netmask",      -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Status",       -1, false}
};
static const int ColumnsHeaderCount = 12;
static const int ColumnSortedIndex = -1;

void MCClientsTreeWidget::cleanAll_() {

}

MCClientsTreeWidget::MCClientsTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCClientsTreeWidget::~MCClientsTreeWidget() {
  cleanAll_();
}

void MCClientsTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);  
  sortByColumn(ColumnSortedIndex, Qt::AscendingOrder);
  setPersistentColumnIndex(StateColumn);

  // Construct the list of items based on the server history
  foreach (const CNetworkInfo& networkInfo, MCServerHistory::instance()->allClients()) {
    newClientItemFromNetworkInfo_(networkInfo);
  }

  // Setup signals/slots connections
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionStateChanged(MCClientThread*,MCClientThread::ConnectionState)), this, SLOT(slotClientConnectionStateChanged_(MCClientThread*,MCClientThread::ConnectionState)));
  QObject::connect(MCServer::instance(), SIGNAL(clientConnected(MCClientThread*,bool)), this, SLOT(slotClientConnected_(MCClientThread*,bool)));
  QObject::connect(MCServer::instance(), SIGNAL(clientDisconnected(MCClientThread*)), this, SLOT(slotClientDisconnected_(MCClientThread*)));
}

void MCClientsTreeWidget::slotClientConnectionStateChanged_(MCClientThread* client, MCClientThread::ConnectionState state) {
  switch (state) {
    /*************************
     Authenticating
     *************************/
    case MCClientThread::AuthenticatingState:
    {
      QTreeWidgetItem* item = newClientItem_();
      m_lstClientsManaged.insert(client->id(), item);
      if (client->isRemoteClient() == true) {
        m_lstItemsRefByHAddr.insert(client->networkInfo().hardwareAddress(), item);
      }

      // Set columns content
      MCClientsTreeWidget::setClientItemValues_(item, client);

      item->setIcon(StateColumn, QIcon(":/Clients/UnauthenticatedIcon"));
      item->setText(StateColumn, "Unknown");

      item->setText(StatusColumn, "Authenticating...");
    }
    break;

    /*************************
     Closing
     *************************/
    case MCClientThread::ClosingState:
    {
      QTreeWidgetItem* item = m_lstClientsManaged.value(client->id());
      AssertCheckPtr(item);

      // Set columns content
      item->setText(StatusColumn, "Closing...");
    }
    break;

    default:;
  }
}

void MCClientsTreeWidget::slotClientConnected_(MCClientThread* client, bool unknownClientInTheServerHistory) {
  AssertCheckPtr(client);

  QTreeWidgetItem* item = m_lstClientsManaged.value(client->id());
  AssertCheckPtr(item);

  quint64 hAddr = client->networkInfo().hardwareAddress();

  // Client previously in the server history (cannot be a local client
  if (unknownClientInTheServerHistory == false) {
    delete item;
    m_lstClientsManaged.remove(client->id());

    item = m_lstItemsRefByHAddr.value(hAddr);
    AssertCheckPtr(item);

    // Erase the preview item attached with this client
    m_lstClientsManaged.insert(client->id(), item);
  }

  m_lstItemsRefByHAddr.insert(hAddr, item);

  // Set columns content
  MCClientsTreeWidget::setClientItemValues_(item, client);

  // Local
  if (client->isLocalClient() == true) {
    item->setIcon(StateColumn, QIcon(":/Clients/ConnectedLocalIcon"));
    item->setText(StateColumn, "Local");
  }
  // Remote
  else {
    item->setIcon(StateColumn, QIcon(":/Clients/ConnectedRemoteIcon"));
    item->setText(StateColumn, "Remote");
  }

  item->setText(StatusColumn, "Connected");
}

void MCClientsTreeWidget::slotClientDisconnected_(MCClientThread* client) {
  AssertCheckPtr(client);

  QTreeWidgetItem* item = m_lstClientsManaged.value(client->id());
  AssertCheckPtr(item);

  m_lstClientsManaged.remove(client->id());

  // No trace
  if ((client->isAuthenticated() == false)
   || (client->isConnectionRefused() == true)
   || (client->isLocalClient() == true))
  {
    delete item;
    return;
  }

  // Keep a trace (image of the server history)
  MCClientsTreeWidget::unsetClientItemValues_(item, true);
}

void MCClientsTreeWidget::on_forceToDisconnectClient_() {
  if (selectedItems().isEmpty()) { return; }

  QTreeWidgetItem* item = selectedItems().at(0);
  MCClientThread* client = MCServer::instance()->clientFromId(item->data(ThreadIdColumn, Qt::UserRole).toULongLong());

  // Managed client
  if (client != NULL) {
    MCServer::instance()->removeClient(client);
  }
}

void MCClientsTreeWidget::on_removeClient_() {
  if (selectedItems().isEmpty()) { return; }

  QTreeWidgetItem* item = selectedItems().at(0);
  MCClientThread* client = MCServer::instance()->clientFromId(item->data(ThreadIdColumn, Qt::UserRole).toULongLong());

  // Unmanaged client
  if (client == NULL) {
    quint64 hAddr = item->data(HardwareAddressColumn, Qt::UserRole).toULongLong();
    if (hAddr == 0x0) { return; } // Impossible case, normaly !

    m_lstItemsRefByHAddr.remove(hAddr);
    MCServerHistory::instance()->removeClient(hAddr);
    delete item;
  }
}

void MCClientsTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
  QTreeWidgetItem* item = itemAt(event->pos());

  // General context menu
  if (item == NULL) {
    MyQTreeWidget::contextMenuEvent(event);
    return;
  }

  // Item context menu
  QMenu menu(this);
  MCClientThread* client = MCServer::instance()->clientFromId(item->data(ThreadIdColumn, Qt::UserRole).toULongLong());

  // Unmanaged client
  if (client == NULL) {
    menu.addAction("Remove", this, SLOT(on_removeClient_()));
  }
  // Managed client
  else {
    menu.addAction("Force to disconnect", this, SLOT(on_forceToDisconnectClient_()));
  }

  menu.exec(event->globalPos());
  event->accept();
}

void MCClientsTreeWidget::unsetClientItemValues_(QTreeWidgetItem* item, bool previouslyConnected) {
  AssertCheckPtr(item);

  item->setData(ThreadIdColumn,    Qt::UserRole, (quint64)0);

  // Set columns content
  item->setText(ThreadIdColumn,    QString());
  item->setText(PeerPortColumn,    QString());

  if (previouslyConnected == true) {
    item->setIcon(StateColumn, QIcon(":/Clients/UnconnectedIcon"));
    item->setText(StateColumn, "Unconnected");
  }
  else {
    item->setIcon(StateColumn, QIcon(":/Clients/UnauthenticatedIcon"));
    item->setText(StateColumn, "Unknown");
  }

  item->setText(StatusColumn, "Unconnected");
}

void MCClientsTreeWidget::setClientItemValues_(QTreeWidgetItem* item, MCClientThread* client) {
  AssertCheckPtr(item);
  AssertCheckPtr(client);

  MCClientsTreeWidget::setClientItemFromNetworkInfo_(item, client->networkInfo());

  item->setData(ThreadIdColumn, Qt::UserRole, QVariant::fromValue(client->id()));

  // Set columns content
  item->setText(ThreadIdColumn,    "0x" + QString::number(client->id(), 16));
  item->setText(PeerAddressColumn, client->peerAddress().toString());
  item->setText(PeerPortColumn,    QString::number(client->peerPort()));
}

void MCClientsTreeWidget::setClientItemFromNetworkInfo_(QTreeWidgetItem* item, const CNetworkInfo& networkInfo) {
  // Set column HardwareAddress
  if (networkInfo.hardwareAddress() != 0x0) {
    item->setData(HardwareAddressColumn, Qt::UserRole, networkInfo.hardwareAddress());
    item->setText(HardwareAddressColumn, networkInfo.hardwareAddressString());
  }

  // Set columns content
  item->setText(PeerAddressColumn, networkInfo.peerAddress().toString());
  item->setText(HostNameColumn,    networkInfo.hostName());
  item->setText(HostDomainColumn,  networkInfo.hostDomain());
  item->setText(IPColumn,          networkInfo.ip().toString());
  item->setText(GatewayColumn,     networkInfo.gateway().toString());
  item->setText(BroadcastColumn,   networkInfo.broadcast().toString());
  item->setText(NetmaskColumn,     networkInfo.netmask().toString());
}

QTreeWidgetItem* MCClientsTreeWidget::newClientItem_() {
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  MCClientsTreeWidget::unsetClientItemValues_(item, false);

  return item;
}

void MCClientsTreeWidget::newClientItemFromNetworkInfo_(const CNetworkInfo& networkInfo) {
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  m_lstItemsRefByHAddr.insert(networkInfo.hardwareAddress(), item);
  MCClientsTreeWidget::unsetClientItemValues_(item, true);

  // Set columns content
  MCClientsTreeWidget::setClientItemFromNetworkInfo_(item, networkInfo);
}
