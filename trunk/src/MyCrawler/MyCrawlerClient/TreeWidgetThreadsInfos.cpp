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

#include "TreeWidgetThreadsInfos.h"
#include "NetworkManager.h"
#include "TransferRate.h"
#include "Tools.h"

#define COLUMN_ID               0
#define COLUMN_URL              1
#define COLUMN_PROGRESSBAR      2
#define COLUMN_TRANSFER_RATE    3
#define COLUMN_STAT             4

TreeWidgetThreadsInfos::TreeWidgetThreadsInfos(QWidget* parent)
  : QTreeWidget(parent)
{}

void TreeWidgetThreadsInfos::setup(const CNetworkManager* networkManager) {
  clear();

  setColumnWidth(COLUMN_ID, 65);
  setColumnWidth(COLUMN_URL, 350);
  setColumnWidth(COLUMN_PROGRESSBAR, 220);
  setColumnWidth(COLUMN_TRANSFER_RATE, 90);
  //setColumnWidth(COLUMN_STAT, 200);

  for (int i = 0; i < networkManager->numberOfThreads(); ++i) {
    QTreeWidgetItem* item = new QTreeWidgetItem(this);

    item->setText(COLUMN_ID, QString("Thread #%1").arg(i + 1));
    item->setText(COLUMN_STAT, QString("En attente."));

    QProgressBar* progressbar = new QProgressBar();
    setItemWidget(item, COLUMN_PROGRESSBAR, progressbar);

    insertTopLevelItem(0, item);
  }

  // Connexion des signaux aux slots
  QObject::connect(
    networkManager, SIGNAL(started(const NetworkManagerThread*)),
    this, SLOT(slotRequestStarted(const NetworkManagerThread*))
  );
  QObject::connect(
    networkManager, SIGNAL(downloadStarted(const NetworkManagerThread*)),
    this, SLOT(slotRequestDownloadStarted(const NetworkManagerThread*))
  );
  QObject::connect(
    networkManager, SIGNAL(downloadProgress(qint64, qint64, const NetworkManagerThread*)),
    this, SLOT(slotRequestDownloadProgress(qint64, qint64, const NetworkManagerThread*))
  );
  QObject::connect(
    networkManager, SIGNAL(finished(const NetworkManagerThread*)),
    this, SLOT(slotRequestFinished(const NetworkManagerThread*))
  );
  QObject::connect(
    networkManager, SIGNAL(transferRateUpdated(const CTransferRate*, const NetworkManagerThread*)), this,
    SLOT(slotRequestTransferRateUpdated(const CTransferRate*, const NetworkManagerThread*))
  );
}


void TreeWidgetThreadsInfos::slotRequestStarted(const NetworkManagerThread* thread) {
  QTreeWidgetItem* item = topLevelItem(thread->id());
  QProgressBar* progressBar = qobject_cast<QProgressBar*>(itemWidget(item, COLUMN_PROGRESSBAR));

  item->setText(COLUMN_URL, thread->reply()->url().toString());
  item->setText(COLUMN_TRANSFER_RATE, QString());
  item->setText(COLUMN_STAT, "Connexion en cours...");

  progressBar->setMaximum(100);
  progressBar->setValue(0);
}

/*void TreeWidgetThreadsInfos::slotRequestResponseHeaderReceived(int statusCode, const NetworkManagerThread& thread) {

}*/

/*void TreeWidgetThreadsInfos::slotRequestRedirection(QUrl urlRedirection, const NetworkManagerThread& thread) {

}*/

void TreeWidgetThreadsInfos::slotRequestDownloadStarted(const NetworkManagerThread* thread) {
  QTreeWidgetItem* item = topLevelItem(thread->id());
  item->setText(COLUMN_STAT, "Lecture des données...");
}

void TreeWidgetThreadsInfos::slotRequestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const NetworkManagerThread* thread) {
  QTreeWidgetItem* item = topLevelItem(thread->id());
  QProgressBar* progressBar = qobject_cast<QProgressBar*>(itemWidget(item, COLUMN_PROGRESSBAR));

  progressBar->setMaximum(bytesTotal);
  progressBar->setValue(bytesReceived);
}

void TreeWidgetThreadsInfos::slotRequestFinished(const NetworkManagerThread* thread) {
  QTreeWidgetItem* item = topLevelItem(thread->id());
  QProgressBar* progressBar = qobject_cast<QProgressBar*>(itemWidget(item, COLUMN_PROGRESSBAR));

  item->setText(COLUMN_URL, QString());
  item->setText(COLUMN_TRANSFER_RATE, QString());
  item->setText(COLUMN_STAT, "En attente.");

  progressBar->setMaximum(100);
  progressBar->setValue(0);
}

void TreeWidgetThreadsInfos::slotRequestTransferRateUpdated(const CTransferRate* transferRateManager, const NetworkManagerThread* thread) {
  QTreeWidgetItem* item = topLevelItem(thread->id());

  item->setText(COLUMN_TRANSFER_RATE, Tools::sizeToString(transferRateManager->rate()) + "/s");
}
