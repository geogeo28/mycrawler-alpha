/****************************************************************************
 * @(#) Multi-threads server component.
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

#include "ui_ServerMainWindow.h"
#include <QtGui>

#include "Server.h"

class ServerMainWindow : public QMainWindow,
                         private Ui_ServerMainWindowClass
{
    Q_OBJECT

private:
    void setupWindow_();
    void setupComponents_();

    void cleanAll_();
    void closeWindow_();

public:
    ServerMainWindow(QWidget *parent = 0);
    ~ServerMainWindow();

private slots:
    void on_buttonServerListen_clicked();

    void slotClientNewConnection(QAbstractSocket* clientSocket);
    void slotClientStateChanged(QAbstractSocket::SocketState state);

private:
    bool startServerConnection_();
    void stopServerConnection_();

private:
    CServer* m_pServer;
};

Q_DECLARE_METATYPE(QTreeWidgetItem*);


#endif // SERVERMAINWINDOW_H
