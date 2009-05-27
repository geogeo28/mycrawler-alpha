/****************************************************************************
 * @(#) Server history.
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

#ifndef SERVERHISTORY_H
#define SERVERHISTORY_H

#include <QMap>
#include <QDataStream>

#include "Debug/Exception.h"
#include "Utilities/NetworkInfo.h"

class MCClientThread;

class MCServerHistory
{  
public:
    static MCServerHistory* instance();
    static void destroy();

private:
    void init_();
    void cleanAll_();

protected:
    MCServerHistory();
    ~MCServerHistory();

public:
    bool isHardwareAddressRegistered(quint64 hardwareAddress) const;
    void addClient(const CNetworkInfo& networkInfo);
    void addClient(MCClientThread* client);
    bool removeClient(quint64 hardwareAddress);
    bool removeClient(MCClientThread* client);
    QList<CNetworkInfo> allClients() const;

public:
    void write(QDataStream& out) const;
    void read(QDataStream& in);

    void save(const QString& fileName) const throw(CFileException);
    void load(const QString& fileName) throw(CFileException);

private:
    static MCServerHistory* s_instance;

    typedef QMap<quint64, CNetworkInfo> ClientsNetworkInfoList;

    ClientsNetworkInfoList m_lstClients;
};

QDataStream& operator<<(QDataStream& out, const MCServerHistory& history);
QDataStream& operator>>(QDataStream& in, MCServerHistory& history);

#endif // SERVERHISTORY_H
