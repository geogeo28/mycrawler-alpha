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

#include <QIODevice>
#include <QTimer>
#include <QTime>

#include "TransferRate.h"

/***************************************************************************
 * CONSTRUCTEUR, DESTRUCTEUR
 ***************************************************************************/
CTransferRate::CTransferRate(const QIODevice* device, int timeUpdate, QObject* parent)
  : QObject(parent),
    m_bFirstStart(true),
    m_pDevice(device), m_pTimer(new QTimer()),
    m_nBeginSize(0), m_nBytesTransfered(0), m_nTransferRate(0)
{
  m_pTimer->setInterval(timeUpdate);
  if (timeUpdate) {
    QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(calculateRate()));
  }
}

CTransferRate::~CTransferRate() {
  delete m_pTimer;
}


/***************************************************************************
 * METHODES PUBLIQUES
 ***************************************************************************/
void CTransferRate::start() {
  if (m_bFirstStart) {
    m_elapsedTime.start();
    m_bFirstStart = false;
  }
  else
    m_elapsedTime.restart();

  m_nBeginSize = m_nBytesTransfered = 0; /*m_pDevice->size();*/

  if (m_pTimer->interval())
    m_pTimer->start();
}

void CTransferRate::stop() {
  m_pTimer->stop();
}

int CTransferRate::elapsed() const {
  return m_elapsedTime.elapsed();
}

qint64 CTransferRate::averageRate() const {
  int nElapsed = elapsed();
  return (((m_pDevice->size() - m_nBeginSize) * 1000) / (nElapsed?nElapsed:1));
}


/***************************************************************************
 * SLOTS PUBLICS
 ***************************************************************************/
void CTransferRate::calculateRate() {
  qint64 diff = m_pDevice->size() - m_nBytesTransfered;
  int interval = m_pTimer->interval();
  m_nTransferRate = (diff * 1000) / (interval?interval:1);
  m_nBytesTransfered = m_pDevice->size();

  emit updated();
}
