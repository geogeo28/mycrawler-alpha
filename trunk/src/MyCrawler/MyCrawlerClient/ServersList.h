/****************************************************************************
 * @(#) Servers list.
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

#ifndef SERVERSLIST_H
#define SERVERSLIST_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QHostAddress>
#include <QTextStream>

#include "Debug/Exception.h"

class MCServerInfo;

class MCServersList : public QObject
{
    Q_OBJECT

private:
    void init_();
    void cleanAll_();

public:
    static MCServersList* instance();
    static void destroy();

protected:
    MCServersList(QObject* parent = NULL);
    virtual ~MCServersList();

public:
    bool ipExists(quint32 ip) const;
    MCServerInfo& fromIp(quint32 ip); // unsafe
    void addServer(const MCServerInfo& serverInfo);
    bool removeServer(quint32 ip);
    void removeAllServers();
    void updateServer(quint32 ip, const MCServerInfo& serverInfo);
    QList<MCServerInfo> allServers() const;
    QList<MCServerInfo> serversListSorted() const;

public:
    void write(QTextStream& out) const;
    bool read(QTextStream& in);

    void save(const QString& fileName) const throw(CFileException);
    void load(const QString& fileName) throw(CFileException);

signals:
    void serverAdded(const MCServerInfo& serverInfo);
    void serverRemoved(quint32 ip);
    void allServersRemoved();
    void serverUpdated(const MCServerInfo& serverInfo);

private:
    typedef QMap<quint32, MCServerInfo> ServersList;

    static MCServersList* s_instance;

    ServersList m_lstServers;
};

#endif // SERVERSLIST_H
