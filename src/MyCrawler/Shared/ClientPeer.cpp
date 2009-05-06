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

#include "ClientPeer.h"

int MCClientPeer::s_nTimeout = 120 * 1000;
int MCClientPeer::s_nConnectTimeout = 60 * 1000;
int MCClientPeer::s_nKeepAliveInterval = 30 * 1000;

MCClientPeer::MCClientPeer(QObject* parent)
  : QTcpSocket(parent),
    m_idTimeoutTimer(0), m_idKeepAliveTimer(0), m_bInvalidateTimeout(false),
    m_bReceivedHandShake(false)
{
  ILogger::Debug() << "Construct.";

  m_idTimeoutTimer = startTimer(s_nConnectTimeout);

  QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processIncomingData_()));
  QObject::connect(this, SIGNAL(connected()), this, SLOT(connected_()));
}

MCClientPeer::~MCClientPeer() {
  ILogger::Debug() << "Destroyed.";
}

void MCClientPeer::setConnected() {
  ILogger::Debug() << QString("%1:%2 : Set state to connected.")
                      .arg(peerAddress().toString()).arg(peerPort());

  setSocketState(ConnectedState);
  waitForConnected();
}

void MCClientPeer::connectionRefused() {
  ILogger::Debug() << QString("%1:%2 : Connection refused.")
                      .arg(peerAddress().toString()).arg(peerPort());

  setSocketError(ConnectionRefusedError);
  disconnectFromHost();
}

void MCClientPeer::processIncomingData_() {
  ILogger::Trace() << QString("Client %1:%2 : Ready read.")
                      .arg(peerAddress().toString()).arg(peerPort());

  m_bInvalidateTimeout = true;
}

void MCClientPeer::connected_() {
  ILogger::Debug() << QString("%1:%2 : Signal connected emitted.")
                      .arg(peerAddress().toString()).arg(peerPort());

  // Initialize keep-alive timer
  if (!m_idKeepAliveTimer) {
    m_idKeepAliveTimer = startTimer(s_nKeepAliveInterval);
  }
}

void MCClientPeer::timerEvent(QTimerEvent *event) {
  if (event->timerId() == m_idTimeoutTimer) {
    // Disconnect if we timed out; otherwise the timeout is
    // restarted.
    if (m_bInvalidateTimeout) {
      m_bInvalidateTimeout = false;
    }
    else {
      ILogger::Debug() << QString("%1:%2 : Timeout.")
                          .arg(peerAddress().toString()).arg(peerPort());

      emit timeout((m_bReceivedHandShake == false)?ConnectTimeoutNotify:PeerTimeoutNotify);
      abort();
    }
  }
  else if (event->timerId() == m_idKeepAliveTimer) {
    //sendKeepAlive();
    emit packetKeepAliveSended();
  }

  QTcpSocket::timerEvent(event);
}
