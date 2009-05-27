/****************************************************************************
 * @(#) Servers Tree Widget.
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

#ifndef SERVERSTREEWIDGET_H
#define SERVERSTREEWIDGET_H

#include <QTreeWidget>

#include "Widgets/MyQTreeWidget.h"

class MCServerInfo;

class MCServersTreeWidget : public MyQTreeWidget
{
    Q_OBJECT

public:
    enum {
      NameColumn,
      IPColumn,
      PortColumn,
      PingColumn,
      UsersColumn,
      MaxUsersColumn,
      PriorityColumn
    };

private:
    void cleanAll_();

public:
    MCServersTreeWidget(QWidget* parent = 0);
    ~MCServersTreeWidget();

    void setup();

private slots:
    void slotServerAdded(const MCServerInfo& serverInfo);
    void slotServerRemoved(quint32 ip);
    void slotAllServersRemoved();
    void slotServerUpdated(const MCServerInfo& serverInfo);

private slots:
    void on_connectToServer();
    void on_priorityChanged(QAction* action);
    void on_remove();
    void on_removeAll();

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private:
    static void unsetServerItemValues_(QTreeWidgetItem* item);
    static void setServerItemFromServerInfo_(QTreeWidgetItem* item, const MCServerInfo& serverInfo);

private:
    QTreeWidgetItem* newServerItem_();
    void newServerItemFromServerInfo_(const MCServerInfo& serverInfo);

private:
    typedef QMap<quint32, QTreeWidgetItem*> ServersManagedList; // ip, info

    ServersManagedList m_lstServersManaged;
};

#endif // SERVERSTREEWIDGET_H
