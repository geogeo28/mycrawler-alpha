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
#include "Utilities/NetworkInfo.h"

#include "ClientThread.h"
#include "Server.h"

MCClientThread::MCClientThread(int socketDescriptor, QObject* parent)
    : QThread(parent),
      mutex(QMutex::Recursive),
      m_nSocketDescriptor(socketDescriptor),
      m_u16PeerPort(0),
      m_enumConnectionState(UnconnectedState),
      m_bAuthenticated(false),
      m_bConnectionRefused(false)
{
  ILogger::Debug() << "Construct.";

  setError_(NoError, false);
}

MCClientThread::~MCClientThread() {
  ILogger::Debug() << "Destroyed.";
}

quint64 MCClientThread::id() const {
  return (quint64)((unsigned int)this);
}

QString MCClientThread::connectionStateToString(ConnectionState state) {
  switch (state) {
    case UnconnectedState:     return QT_TRANSLATE_NOOP(MCClientThread, "Unconnected");
    case HostLookupState:      return QT_TRANSLATE_NOOP(MCClientThread, "Host Lookup");
    case ConnectingState:      return QT_TRANSLATE_NOOP(MCClientThread, "Connecting");
    case AuthenticatingState : return QT_TRANSLATE_NOOP(MCClientThread, "Authenticating");
    case ConnectedState:       return QT_TRANSLATE_NOOP(MCClientThread, "Connected");
    case ClosingState:         return QT_TRANSLATE_NOOP(MCClientThread, "Closing");

    default:
      return QT_TRANSLATE_NOOP(MCClientThread, "Unknown state");
  }
}

void MCClientThread::run() {
  mutex.lock();
  ILogger::Debug() << "Running...";

  MCClientPeer clientPeer;
  m_pClientPeer = &clientPeer;

  // Setup signals/slots connections
  qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
  qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
  qRegisterMetaType<MCClientPeer::TimeoutNotify>("MCClientPeer::TimeoutNotify");
  qRegisterMetaType<MCClientPeer::PacketType>("MCClientPeer::PacketType");
  qRegisterMetaType<MCClientPeer::PacketError>("MCClientPeer::PacketError");
  qRegisterMetaType<CNetworkInfo>("CNetworkInfo");
  qRegisterMetaType<MCServerInfo>("MCServerInfo");

  // Receive 'message'
  QObject::connect(&clientPeer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(peerError_(QAbstractSocket::SocketError)));
  QObject::connect(&clientPeer, SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SIGNAL(timeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(&clientPeer, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
  QObject::connect(&clientPeer, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(peerStateChanged_(QAbstractSocket::SocketState)));
  QObject::connect(&clientPeer, SIGNAL(authenticated(const CNetworkInfo&)), this, SLOT(peerAuthenticated_(const CNetworkInfo&)));
  QObject::connect(&clientPeer, SIGNAL(serverInfoRequest()), this, SLOT(peerServerInfoRequest_()));

  // Send 'message'
  QObject::connect(this, SIGNAL(callPeerRefuseConnection_(const QString&)), &clientPeer, SLOT(refuseConnection(const QString&)));
  QObject::connect(this, SIGNAL(callPeerSendHandShake_()), &clientPeer, SLOT(sendHandShake()));
  QObject::connect(this, SIGNAL(callPeerServerInfoResponse_(const MCServerInfo&)), &clientPeer, SLOT(sendServerInfoResponse(const MCServerInfo&)));

  // Could not attach the socket of the client peer from the socket descriptor
  if (!clientPeer.setSocketDescriptor(m_nSocketDescriptor, MCClientPeer::ConnectedState, MCClientPeer::ReadWrite)) {
    setError_(MCClientThread::ClientPeerError, true);
    clientPeer.close();
    mutex.unlock();
    return;
  }

  // Set client's information
  m_sPeerName = m_pClientPeer->peerName();
  m_peerAddress = m_pClientPeer->peerAddress();
  m_u16PeerPort = m_pClientPeer->peerPort();

  ILogger::Debug() << currentThread()
                   << QString(" - %2 : Execute the event loop.")
                      .arg(peerAddressAndPort());
  mutex.unlock();

  // Event loop
  exec();

  mutex.lock();
  ILogger::Debug() << "Exit the event loop.";

  // Disconnection of the client peer volunteer (initiate by the client thread)
  if (clientPeer.state() != MCClientPeer::UnconnectedState) {
    clientPeer.disconnect();
  }

  m_pClientPeer = NULL;
  mutex.unlock();
}

void MCClientThread::sendHandShake() {
  emit callPeerSendHandShake_();
}

void MCClientThread::refuseConnection(const QString& reason) {
  mutex.lock();
  m_bConnectionRefused = true;
  mutex.unlock();

  emit callPeerRefuseConnection_(reason);
}

void MCClientThread::peerError_(QAbstractSocket::SocketError socketError) {
  QMutexLocker locker(&mutex);
  setError_(ClientPeerError, true);
}

void MCClientThread::peerStateChanged_(QAbstractSocket::SocketState socketState) {
  mutex.lock();

  // Translate socket state to MCClientThread::connectionState
  ConnectionState state = InvalidState;

  switch (socketState) {
    case QAbstractSocket::UnconnectedState:
      state = UnconnectedState;
      break;
    case QAbstractSocket::HostLookupState:
      state = HostLookupState;
      break;
    case QAbstractSocket::ConnectingState:
      state = ConnectingState;
      break;
    case QAbstractSocket::ConnectedState:
      state = AuthenticatingState;
      break;
    case QAbstractSocket::ClosingState:
      state = ClosingState;
      break;
    default:;
  }

  mutex.unlock();

  // Emit signal state changed
  setConnectionState_(state, true);
}

void MCClientThread::peerAuthenticated_(const CNetworkInfo& networkInfo) {
  mutex.lock();
  m_bAuthenticated = true;
  m_networkInfo = networkInfo;
  mutex.unlock();

  setConnectionState_(ConnectedState, true);
  emit authenticated(networkInfo);
}

void MCClientThread::peerServerInfoRequest_() {
  emit callPeerServerInfoResponse_(MCServer::instance()->serverInfo());
}

void MCClientThread::setError_(Error error, bool signal) {
  mutex.lock();

  m_enumError = error;

  switch (error) {
    case NoError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "No error");
      break;
    case ClientPeerError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "An error was occurred on the client peer");
      break;
    default:
      m_enumError = MCClientThread::UnknownError;
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "Unknown error");
      break;
  }

  if (signal == true) {
    mutex.unlock();
    emit MCClientThread::error(m_enumError);
    return;
  }

  mutex.unlock();
}

void MCClientThread::setConnectionState_(ConnectionState state, bool signal) {  
  mutex.lock();

  // Do nothing if the connection state didn't changed
  if (state == m_enumConnectionState) {
    mutex.unlock();
    return;
  }

  ILogger::Debug() << QString("%1 : Connection state changed : %2 (%3).")
                      .arg(peerAddressAndPort())
                      .arg(MCClientThread::connectionStateToString(state))
                      .arg(state);

  m_enumConnectionState = state;

  // Emit signal if set
  if (signal == true) {
    mutex.unlock();
    emit MCClientThread::connectionStateChanged(state);
    mutex.lock();
  }

  // Connected and disconnected
  switch (state) {
    // Connected
    case ConnectedState:
    {
      if (signal == true) {
        mutex.unlock();
        emit connected();
        return;
      }
    }
    break;

    // Unconnected
    case UnconnectedState:
    {
      if (signal == true) {
        mutex.unlock();
        emit disconnected();
        return;
      }
    }
    break;

    default:;
  }

  mutex.unlock();
}
