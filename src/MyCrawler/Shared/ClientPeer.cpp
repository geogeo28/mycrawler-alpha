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

#include <QDataStream>

#include "Debug/Logger.h"

#include "ClientPeer.h"

int MCClientPeer::s_nTimeout = 120 * 1000;
int MCClientPeer::s_nConnectTimeout = 60 * 1000;
int MCClientPeer::s_nHandShakeTimeout = 60 * 1000;
int MCClientPeer::s_nKeepAliveInterval = 30 * 1000;

static const quint32 MinimalPacketSize = 6;

MCClientPeer::MCClientPeer(QObject* parent)
  : QTcpSocket(parent),
    m_idTimeoutTimer(0), m_idKeepAliveTimer(0), m_bInvalidateTimeout(false),
    m_bReceivedHandShake(false), m_bSentHandShake(false),
    m_u32PacketSize(0), m_u16PacketType(0)
{
  ILogger::Debug() << "Construct.";

  QObject::connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectionStateChanged_(QAbstractSocket::SocketState)));
  QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processIncomingData_()));
}

MCClientPeer::~MCClientPeer() {
  ILogger::Debug() << "Destroyed.";
}

void MCClientPeer::sendPacket(PacketType type, const QByteArray& data) {
  ILogger::Debug() << QString("Sending the packet : %1 (%2)...")
                      .arg(packetTypeToString(type))
                      .arg(type);

  // Prepate the packet
  QByteArray packet;
  QDataStream out(&packet, QIODevice::WriteOnly);

  quint32 packetSize = ((quint32)data.size()) + MinimalPacketSize;
  out << (quint32)packetSize; // Size of the packet
  out << (quint16)type; // Type of the packet
  if (!data.isEmpty()) {
    out << data; // Data transported
  }

  // Send the packet
  write(packet);

  emit packetSent(type, packetSize);
}

QString MCClientPeer::stateToString(QAbstractSocket::SocketState state) {
  switch (state) {
    case QAbstractSocket::HostLookupState:   return QT_TRANSLATE_NOOP(MCClientPeer, "Host Lookup");
    case QAbstractSocket::ConnectingState:   return QT_TRANSLATE_NOOP(MCClientPeer, "Connecting");
    case QAbstractSocket::ConnectedState:    return QT_TRANSLATE_NOOP(MCClientPeer, "Connected");
    case QAbstractSocket::BoundState:        return QT_TRANSLATE_NOOP(MCClientPeer, "Bound");
    case QAbstractSocket::ClosingState:      return QT_TRANSLATE_NOOP(MCClientPeer, "Closing");
    case QAbstractSocket::ListeningState:    return QT_TRANSLATE_NOOP(MCClientPeer, "Listening");

    default:
      return QT_TRANSLATE_NOOP(MCClientThread, "Unconnected");
  }
}

QString MCClientPeer::timeoutNotifyToString(TimeoutNotify notify) {
  switch (notify) {
    case ConnectTimeoutNotify:   return QT_TRANSLATE_NOOP(MCClientPeer, "Connection timed out");
    case HandShakeTimeoutNotify: return QT_TRANSLATE_NOOP(MCClientPeer, "Handshake timed out");
    case PeerTimeoutNotify:      return QT_TRANSLATE_NOOP(MCClientPeer, "Peer timed out");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown timeout");
  }
}

QString MCClientPeer::packetTypeToString(PacketType type) {
  switch (type) {
    case HandShakePacket: return QT_TRANSLATE_NOOP(MCClientPeer, "Handshake");
    case KeepAlivePacket: return QT_TRANSLATE_NOOP(MCClientPeer, "KeepAlive");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown packet type");
  }
}

QString MCClientPeer::packetErrorToString(PacketError error) {
  switch (error) {
    case PacketSizeError:                  return QT_TRANSLATE_NOOP(MCClientPeer, "Packet size error");
    case PacketTypeError:                  return QT_TRANSLATE_NOOP(MCClientPeer, "Packet type unknown");
    case HandShakePacketNotReceivedError : return QT_TRANSLATE_NOOP(MCClientPeer, "Could not process the packet because no handshake message was received");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown packet error");
  }
}

void MCClientPeer::connectionRefused() {
  ILogger::Debug() << QString("%1:%2 : Connection refused.")
                      .arg(peerAddress().toString()).arg(peerPort());

  setSocketError(ConnectionRefusedError);
  disconnectFromHost();
}

void MCClientPeer::disconnect(int msecs) {
  ILogger::Debug() << "Try to disconnect the client peer.";

  // Close the connection
  if (state() == MCClientPeer::ConnectingState) {
    ILogger::Debug() << "Close the connection.";
    abort();
  }
  // Force to close the connection properly
  else if (state() != MCClientPeer::UnconnectedState) {
    ILogger::Debug() << "Disconnect the client peer.";
    disconnectFromHost();

    if ((msecs > 0) && (state() != MCClientPeer::UnconnectedState)) {
      ILogger::Debug() << "Waiting to disconnect...";
      waitForDisconnected(msecs);
    }
  }
}

void MCClientPeer::sendHandShake() {
  m_bSentHandShake = true;

  // Restart the timeout
  if (m_idTimeoutTimer) {
    killTimer(m_idTimeoutTimer);
  }
  m_idTimeoutTimer = startTimer(s_nTimeout);

  sendPacket(HandShakePacket);
}

void MCClientPeer::connectionStateChanged_(QAbstractSocket::SocketState state) {
  ILogger::Debug() << QString("Connection state changed : %1 (%2).")
                      .arg(stateToString(state))
                      .arg(state);

  switch (state) {
    // Connecting...
    case ConnectingState:
      connecting_();
      break;

    // Connected
    case ConnectedState:
      connected_();
      break;

    // Closing
    case ClosingState:
      closing_();
      break;

    // Disconnected
    case UnconnectedState:
      disconnected_();
      break;

    default:;
  }
}

void MCClientPeer::processIncomingData_() {
  ILogger::Debug() << "Ready read";

  m_bInvalidateTimeout = true;

  do {
    // New packet
    if (m_u32PacketSize == 0) {
      // Check that we received enough data
      if (bytesAvailable() < MinimalPacketSize) {
        return;
      }

      // Get packet size and packet type
      QDataStream dataStream(this);
      dataStream >> m_u32PacketSize;
      dataStream >> m_u16PacketType;

      // Prevent DoS attack
      if ((m_u32PacketSize < MinimalPacketSize) || (m_u32PacketSize > 200000)) {
        errorProcessingPacket_(PacketSizeError, true);
        return;
      }
    }

    // Attempts to have enough bytes to process the packet
    if (bytesAvailable() < (m_u32PacketSize - MinimalPacketSize)) {
      return;
    }

    processPacket_();

    m_u32PacketSize = 0;
  } while (bytesAvailable() >= MinimalPacketSize);
}

void MCClientPeer::timerEvent(QTimerEvent *event) {
  // Timeout (Connect, Handshake, Peer)
  if (event->timerId() == m_idTimeoutTimer) {
    // Disconnect if we timed out; otherwise the timeout is restarted.
    if (m_bInvalidateTimeout == true) {
      m_bInvalidateTimeout = false;
    }
    else {
      TimeoutNotify notify = UnknownTimeoutNotify;
      if (state() == ConnectingState) {
        notify = ConnectTimeoutNotify;
      }
      else if (state() == ConnectedState) {
        if (m_bReceivedHandShake == false) { notify = HandShakeTimeoutNotify; }
        else { notify = PeerTimeoutNotify; }
      }

      ILogger::Debug() << QString("%1 (%2).")
                          .arg(timeoutNotifyToString(notify))
                          .arg(notify);
      emit timeout(notify);
      abort();
    }
  }
  // KeepAlive
  else if (event->timerId() == m_idKeepAliveTimer) {
    sendPacket(KeepAlivePacket);
  }

  QTcpSocket::timerEvent(event);
}

void MCClientPeer::errorProcessingPacket_(MCClientPeer::PacketError error, bool aborted) {
  PacketType packetType = static_cast<PacketType>(m_u16PacketType);

  emit errorProcessingPacket(error, packetType, m_u32PacketSize, aborted);

  // Abort the connection
  if (aborted == true) {
    abort();
  }
}

// Initialize all state variable
void MCClientPeer::connecting_() {
  m_bInvalidateTimeout = false;
  m_bReceivedHandShake = false;
  m_idTimeoutTimer = startTimer(s_nConnectTimeout);
}

void MCClientPeer::connected_() {
  m_idTimeoutTimer = startTimer(s_nHandShakeTimeout);
}

void MCClientPeer::closing_() {
  if (m_idTimeoutTimer) { killTimer(m_idTimeoutTimer); }
}

// Clean all state variable
void MCClientPeer::disconnected_() {
  m_bInvalidateTimeout = false;
  m_bReceivedHandShake = false;

  if (m_idTimeoutTimer) { killTimer(m_idTimeoutTimer); }
  if (m_idKeepAliveTimer) { killTimer(m_idKeepAliveTimer); }
}

void MCClientPeer::processPacket_() {
  PacketType packetType = static_cast<PacketType>(m_u16PacketType);

  ILogger::Debug() << QString("Processing the packet : %1 (%2)...")
                      .arg(packetTypeToString(packetType))
                      .arg(packetType);

  // Could not process the packet because no handshake message was received.
  if ((m_bReceivedHandShake == false) && (packetType != HandShakePacket)) {
    errorProcessingPacket_(HandShakePacketNotReceivedError, true);
    return;
  }

  switch (packetType) {
    case HandShakePacket:
      m_bReceivedHandShake = true;

      // Initialize keep-alive timer
      if (!m_idKeepAliveTimer) {
        m_idKeepAliveTimer = startTimer(s_nKeepAliveInterval);
      }

      if (m_bSentHandShake == false) {
        sendHandShake();
      }
      break;

    case KeepAlivePacket:
      sendHandShake();
      break;

    default:
      errorProcessingPacket_(PacketTypeError, true);
  }
}
