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

#include "ClientThread.h"

MCClientThreadInfo::MCClientThreadInfo(
  const QString& peerName,
  const QHostAddress& peerAddress,
  quint16 peerPort
)
  : m_sPeerName(peerName), m_peerAddress(peerAddress), m_u16PeerPort(peerPort)
{}

QString MCClientThreadInfo::peerAddressAndPort() const {
  return QString("%1:%2")
         .arg(peerAddress().toString())
         .arg(peerPort());
}

MCClientThread::MCClientThread(int socketDescriptor, QObject* parent)
    : QThread(parent),
      m_nSocketDescriptor(socketDescriptor),
      m_enumConnectionState(UnconnectedState)
{
  ILogger::Debug() << "Construct.";

  setError_(NoError, false);
}

MCClientThread::~MCClientThread() {
  ILogger::Debug() << "Destroyed.";
}

QString MCClientThread::connectionStateToString(ConnectionState state) {
  switch (state) {
    case UnconnectedState:  return QT_TRANSLATE_NOOP(MCClientThread, "Unconnected");
    case HostLookupState:   return QT_TRANSLATE_NOOP(MCClientThread, "Host Lookup");
    case ConnectingState:   return QT_TRANSLATE_NOOP(MCClientThread, "Connecting");
    case ConnectedState:    return QT_TRANSLATE_NOOP(MCClientThread, "Connected");
    case ClosingState:      return QT_TRANSLATE_NOOP(MCClientThread, "Closing");
    case ListeningState:    return QT_TRANSLATE_NOOP(MCClientThread, "Listening");

    default:
      return QT_TRANSLATE_NOOP(MCClientThread, "Invalid state");
  }
}

void MCClientThread::run() {
  m_mutex.lock();

  ILogger::Debug() << currentThread() << " : Running...";

  MCClientPeer clientPeer;
  m_pClientPeer = &clientPeer;

  // Setup signals/slots connections
  qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
  qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
  qRegisterMetaType<MCClientPeer::TimeoutNotify>("MCClientPeer::TimeoutNotify");

  QObject::connect(&clientPeer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(peerError_(QAbstractSocket::SocketError)));
  QObject::connect(&clientPeer, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(peerStateChanged_(QAbstractSocket::SocketState)));
  QObject::connect(&clientPeer, SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SIGNAL(timeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(&clientPeer, SIGNAL(packetKeepAliveSent()), this, SIGNAL(keepAliveNotify()));

  // Could not attach the socket of the client peer from the socket descriptor
  if (!clientPeer.setSocketDescriptor(m_nSocketDescriptor, MCClientPeer::ConnectedState, MCClientPeer::ReadWrite)) {
    setError_(MCClientThread::ClientPeerError, true);
    clientPeer.close();
    return;
  }

  // Set thread' information if doesn't valid
  m_threadInfo.setPeerName(m_pClientPeer->peerName());
  m_threadInfo.setPeerAddress(m_pClientPeer->peerAddress());
  m_threadInfo.setPeerPort(m_pClientPeer->peerPort());
  
  // Set connection state to listening
  //setConnectionState_(ListeningState, true);

  ILogger::Debug() << currentThread()
                   << QString(" - %2 : Execute the event loop.")
                      .arg(threadInfo().peerAddressAndPort());
  m_mutex.unlock();

  // Event loop
  exec();

  ILogger::Debug() << "Exit event loop.";

  // Disconnection of the client peer volunteer (initiate by the client thread)
  if (clientPeer.state() != MCClientPeer::UnconnectedState) {
    clientPeer.disconnect();
  }
  m_pClientPeer = NULL;
}

void MCClientThread::peerError_(QAbstractSocket::SocketError socketError) {
  setError_(ClientPeerError, true);
}

void MCClientThread::peerStateChanged_(QAbstractSocket::SocketState socketState) {
  QMutexLocker locker(&m_mutex);  

  ConnectionState state = InvalidState;

  // Translate socket state to MCClientThread::connectionState
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
      state = ConnectedState;
      break;
    case QAbstractSocket::ClosingState:
      state = ClosingState;
      break;
    default:;
  }

  // Emit signal state changed
  setConnectionState_(state, true);
}

void MCClientThread::setError_(Error error, bool signal) {
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
    emit MCClientThread::error(m_enumError);
  }
}

void MCClientThread::setConnectionState_(ConnectionState state, bool signal) {
  // Do nothing if the connection state didn't changed
  if (state == m_enumConnectionState) { return; }

  ILogger::Debug() << QString("%1 : Connection state changed : %2 (%3).")
                      .arg(threadInfo().peerAddressAndPort())
                      .arg(MCClientThread::connectionStateToString(state))
                      .arg(state);

  m_enumConnectionState = state;

  // Emit signal if set
  if (signal == true) {
    emit MCClientThread::connectionStateChanged(state);
  }

  // Connected and disconnected
  switch (state) {
    // Connected
    case ConnectedState:
    {
      ILogger::Debug() << QString("%1 : Connected.")
                          .arg(threadInfo().peerAddressAndPort());
      if (signal == true) { emit connected(); }

      break;
    }

    // Unconnected
    case UnconnectedState:
    {
      ILogger::Debug() << QString("%1 : Disconnected. Quit and delete the thread.")
                          .arg(threadInfo().peerAddressAndPort());

      if (signal == true) { emit disconnected(); }
      break;
    }

    default:;
  }
}
