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
#include "Server.h"

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Thread ID",    -1, true},
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

  // Setup signals/slots connections
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionStateChanged(MCClientThread*,MCClientThread::ConnectionState)), this, SLOT(slotClientConnectionStateChanged(MCClientThread*,MCClientThread::ConnectionState)));
}

void MCClientsTreeWidget::slotClientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state) {
  /*************************
   Authenticating
   *************************/
  if (state == MCClientThread::AuthenticatingState) {
    QTreeWidgetItem* item = newClientItem_();

    item->setIcon(StateColumn, QIcon(":/Clients/UnauthenticatedIcon"));
    item->setText(StateColumn, "Unknown");

    item->setText(ThreadIdColumn,  "0x" + QString::number((int)client, 16));
    item->setText(PeerAddressColumn, client->threadInfo().peerAddress().toString());
    item->setText(PeerPortColumn,    QString::number(client->threadInfo().peerPort()));
    item->setText(StatusColumn,      "Authenticating...");

    m_lstClientsManaged.insert(client, item);
  }
  // Other states
  else {
    const CNetworkInfo& networkInfo = client->threadInfo().networkInfo();

    QTreeWidgetItem* item = m_lstClientsManaged.value(client);
    AssertCheckPtr(item);

    switch (state) {
      /*************************
       Connected
       *************************/
      case MCClientThread::ConnectedState:
      {
        item->setData(HardwareAddressColumn, Qt::UserRole, networkInfo.hardwareAddress());

        // Local
        if (networkInfo.hardwareAddress() == 0) {
          item->setIcon(StateColumn, QIcon(":/Clients/ConnectedLocalIcon"));
          item->setText(StateColumn, "Local");
        }
        // Remote
        else {
          // Find unmanaged item with the same hardware address
          foreach (QTreeWidgetItem* unmanagedItem, m_lstClientsUnmanaged) {
            QVariant hAddr = unmanagedItem->data(HardwareAddressColumn, Qt::UserRole);
            if (hAddr.isValid() && (hAddr.toULongLong() == networkInfo.hardwareAddress())) {
              // Unmanaged => Managed
              m_lstClientsManaged.remove(client);
              delete item;

              item = unmanagedItem;
              item->setText(ThreadIdColumn,  "0x" + QString::number((int)client, 16));
              item->setText(PeerAddressColumn, client->threadInfo().peerAddress().toString());
              item->setText(PeerPortColumn,    QString::number(client->threadInfo().peerPort()));

              break;
            }
          }

          // Change status
          item->setIcon(StateColumn, QIcon(":/Clients/ConnectedRemoteIcon"));
          item->setText(StateColumn, "Remote");
        }

        item->setText(HardwareAddressColumn, networkInfo.hardwareAddressString());
        item->setText(HostNameColumn,        networkInfo.hostName());
        item->setText(HostDomainColumn,      networkInfo.hostDomain());
        item->setText(IPColumn,              networkInfo.ip().toString());
        item->setText(BroadcastColumn,       networkInfo.broadcast().toString());
        item->setText(NetmaskColumn,         networkInfo.netmask().toString());

        item->setText(StatusColumn,          "Connected");
      }
      break;


      /*************************
       Closing
       *************************/
      case MCClientThread::ClosingState:
      {
        item->setText(StatusColumn, "Closing...");
      }
      break;


      /*************************
       Unconnected
       *************************/
      case MCClientThread::UnconnectedState:
      {
        item->setIcon(StateColumn, QIcon(":/Clients/UnconnectedIcon"));
        item->setText(StateColumn, "Unconnected");

        item->setText(StatusColumn, "Unconnected");

        m_lstClientsManaged.remove(client);

        // Local computer, delete item
        if (networkInfo.hardwareAddress() == 0) {
          delete item;
        }
        // Remote computer, unmanged item
        else {
          m_lstClientsUnmanaged.append(item);
        }
      }
      break;

      default:;
    }
  }
}

QTreeWidgetItem* MCClientsTreeWidget::newClientItem_() {
  QTreeWidgetItem* item = new QTreeWidgetItem(this);

  item->setIcon(StateColumn, QIcon(":/Clients/UnauthenticatedIcon"));
  item->setText(StateColumn, "Unknown");

  item->setText(PeerAddressColumn, "0.0.0.0");
  item->setText(PeerPortColumn, "0");

  return item;
}
