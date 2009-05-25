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

#include "Client.h"

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

MCClient::MCClient(QObject* parent)
  : QObject(parent),
    m_bConnectionRefused(false)
{
  ILogger::Debug() << "Construct.";

  // Send HandShake
  QObject::connect(&m_clientPeer, SIGNAL(connected()), &m_clientPeer, SLOT(sendHandShake()));

  // Other connections
  QObject::connect(&m_clientPeer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(peerError_(QAbstractSocket::SocketError)));
  QObject::connect(&m_clientPeer, SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SIGNAL(timeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(&m_clientPeer, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
  QObject::connect(&m_clientPeer, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SIGNAL(connectionStateChanged(QAbstractSocket::SocketState)));
  QObject::connect(&m_clientPeer, SIGNAL(connected()), this, SIGNAL(connected()));
  QObject::connect(&m_clientPeer, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
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

  m_serverInfo = serverInfo;
  m_clientPeer.connectToHost(serverInfo.ip(), serverInfo.port());
}

void MCClient::disconnect(int msecs) {
  ILogger::Debug() << "Slot disconnect.";

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
