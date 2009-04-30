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

#include "Debug/Logger.h"

#include "ClientThread.h"
#include "ClientPeer.h"

MCClientThread::MCClientThread(int socketDescriptor, QObject* parent)
    : QThread(parent),
      m_nSocketDescriptor(socketDescriptor)
{
  ILogger::Debug() << QString("Construct a ClientThread - Uid = %1.").arg(uid().toString());

  setError_(NoError, false);

  QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

MCClientThread::~MCClientThread() {
  ILogger::Debug() << QString("Destroy a ClientThread - Uid = %1.").arg(uid().toString());

  if (m_pClientPeer) { delete m_pClientPeer; }
}

void MCClientThread::run() {
  /*// Could not attach the socket of the client peer from the socket descriptor
  if (!m_pClientPeer->setSocketDescriptor(m_nSocketDescriptor, MCClientPeer::ConnectingState, MCClientPeer::ReadWrite)) {
    setError_(MCClientThread::ClientPeerError, true);
    m_pClientPeer->abort();
    return;
  }*/

  /*ILogger::Trace() << QString("Client thread %1:%2 : Running...")
                      .arg(m_pClientPeer->peerAddress().toString())
                      .arg(m_pClientPeer->peerPort());*/

  /*MCClientPeer p;
  if (!p.setSocketDescriptor(m_nSocketDescriptor, MCClientPeer::ConnectedState, MCClientPeer::ReadWrite)) {
    setError_(MCClientThread::ClientPeerError, true);
    p.abort();
    return;
  }*/
    
  // Event loop
  exec();
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

