/****************************************************************************
 * @(#) Server table of IP.
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

#ifndef SERVERTABLEIP_H
#define SERVERTABLEIP_H

#include <QMap>
#include <QMultiMap>

class MCClientThread;

class MCServerTableIP
{
public:
    typedef QMap<quint64, MCClientThread*> RemoteClientsList;
    typedef QMap<quint16, MCClientThread*> LocalClientsList;
    typedef QMultiMap<quint32, quint64> RemoteIPList;

public:
    MCServerTableIP();

    void addClient(MCClientThread* client);
    bool isHardwareAddressRegistered(quint64 hardwareAddress) const;
    MCClientThread* remoteClient(quint64 hardwareAddress) const;
    MCClientThread* localClient(quint16 port) const;
    void removeClient(MCClientThread* client);

private:
    RemoteClientsList m_mapRemoteClientThreads;
    LocalClientsList m_mapLocalClientThreads;
    RemoteIPList m_mapRemoteIP;
};

#endif // SERVERTABLEIP_H
