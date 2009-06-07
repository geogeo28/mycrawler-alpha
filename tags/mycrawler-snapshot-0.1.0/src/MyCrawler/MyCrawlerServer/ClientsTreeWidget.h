/****************************************************************************
 * @(#) MyCrawler server. Tree widget of clients.
 * GUI interface
 *
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

#ifndef CLIENTSTREEWIDGET_H
#define CLIENTSTREEWIDGET_H

#include <QList>

#include "Widgets/MyQTreeWidget.h"
#include "Utilities/NetworkInfo.h"

#include "ClientThread.h"

class MCClientsTreeWidget : public MyQTreeWidget
{
    Q_OBJECT

public:
    enum {
      ThreadIdColumn,
      HardwareAddressColumn,
      StateColumn,
      PeerAddressColumn,
      PeerPortColumn,
      HostNameColumn,
      HostDomainColumn,
      IPColumn,
      GatewayColumn,
      BroadcastColumn,
      NetmaskColumn,
      StatusColumn
    };

private:
    void cleanAll_();

public:
    MCClientsTreeWidget(QWidget* parent = NULL);
    ~MCClientsTreeWidget();

    void setup();

private slots:
    void slotClientConnectionStateChanged_(MCClientThread* client, MCClientThread::ConnectionState state);
    void slotClientConnected_(MCClientThread* client, bool unknownClientInTheServerHistory);
    void slotClientDisconnected_(MCClientThread* client);

private slots:
    void on_forceToDisconnectClient_();
    void on_removeClient_();

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private:
    static void unsetClientItemValues_(QTreeWidgetItem* item, bool previouslyConnected = true);
    static void setClientItemValues_(QTreeWidgetItem* item, MCClientThread* client);
    static void setClientItemFromNetworkInfo_(QTreeWidgetItem* item, const CNetworkInfo& networkInfo);

private:
    QTreeWidgetItem* newClientItem_();
    void newClientItemFromNetworkInfo_(const CNetworkInfo& networkInfo);

private:
    typedef QMap<quint64, QTreeWidgetItem*> ClientsManagedList; // clientId, item
    typedef QMap<quint64, QTreeWidgetItem*> ItemsRefByHAddrList; // hardwareAddress, item

    ClientsManagedList m_lstClientsManaged;
    ItemsRefByHAddrList m_lstItemsRefByHAddr;
};

#endif // CLIENTSTREEWIDGET_H
