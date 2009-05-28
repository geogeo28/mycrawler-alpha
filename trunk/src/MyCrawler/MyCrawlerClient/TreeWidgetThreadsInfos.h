/****************************************************************************
 * @(#) Crawler Tree Widget.
 * Source code of the crawler of IC05
 *
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

#ifndef TREEWIDGETTHREADSINFOS_H
#define TREEWIDGETTHREADSINFOS_H

#include <QtGui>

class CNetworkManager;
class NetworkManagerThread;
class CTransferRate;

class TreeWidgetThreadsInfos : public QTreeWidget {
  Q_OBJECT

  public:
    TreeWidgetThreadsInfos(QWidget* parent = NULL);
    void setup(const CNetworkManager* networkManager);

  private slots:
    void slotRequestStarted(const NetworkManagerThread* thread);
    //void slotRequestResponseHeaderReceived(int statusCode, const NetworkManagerThread& thread);
    //void slotRequestRedirection(QUrl urlRedirection, const NetworkManagerThread& thread);
    void slotRequestDownloadStarted(const NetworkManagerThread* thread);
    void slotRequestDownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const NetworkManagerThread* thread);
    void slotRequestFinished(const NetworkManagerThread* thread);
    void slotRequestTransferRateUpdated(const CTransferRate* transferRateManager, const NetworkManagerThread* thread);

};

#endif
