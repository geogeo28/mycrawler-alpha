/****************************************************************************
 * @(#) Multi-threads server component.
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

#ifndef MCSERVER_H
#define MCSERVER_H

#include <QtNetwork>

class MCClientPeer;

class MCServer : public QTcpServer
{
  Q_OBJECT
  
public:
    static MCServer* instance();

    MCServer(QObject* parent = NULL);
    virtual ~MCServer();

public:
    void addClientPeer(MCClientPeer* client);
    void removeClientPeer(MCClientPeer* client);

protected:
    void incomingConnection(int socketDescriptor);

private:    
    QList<MCClientPeer*> m_lstClientsPeer;
};

#endif // MCSERVER_H
