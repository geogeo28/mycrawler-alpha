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

#include "Client.h"
#include "ServersList.h"
#include "ServerInfo.h"

MCClient* MCClient::s_instance = NULL;

MCClient* MCClient::instance() {
  if (s_instance == NULL) {
    s_instance = new MCClient();
  }

  return s_instance;
}

void MCClient::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCClient::init_() {
  m_enumConnectionState = UnconnectedState;
  m_bHandShakeReceived = false;
  m_bConnectionRefused = false;
}

MCClient::MCClient(QObject* parent)
  : QObject(parent)
{
  ILogger::Debug() << "Construct.";

  // Check server authentication is not necessary to process packets
  m_clientPeer.setRequireAuthentication(false);

  init_();

  // Send HandShake and authentication
  QObject::connect(&m_clientPeer, SIGNAL(connected()), this, SLOT(peerConnected_()));

  // Other connections
  QObject::connect(&m_clientPeer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(peerError_(QAbstractSocket::SocketError)));
  QObject::connect(&m_clientPeer, SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SIGNAL(timeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(&m_clientPeer, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,MCClientPeer::ErrorBehavior)), this, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,MCClientPeer::ErrorBehavior)));
  QObject::connect(&m_clientPeer, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(peerStateChanged_(QAbstractSocket::SocketState)));
  QObject::connect(&m_clientPeer, SIGNAL(handShakeReceived()), this, SLOT(peerHandShakeReceived_()));
  QObject::connect(&m_clientPeer, SIGNAL(serverInfoResponse(const MCServerInfo&)), this, SLOT(peerServerInfoResponse_(const MCServerInfo&)));
}

MCClient::~MCClient() {
  if (m_clientPeer.state() != MCClientPeer::UnconnectedState) {
    m_clientPeer.disconnect();
  }

  ILogger::Debug() << "Destroyed.";
}

void MCClient::connectToHost(const MCServerInfo& serverInfo) {
  if (m_clientPeer.state() != QAbstractSocket::UnconnectedState) {
    m_clientPeer.disconnect();
  }

  Assert(serverInfo.isValid());
  ILogger::Debug() << QString("Connecting to the host %1...")
                      .arg(serverInfo.ipAndPortString());

  init_();
  m_serverInfo = serverInfo;
  m_clientPeer.connectToHost(serverInfo.ip(), serverInfo.port());
}

QString MCClient::connectionStateToString(ConnectionState state) {
  switch (state) {
    case UnconnectedState:     return QT_TRANSLATE_NOOP(MCClientThread, "Unconnected");
    case HostLookupState:      return QT_TRANSLATE_NOOP(MCClientThread, "Host Lookup");
    case ConnectingState:      return QT_TRANSLATE_NOOP(MCClientThread, "Connecting");
    case ConnectedState:       return QT_TRANSLATE_NOOP(MCClientThread, "Connected");
    case ClosingState:         return QT_TRANSLATE_NOOP(MCClientThread, "Closing");

    default:
      return QT_TRANSLATE_NOOP(MCClientThread, "Unknown state");
  }
}

/*QString MCClient::stateToString(State state) {

}*/

void MCClient::disconnect(int msecs) {
  m_clientPeer.disconnect(msecs);
}

void MCClient::peerError_(QAbstractSocket::SocketError socketError) {
  if (socketError == QAbstractSocket::ConnectionRefusedError) {
    m_bConnectionRefused = true;
  }
  else if ((socketError == QAbstractSocket::RemoteHostClosedError) && (m_bConnectionRefused == true)) {
    return;
  }

  emit error(socketError);
}

void MCClient::peerStateChanged_(QAbstractSocket::SocketState socketState) {
  // Translate socket state to MCClient::connectionState
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
      state = ConnectingState;
      break;
    case QAbstractSocket::ClosingState:
      state = ClosingState;
      break;
    default:;
  }

  // Emit signal state changed
  setConnectionState_(state, true);
}

void MCClient::peerConnected_() {
  m_clientPeer.sendHandShake(); // Send HandShake
  m_clientPeer.sendAuthentication(); // Send Authentication
}

void MCClient::peerHandShakeReceived_() {
  m_bHandShakeReceived = true;

  // Server Info request
  m_clientPeer.sendServerInfoRequest();

  setConnectionState_(ConnectedState, true);
}

void MCClient::peerServerInfoResponse_(const MCServerInfo& serverInfo) {
  // Update internal server info
  m_serverInfo.update(serverInfo);

  // Set servers list information
  quint32 ip = m_clientPeer.peerAddress().toIPv4Address();
  MCServersList::instance()->updateServer(ip, serverInfo);
}

void MCClient::setConnectionState_(ConnectionState state, bool signal) {
  // Do nothing if the connection state didn't changed
  if (state == m_enumConnectionState) {
    return;
  }

  ILogger::Debug() << QString("Connection state changed : %2 (%3).")
                      .arg(MCClient::connectionStateToString(state))
                      .arg(state);

  m_enumConnectionState = state;

  // Emit signal if set
  if (signal == true) {
    emit MCClient::connectionStateChanged(state);
  }

  // Connected and disconnected
  switch (state) {
    // Connected
    case ConnectedState:
    {
      if (signal == true) {
        emit connected();
        return;
      }
    }
    break;

    // Unconnected
    case UnconnectedState:
    {
      if (signal == true) {
        emit disconnected();
        return;
      }
    }
    break;

    default:;
  }
}
