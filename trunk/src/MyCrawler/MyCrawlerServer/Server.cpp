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
#include "Debug/Logger.h"

#include "Server.h"

MCServer* MCServer::s_instance = NULL;

int MaxConnections = 5;

MCServer* MCServer::instance() {
  if (s_instance == NULL) {
    s_instance = new MCServer();
  }

  return s_instance;
}

void MCServer::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

MCServer::MCServer(QObject* parent)
  : QTcpServer(parent)
{
  ILogger::Debug() << "Construct.";

  setError_(NoError);
}

MCServer::~MCServer() {
  ILogger::Debug() << "Delete all threads.";
  QListIterator<MCClientThread*> it(m_lstClientThreads);
  while (it.hasNext()) {
    MCClientThread* client = it.next();

    ILogger::Debug() << "Attempt to destroy the thread " << client << ".";
    client->quit();
    client->wait();
    client->deleteLater();
  }
  ILogger::Debug() << "Destroyed.";
}

int MCServer::maxConnections() const {
  return MaxConnections;
}

void MCServer::setMaxConnections(int n) {
  Assert(n > 0);

  MaxConnections = n;
}

bool MCServer::canAcceptNewConnection() const {
  return (m_lstClientThreads.count() < maxConnections());
}

bool MCServer::addClient(MCClientThread* client) {
  if (canAcceptNewConnection() == false) {
    setError_(ServerFullError, false);
    return false;
  }

  ILogger::Debug() << "Add a new client " << client << ".";
  m_lstClientThreads << client;

  return true;
}

void MCServer::removeClient(MCClientThread* client) {
  ILogger::Debug() << "Remove the client " << client << ".";
  m_lstClientThreads.removeAll(client);
}

void MCServer::clientError_(MCClientThread::Error error) {
  MCClientThread* client = qobject_cast<MCClientThread*>(this->sender());
  emit clientError(client, error);
}

void MCServer::clientConnectionStateChanged_(MCClientThread::ConnectionState state) {
  MCClientThread* client = qobject_cast<MCClientThread*>(this->sender());
  emit clientConnectionStateChanged(client, state);
}

void MCServer::clientDisconnected_() { 
  MCClientThread* client = qobject_cast<MCClientThread*>(this->sender());
  ILogger::Debug() << "The thread " << client << " is finished (client disconnected).";

  removeClient(client);
  client->deleteLater();
}

void MCServer::incomingConnection(int socketDescriptor) { 
  ILogger::Trace() << "Server : New incoming connection.";

  // The new client cannot be accepted
  if (canAcceptNewConnection() == false) {
    MCClientPeer client;

    // Refuse connection
    if (client.setSocketDescriptor(socketDescriptor) == true) {
      client.connectionRefused();
    }

    setError_(ServerFullError, true);
    return;
  }

  // Create a new thread
  MCClientThread* client = new MCClientThread(socketDescriptor, this);

  // Setup signals/slots connections
  qRegisterMetaType<MCClientThread::Error>("MCClientThread::Error");
  qRegisterMetaType<MCClientThread::ConnectionState>("MCClientThread::ConnectionState");

  QObject::connect(client, SIGNAL(error(MCClientThread::Error)), this, SLOT(clientError_(MCClientThread::Error)));
  QObject::connect(client, SIGNAL(connectionStateChanged(MCClientThread::ConnectionState)), this, SLOT(clientConnectionStateChanged_(MCClientThread::ConnectionState)));
  QObject::connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected_()));

  // Add the client in the server
  addClient(client);

  // Run the thread (Start listening activities on the socket)
  client->start();
}

void MCServer::setError_(Error error, bool signal) {
  m_enumError = error;

  switch (error) {
    case NoError:
      m_sError = QT_TRANSLATE_NOOP(MCServer, "No error");
      break;
    case ServerFullError:
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
