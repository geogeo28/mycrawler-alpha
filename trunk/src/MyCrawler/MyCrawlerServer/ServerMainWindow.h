/****************************************************************************
 * @(#) MyCrawler server. Main window.
 * GUI interface
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

#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QtGui>
#include <QPointer>

#include "ui_ServerMainWindow.h"
#include "Server.h"
#include "ClientThread.h"
#include "ClientPeer.h"

class QAction;

class MCServerMainWindow : public QMainWindow,
                           private Ui_MCServerMainWindow
{
    Q_OBJECT

private:
    void setupWindow_();
    void setupMainToolBar_();
    void setupMenu_();
    void setupServerLogForm_();
    void setupClientsForm_();
    void setupComponents_();

    void cleanAll_();
    void closeWindow_();

public:
    MCServerMainWindow(QWidget *parent = NULL);
    ~MCServerMainWindow();

    void setup();

private slots:
    void on_doFilePreferences_triggered();
    void on_mainToolBar_actionTriggered(QAction* action);
    void on_doMainToolBarConnectDisconnect_triggered();

private slots:
    void slotProgressClientFinished(MCClientThread* client);

    void slotServerError(MCServer::Error error);
    void slotServerStateChanged(MCServer::State state);

    void slotClientError(MCClientThread* client, MCClientThread::Error error);
    void slotClientTimeout(MCClientThread* client, MCClientPeer::TimeoutNotify notifiedWhen);
    void slotClientErrorProcessingPacket(MCClientThread* client, MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void slotClientConnectionRefused(MCClientThread* client, const QString& reason);
    void slotClientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state);

protected:
    void closeEvent(QCloseEvent* event);

private:
    //void serverLogMessage_(const QStringconst QString& message,
    bool connectServer_();
    void disconnectServer_();

private:
    QAction* m_pActionCurrentForm;
    QPointer<QProgressDialog> m_pProgressDialogCloseClients;
};

#endif // SERVERMAINWINDOW_H
