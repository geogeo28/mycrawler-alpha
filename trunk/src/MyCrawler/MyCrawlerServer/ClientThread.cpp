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

#include <QUrl>

#include "Debug/Exception.h"
#include "Debug/Logger.h"
#include "Utilities/NetworkInfo.h"

#include "UrlInfo.h"
#include "ServerInfo.h"

#include "ClientThread.h"
#include "Server.h"
#include "ServerApplication.h"

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

  // Signals/slots connections
  QObject::connect(&m_urlsInProgress, SIGNAL(urlAdded(MCUrlInfo)), this, SIGNAL(urlInProgressAdded(MCUrlInfo)));
  QObject::connect(&m_urlsInProgress, SIGNAL(urlRemoved(MCUrlInfo)), this, SIGNAL(urlInProgressRemoved(MCUrlInfo)));
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

  clientPeer.setRequireAuthentication(true); // The client must be authenticated to process packets

  // Setup signals/slots connections
  qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
  qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
  qRegisterMetaType<MCClientPeer::TimeoutNotify>("MCClientPeer::TimeoutNotify");
  qRegisterMetaType<MCClientPeer::PacketType>("MCClientPeer::PacketType");
  qRegisterMetaType<MCClientPeer::PacketError>("MCClientPeer::PacketError");
  qRegisterMetaType<MCClientPeer::ErrorBehavior>("MCClientPeer::ErrorBehavior");
  qRegisterMetaType<CNetworkInfo>("CNetworkInfo");
  qRegisterMetaType<MCServerInfo>("MCServerInfo");

  // Receive 'message'
  QObject::connect(&clientPeer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(peerError_(QAbstractSocket::SocketError)));
  QObject::connect(&clientPeer, SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SIGNAL(timeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(&clientPeer, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,MCClientPeer::ErrorBehavior)), this, SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,MCClientPeer::ErrorBehavior)));
  QObject::connect(&clientPeer, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(peerStateChanged_(QAbstractSocket::SocketState)));
  QObject::connect(&clientPeer, SIGNAL(authenticated(const CNetworkInfo&)), this, SLOT(peerAuthenticated_(const CNetworkInfo&)));
  QObject::connect(&clientPeer, SIGNAL(serverInfoRequest()), this, SLOT(peerServerInfoRequest_()));
  QObject::connect(&clientPeer, SIGNAL(seedUrlRequest()), this, SLOT(peerSeedUrlRequest_()));

  // Send 'message'
  QObject::connect(this, SIGNAL(callPeerRefuseConnection_(const QString&)), &clientPeer, SLOT(refuseConnection(const QString&)));
  QObject::connect(this, SIGNAL(callPeerSendHandShake_()), &clientPeer, SLOT(sendHandShake()));
  QObject::connect(this, SIGNAL(callPeerSendRequestDenied_(MCClientPeer::PacketType)), &clientPeer, SLOT(sendRequestDenied(MCClientPeer::PacketType)));
  QObject::connect(this, SIGNAL(callPeerSendServerInfoResponse_(const MCServerInfo&)), &clientPeer, SLOT(sendServerInfoResponse(const MCServerInfo&)));
  QObject::connect(this, SIGNAL(callPeerSendSeedUrlResponse_(const QString&,quint32)), &clientPeer, SLOT(sendSeedUrlResponse(const QString&,quint32)));

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

  ILogger::Debug() << "Exit the event loop.";

  // Disconnection of the client peer volunteer (initiate by the client thread)
  if (clientPeer.state() != MCClientPeer::UnconnectedState) {
    clientPeer.disconnect();
  }

  m_pClientPeer = NULL;
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
  setConnectionState_(state);
}

void MCClientThread::peerAuthenticated_(const CNetworkInfo& networkInfo) {
  mutex.lock();
  m_bAuthenticated = true;
  m_networkInfo = networkInfo;
  mutex.unlock();

  setConnectionState_(ConnectedState);
  emit authenticated(networkInfo);
}

void MCClientThread::peerServerInfoRequest_() {
  emit callPeerSendServerInfoResponse_(MCServer::instance()->serverInfo());
}

void MCClientThread::peerSeedUrlRequest_() {
  mutex.lock();

  // Take an Url in queue
  MCUrlInfo urlInfo = MCApp->urlsInQueue()->takeOne();

  // Not Url in queue
  if (urlInfo.isValid() == false) {
    mutex.unlock();

    emit callPeerSendRequestDenied_(MCClientPeer::SeedUrlRequestPacket);
    return;
  }

  // Send Url
  m_urlsInProgress.addUrl(urlInfo);
  mutex.unlock();

  emit callPeerSendSeedUrlResponse_(urlInfo.url().toString(QUrl::None), urlInfo.depth());
}

void MCClientThread::setError_(Error error, bool signal) {
  mutex.lock();

  m_enumError = error;

  switch (error) {
    case NoError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "No error");
      break;
    case ClientPeerError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "An error occurred on the client peer");
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

void MCClientThread::setConnectionState_(ConnectionState state) {
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

  mutex.unlock();
  emit MCClientThread::connectionStateChanged(state);
  mutex.lock();

  // Connected and disconnected
  switch (state) {
    // Connected
    case ConnectedState:
    {
      emit connected();
      mutex.unlock();
      connected_();
      return;
    }
    break;

    // Unconnected
    case UnconnectedState:
    {
      disconnected_();
      mutex.unlock();
      emit disconnected();
      return;
    }
    break;

    default:;
  }

  mutex.unlock();
}

void MCClientThread::connected_() {

}

void MCClientThread::disconnected_() {
  // Restore Url in Progress
  MCApp->urlsInQueue()->merge(m_urlsInProgress);
  // Flush Urls in Progress is useless because when a client is disconnect, the thread is destroyed
}
