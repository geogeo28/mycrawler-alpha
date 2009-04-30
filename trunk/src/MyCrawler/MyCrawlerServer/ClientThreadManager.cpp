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

#include "ClientThreadManager.h"

#include "ClientThread.h"

static const int MaxThreads = 10;

Q_GLOBAL_STATIC(MCClientThreadManager, MCClientThreadManagerInstance)

MCClientThreadManager* MCClientThreadManager::instance() {
  return MCClientThreadManagerInstance();
}

MCClientThreadManager::MCClientThreadManager()
{
  setError_(MCClientThreadManager::NoError);
}

MCClientThreadManager::~MCClientThreadManager()
{}

MCClientThreadManager::Error MCClientThreadManager::error() const {
  return m_enumError;
}

QString MCClientThreadManager::errorString() const {
  return m_sError;
}

bool MCClientThreadManager::canCreateThread() const {
  return (m_lstClientThreads.size() < maxThreads());
}

int MCClientThreadManager::maxThreads() const {
  return MaxThreads;
}

MCClientThread* MCClientThreadManager::createThread(int socketDescriptor) {
  if (!canCreateThread()) {
    setError_(MCClientThreadManager::ManagerFull, true);
    return NULL;
  }

  MCClientThread* clientThread = new MCClientThread(socketDescriptor/*, this*/);

  QObject::connect(clientThread, SIGNAL(error(MCClientThread::Error)), this, SLOT(errorThread_(MCClientThread::Error)));
  QObject::connect(clientThread, SIGNAL(finished()), this, SLOT(finishedThread_()));

  m_lstClientThreads << clientThread;

  return clientThread;
}

void MCClientThreadManager::errorThread_(MCClientThread::Error error) {
  MCClientThread* clientThread = qobject_cast<MCClientThread*>(this->sender());
  emit errorThread(clientThread, error);
}

void MCClientThreadManager::finishedThread_() {
  MCClientThread* clientThread = qobject_cast<MCClientThread*>(this->sender());
  emit finishedThread(clientThread);
}

void MCClientThreadManager::setError_(Error error, bool signal) {
  m_enumError = error;

  switch (error) {
    case NoError:
      m_sError = QT_TRANSLATE_NOOP(MCClientThreadManager, "No error");
      break;
    case ManagerFull:
      m_sError = QT_TRANSLATE_NOOP(MCClientThreadManager, "The manager of client threads is full. You must increase the variable MaxThreads to accept new threads.");
      break;
    default:
      m_enumError = UnknownError;
      m_sError = QT_TRANSLATE_NOOP(MCClientThreadManager, "Unknown error");
      break;
  }

  if (signal == true) {
    emit MCClientThreadManager::error(m_enumError);
  }
}
