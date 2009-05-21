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

#include <QCoreApplication>

#include "Debug/Exception.h"
#include "Debug/Logger.h"

#include "Server.h"
#include "ServerHistory.h"

const int DefaultMaxConnections = 5;

MCServer* MCServer::s_instance = NULL;

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
  : QTcpServer(parent),
    m_nMaxConnections(DefaultMaxConnections),
    m_enumState(ClosedState),
    m_listenAddress(QHostAddress::Any),
    m_u16ListenPort(0)
{
  ILogger::Debug() << "Construct.";

  setError_(NoError);
}

MCServer::~MCServer() {
  MCServer::close();
  waitForClosed();
  ILogger::Debug() << "Destroyed.";
}

bool MCServer::canAcceptNewConnection() const {
  return (countClients() < m_nMaxConnections);
}

bool MCServer::listen() {
  ILogger::Debug() << QString("Try to listening the address %1 on the port %2.")
                      .arg(listenAddress().toString())
                      .arg(listenPort());

  if (state() == ClosingState) {
    setError_(ListenError, false);
    return false;
  }

  bool success = QTcpServer::listen(listenAddress(), listenPort());
  if (success == false) {
    setError_(TcpSocketError, false);
    return false;
  }

  setState_(ListeningState);

  return success;
}

void MCServer::close() {
  if (isClosed()) { return; }

  #ifdef QT_DEBUG
    if (countClients() > 0) {
      ILogger::Debug() << "Disconnect all clients.";
    }
  #endif

  setState_(ClosingState);
  // Just close the connection
  if (m_lstClientThreads.isEmpty()) {
    QTcpServer::close();
    setState_(ClosedState);
  }
  // Force to disconnect all clients
  else {
    ClientThreadsList::Iterator it = m_lstClientThreads.begin();
    for (; it != m_lstClientThreads.end(); ++it) {
      removeClient(*it);
    }
  }
}

void MCServer::waitForClosed() const {
  if (isClosed()) { return; }

  ILogger::Debug() << "Waiting to disconnect all client connections...";
  while (!isClosed()) {
    QCoreApplication::processEvents();
  }
}


bool MCServer::addClient(MCClientThread* client) {
  if (canAcceptNewConnection() == false) {
    setError_(ServerFullError, false);
    return false;
  }

  ILogger::Debug() << "Add a new client " << client << ".";
  m_lstClientThreads.insert(client);

  return true;
}

void MCServer::removeClient(MCClientThread* client) {
  AssertCheckPtr(client);

  ILogger::Debug() << "Force to disconnect the client " << client << ".";
  client->quit();
}

MCClientThread* MCServer::clientFromId(quint64 clientId) {
  MCClientThread* client = (MCClientThread*)(unsigned int)clientId;

  if (m_lstClientThreads.contains(client) == false) {
    return NULL;
  }

  return client;
}

int MCServer::defaultMaxConnections() {
  return DefaultMaxConnections;
}

QString MCServer::stateToString(State state) {
  switch (state) {
    case ListeningState: return QT_TRANSLATE_NOOP(MCServer, "Listening");
    case ClosingState:   return QT_TRANSLATE_NOOP(MCServer, "Closing");

    default:
      return QT_TRANSLATE_NOOP(MCServer, "Closed");
  }
}

void MCServer::clientError_(MCClientThread::Error error) {
  MCClientThread* client = senderClientThread_();
  AssertCheckPtr(client);

  emit clientError(client, error);
}

void MCServer::clientTimeout_(MCClientPeer::TimeoutNotify notifiedWhen) {
  MCClientThread* client = senderClientThread_();
  AssertCheckPtr(client);

  emit clientTimeout(client, notifiedWhen);
}

void MCServer::clientErrorProcessingPacket_(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted) {
  MCClientThread* client = senderClientThread_();
  AssertCheckPtr(client);

  emit clientErrorProcessingPacket(client, error, type, size, aborted);
}

void MCServer::clientConnectionStateChanged_(MCClientThread::ConnectionState state) {
  MCClientThread* client = senderClientThread_();
  AssertCheckPtr(client);

  switch (state) {
    /**********
     Connected
     **********/
    case MCClientThread::ConnectedState:
    {
      quint64 hAddr = client->networkInfo().hardwareAddress();

      // Remote connection
      if (hAddr != 0x0) {
        // Check if the client is already connected to the server (two instances forbidden)
        if (isHardwareAddressRegistered(hAddr) == true) {
          refuseClientConnection_(client, "User already connected");
          return;
        }
      }
      // Local client
      else {
        // It's not a Localhost IP
        if (client->isLocalClient() == false) {
          refuseClientConnection_(client, "A MAC address attribued to a local client isn't possible");
          return;
        }
      }

      // Register client IP
      m_lstClientsIP.addClient(client);
    }
    break;

    /************
     Unconnected
     ************/
    case MCClientThread::UnconnectedState:
    {
      if ((client->isAuthenticated() == true) && (client->isConnectionRefused() == false)) {
        // Remove client IP
        m_lstClientsIP.removeClient(client);

        // Add the client in the server history
        if (client->isRemoteClient() == true) {
          MCServerHistory::instance()->addClient(client);
        }
      }
    }
    break;

    default:;
  }

  emit clientConnectionStateChanged(client, state);

  // Connected
  if (state == MCClientThread::ConnectedState) {
    bool unknownClientInTheServerHistory = true;

    // Manage client in the server history
    if (client->isRemoteClient() == true) {
      quint64 hAddr = client->networkInfo().hardwareAddress();

      unknownClientInTheServerHistory = !MCServerHistory::instance()->isHardwareAddressRegistered(hAddr);

      // Remove the client in the server history
      if (unknownClientInTheServerHistory == false) {
        MCServerHistory::instance()->removeClient(hAddr);
      }
    }

    emit clientConnected(client, unknownClientInTheServerHistory);
  }
}

void MCServer::clientDisconnected_() {
  MCClientThread* client = senderClientThread_();
  AssertCheckPtr(client);

  emit clientDisconnected(client);

  ILogger::Debug() << "Prepare to finish the thread " << client << " (client disconnected).";
  client->quit();
}

void MCServer::clientFinished_() {
  MCClientThread* client = senderClientThread_();
  AssertCheckPtr(client);

  emit clientFinished(client);

  ILogger::Debug() << "The thread " << client << " is finished (Remove client from the server)";
  m_lstClientThreads.remove(client);
  client->deleteLater();

  if ((state() == ClosingState) && m_lstClientThreads.isEmpty()) {
    ILogger::Debug() << "Close the server connection.";
    QTcpServer::close();
    setState_(ClosedState);
  }
}

void MCServer::incomingConnection(int socketDescriptor) { 
  ILogger::Trace() << "Server : New incoming connection.";

  // New client cannot be accepted
  if (canAcceptNewConnection() == false) {
    MCClientPeer client;

    // Refuse connection
    if (client.setSocketDescriptor(socketDescriptor) == true) {
      client.close();
    }

    setError_(ServerFullError, true);
    return;
  }

  // Create a new thread
  MCClientThread* client = new MCClientThread(socketDescriptor, this);

  // Setup signals/slots connections
  qRegisterMetaType<MCClientThread::Error>("MCClientThread::Error");
  qRegisterMetaType<MCClientPeer::TimeoutNotify>("MCClientPeer::TimeoutNotify");
  qRegisterMetaType<MCClientPeer::PacketType>("MCClientPeer::PacketType");
  qRegisterMetaType<MCClientPeer::PacketError>("MCClientPeer::PacketError");
  qRegisterMetaType<MCClientThread::ConnectionState>("MCClientThread::ConnectionState");

  QObject::connect(client, SIGNAL(error(MCClientThread::Error)), this, SLOT(clientError_(MCClientThread::Error)));
  QObject::connect(client, SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SLOT(clientTimeout_(MCClientPeer::TimeoutNotify)));
  QObject::connect(client, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SLOT(clientErrorProcessingPacket_(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
  QObject::connect(client, SIGNAL(connectionStateChanged(MCClientThread::ConnectionState)), this, SLOT(clientConnectionStateChanged_(MCClientThread::ConnectionState)));
  QObject::connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected_()));
  QObject::connect(client, SIGNAL(finished()), this, SLOT(clientFinished_()));

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
    case TcpSocketError:
      m_sError = QTcpServer::errorString();
      break;
    case ServerFullError:
      m_sError = QT_TRANSLATE_NOOP(MCServer, "Could not accept a new client because the server is full.\nYou must increase the number of connections than the server can supported.\nNew client refused.");
      break;
    case ListenError:
      m_sError = QT_TRANSLATE_NOOP(MCServer, "Listen is not allowed in Closing State.");
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

void MCServer::setState_(State state) {
  // Do nothing if the connection state didn't changed
  if (state == m_enumState) { return; }

  ILogger::Debug() << QString("Connection state changed : %1 (%2).")
                      .arg(stateToString(state))
                      .arg(state);

  m_enumState = state;

  emit MCServer::stateChanged(state);

  if (state == ClosedState) {
    emit closed();
  }
}

void MCServer::refuseClientConnection_(MCClientThread* client, const QString& reason) {
  emit clientConnectionRefused(client, reason);
  client->refuseConnection(reason);
}
