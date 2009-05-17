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
#include "Utilities/NetworkInfo.h"

#include "ServerTableIP.h"
#include "ClientThread.h"

MCServerTableIP::MCServerTableIP()
{}

void MCServerTableIP::addClient(MCClientThread* client) {
  AssertCheckPtr(client);

  // Local client
  if (client->isLocalClient()) {
    // Double entries impossible (one port = one socket)
    m_mapLocalClientThreads.insert(client->peerPort(), client);
  }
  // Remote client
  else {
    quint64 hardwareAddress = client->clientInfo().hardwareAddress();

    Assert(hardwareAddress != 0x0);
    Assert(isHardwareAddressRegistered(hardwareAddress) == false);

    quint32 ip = client->peerAddress().toIPv4Address();
    m_mapRemoteClientThreads.insert(hardwareAddress, client);
    m_mapRemoteIP.insert(ip, hardwareAddress);
  }
}

bool MCServerTableIP::isHardwareAddressRegistered(quint64 hardwareAddress) const {
  return m_mapRemoteClientThreads.contains(hardwareAddress);
}

MCClientThread* MCServerTableIP::remoteClient(quint64 hardwareAddress) const {
  return m_mapRemoteClientThreads.value(hardwareAddress, NULL);
}

MCClientThread* MCServerTableIP::localClient(quint16 port) const {
  return m_mapLocalClientThreads.value(port, NULL);
}

void MCServerTableIP::removeClient(MCClientThread* client) {
  AssertCheckPtr(client);

  // Local client
  if (client->isLocalClient()) {
    m_mapLocalClientThreads.remove(client->peerAddress().toIPv4Address());
  }
  // Remote client
  else {
    quint64 hardwareAddress = client->clientInfo().hardwareAddress();

    Assert(hardwareAddress != 0x0);
    Assert(isHardwareAddressRegistered(hardwareAddress) == true);

    m_mapRemoteClientThreads.remove(hardwareAddress);
    m_mapRemoteIP.remove(client->peerAddress().toIPv4Address(), hardwareAddress);
  }
}
