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

#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>

#include "ClientThread.h"

class MCServer : public QTcpServer
{
    Q_OBJECT

public:
    typedef enum {
      NoError,
      UnknownError,
      ServerFullError
    } Error;

public:
    static MCServer* instance();
    static void destroy();

    MCServer(QObject* parent = NULL);
    ~MCServer();

public:
    Error error() const { return m_enumError; }
    QString errorString() const { return m_sError; }
    int maxConnections() const;
    void setMaxConnections(int n);
    bool canAcceptNewConnection() const;

public:
    bool addClient(MCClientThread* client);
    void removeClient(MCClientThread* client);

signals:
    void error(MCServer::Error error);
    void clientError(MCClientThread* client, MCClientThread::Error error);
    void clientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state);
    void clientTimeout(MCClientThread* client, MCClientPeer::TimeoutNotify notifiedWhen);
    void clientKeepAliveNotify(MCClientThread* client);

private slots:
    void clientDisconnected_();
    void clientError_(MCClientThread::Error error) { emit clientError(senderClientThread_(), error); }
    void clientConnectionStateChanged_(MCClientThread::ConnectionState state) { emit clientConnectionStateChanged(senderClientThread_(), state); }
    void clientTimeout_(MCClientPeer::TimeoutNotify notifiedWhen) { emit clientTimeout(senderClientThread_(), notifiedWhen); }
    void clientKeepAliveNotify_() { emit clientKeepAliveNotify(senderClientThread_()); }

protected:
    void incomingConnection(int socketDescriptor);

private:
    inline MCClientThread* senderClientThread_() const { return qobject_cast<MCClientThread*>(sender()); }
    void setError_(Error error, bool signal = true);

private:
    static MCServer* s_instance;

    Error m_enumError;
    QString m_sError;

    QList<MCClientThread*> m_lstClientThreads;
};

#endif // SERVER_H
