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
#include "MCClientThread.h"
#include "MCClientThreadManager.h"

Q_GLOBAL_STATIC(MCServer, MCServerInstance)

MCServer* MCServer::instance() {
  return MCServerInstance();
}

MCServer::MCServer(QObject* parent)
  : QTcpServer(parent)
{}

MCServer::~MCServer()
{}

void MCServer::addClientPeer(MCClientPeer* client) {
  m_lstClientsPeer << client;
}

void MCServer::removeClientPeer(MCClientPeer* client) {
  m_lstClientsPeer.removeAll(client);
}

void MCServer::incomingConnection(int socketDescriptor) { 
  MCClientThread* clientThread = MCClientThreadManager::instance()->createThread(socketDescriptor);

  // Could not create a new client thread
  if (clientThread==NULL) {

    return;
  }

  // Attachs the new client peer to the server
  MCClientPeer* clientPeer = clientThread->clientPeer();
  addClientPeer(clientPeer);

  // Signals and slots connections and etablishs a communication between the server and the client

}

