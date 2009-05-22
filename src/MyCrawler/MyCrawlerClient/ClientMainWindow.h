/****************************************************************************
 * @(#) MyCrawler client. Main window.
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

#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QtGui>

#include "ui_ClientMainWindow.h"
#include "ClientPeer.h"

class MCClientMainWindow : public QMainWindow,
                           private Ui_MCClientMainWindow
{
    Q_OBJECT

private:
    void setupWindow_();
    void setupMainToolBar_();
    void setupMenu_();
    void setupForms_();
    void setupComponents_();

    void cleanAll_();
    void closeWindow_();

public:
    MCClientMainWindow(QWidget *parent = NULL);
    ~MCClientMainWindow();

    void setup();

private slots:
    void on_doFilePreferences_triggered();
    void on_doMainToolBarConnectDisconnect_triggered();

private slots:
    void slotClientError(QAbstractSocket::SocketError error);
    void slotClientTimeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void slotClientErrorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void slotClientConnectionStateChanged(QAbstractSocket::SocketState state); 

protected:
    void closeEvent(QCloseEvent* event);

/*private:
    bool connectServer_();
    void disconnectServer_();*/

private:
    bool m_bClientConnected;
};

#endif // CLIENTMAINWINDOW_H
