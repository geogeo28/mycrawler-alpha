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

MCClientThread::MCClientThread(int socketDescriptor, QObject* parent)
    : QThread(parent),
      m_nSocketDescriptor(socketDescriptor)
{
  ILogger::Debug() << "Construct a ClientThread.";

  setError_(NoError, false);

  //QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
  //QObject::connect(this, SIGNAL(terminated()), this, SLOT(deleteLater()));

}

MCClientThread::~MCClientThread() {
  ILogger::Debug() << "Destroy a ClientThread.";

  // Close the client peer connection
  /*if (m_pClientPeer) {
    m_pClientPeer->close();
    delete m_pClientPeer;
  }*/
}

/*void MCClientThread::changeConnectionState(MCClientThread::ConnectionState state) {
  switch (state) {
    case ConnectionRefusedState:
      //m_pClientPeer->setSocketError(MCClientPeer::ConnectionRefusedError);
      //m_pClientPeer->abort();
      //quit();
      ILogger::Trace() << this->currentThread() << " Emit connection refused.";
      //emit peerConnectionRefused();
      m_pClientPeer->connectionRefused();
      break;
    default:;
  }
}*/

void MCClientThread::run() {
  MCClientPeer clientPeer;

  QObject::connect(
    &clientPeer, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
    this, SLOT(peerStateChanged_(QAbstractSocket::SocketState)),
    Qt::DirectConnection
  );

  QObject::connect(
    &clientPeer, SIGNAL(readyRead()),
    this, SLOT(peerReadyRead_()),
    Qt::DirectConnection
  );

  QObject::connect(
    this, SIGNAL(peerConnectionRefused()),
    &clientPeer, SLOT(connectionRefused()),
    Qt::QueuedConnection
  );

  // Could not attach the socket of the client peer from the socket descriptor
  if (!clientPeer.setSocketDescriptor(m_nSocketDescriptor, MCClientPeer::ConnectedState, MCClientPeer::ReadWrite)) {
    setError_(MCClientThread::ClientPeerError, true);
    clientPeer.close();
    return;
  }

  ILogger::Trace() << currentThread() << QString(" Client %1 : Listening...").arg(clientPeer.peerAddressWithPort());

  //emit connected();

  // Event loop
  exec();
}

void MCClientThread::peerStateChanged_(QAbstractSocket::SocketState socketState) {
  //const MCClientPeer& clientPeer = qobject_cast<const MCClientPeer&>(sender());
  ILogger::Trace() << "Socket state changed " << socketState;
}

void MCClientThread::peerReadyRead_() {
  ILogger::Trace() << "Ready read.";
  emit connected();
  //emit peerConnectionRefused();
}

void MCClientThread::emitPeerConnectionRefused() {
  emit peerConnectionRefused();
}

void MCClientThread::setError_(Error error, bool signal) {
  m_enumError = error;

  switch (error) {
    case NoError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "No error.");
      break;
    case ClientPeerError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "An error was occurred in the client peer.");
      break;
    default:
      m_enumError = MCClientThread::UnknownError;
      m_sError = QT_TRANSLATE_NOOP(MCClientThread, "Unknown error.");
      break;
  }

  if (signal == true) {
    emit MCClientThread::error(m_enumError);
  }
}
