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

#include "Server.h"
#include "ClientPeer.h"
#include "ClientThread.h"
#include "ClientThreadManager.h"

Q_GLOBAL_STATIC(MCServer, MCServerInstance)        

MCServer* MCServer::instance() {
  return MCServerInstance();
}

MCServer::MCServer(QObject* parent)
  : QTcpServer(parent)
{
  setError_(NoError);
}

bool MCServer::canAcceptNewConnection() const {
  return (m_lstClientThreads.size() < maxConnections());
}

void MCServer::errorClient_(MCClientThread::Error error) {
  MCClientThread* client = qobject_cast<MCClientThread*>(this->sender());
  emit errorClient(client, error);
}

void MCServer::finishedClient_() {
  MCClientThread* client = qobject_cast<MCClientThread*>(this->sender());
  emit finishedClient(client);
}

void MCServer::incomingConnection(int socketDescriptor) { 
  //MCClientThread* clientThread = MCClientThreadManager::instance()->createThread(socketDescriptor);

  // Could not create a new client thread
  //if (clientThread == NULL) {
  //
  //  return;
  //}

  // Attachs the new client peer to the server
  //MCClientPeer* clientPeer = clientThread->clientPeer();
  //addClientPeer(clientPeer);

  // Signals and slots connections and etablishs a communication between the server and the client

  //clientThread->start();
}

void MCServer::setError_(Error error, bool signal) {
  m_enumError = error;

  switch (error) {
    case NoError:
      m_sError = QT_TRANSLATE_NOOP(MCServer, "No error");
      break;
    case ServerFull:
      m_sError = QT_TRANSLATE_NOOP(MCServer, "The server is full. You must increase the variable MAX_CONNECTIONS to accept new clients.");
      break;
    default:
      m_enumError = UnknownError;
      m_sError = QT_TRANSLATE_NOOP(MCServer, "Unknown error");
      break;
  }

  if (signal == true) {
    emit MCServer::error(m_enumError);
  }
}
