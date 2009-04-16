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

#include "MCServer.h"
#include "MCClientPeer.h"

Q_GLOBAL_STATIC(MCServer, MCServerInstance)

MCServer* MCServer::instance() {
  return MCServerInstance();
}

MCServer::MCServer(QObject* parent)
  : QTcpServer(parent)
{}

MCServer::~MCServer()
{}

void MCServer::addClient(MCClientPeer* client) {
  m_lstClientsPeer << client;
}

void MCServer::removeClient(MCClientPeer* client) {
  m_lstClientsPeer.removeAll(client);
}

void MCServer::incomingConnection(int socketDescriptor) {
  MCClientPeer* clientPeer = new MCClientPeer(this);

  // Attach the socket of client peer from the socket descriptor
  if (clientPeer->setSocketDescriptor(socketDescriptor, MCClientPeer::ConnectingState, MCClientPeer::ReadWrite)) {
    // TODO : Check if MCClientThreadManager can add a new thread ?
    // TODO : Set signals to slots
    // TODO : Create a new thread

    return;
  }

  clientPeer->abort();
  delete clientPeer;
}
