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

MCClientPeer::MCClientPeer(QObject* parent)
  : QTcpSocket(parent)
{
  ILogger::Debug() << "Construct.";

  QObject::connect(this, SIGNAL(readyRead()), this, SLOT(readyRead_()));
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

void MCClientPeer::readyRead_() {
  ILogger::Trace() << QString("Client %1:%2 : Ready read.")
                      .arg(peerAddress().toString()).arg(peerPort());
}

void MCClientPeer::connected_() {
  ILogger::Debug() << QString("%1:%2 : Signal connected emitted.")
                      .arg(peerAddress().toString()).arg(peerPort());
}
