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
#include "ServerTableIP.h"
#include "ServerInfo.h"

/*!
  \note An instance of MCClientThread is created in the private method 'incomingConnection'.
  This instance is deleted after the signal 'clientFinished' is emitted.
  In other words, the parameter 'client*' of all signals is thread-safe if you connect
  signals to slots in Direct Connection mode. If you use a Queue Connection, don't forget to
  lock the mutex.
 */
class MCServer : public QTcpServer
{
    Q_OBJECT

public:
    typedef enum {
      NoError,
      UnknownError,
      TcpSocketError,
      ServerFullError,
      ListenError
    } Error;

    typedef enum {
      ClosedState,
      ListeningState,
      ClosingState
    } State;

public:
    static MCServer* instance();
    static void destroy();

    MCServer(QObject* parent = NULL);
    ~MCServer();

public:
    void setListenAddress(const QHostAddress& address) { m_listenAddress = address; }
    void setListenAddress(const QString& address) { m_listenAddress = QHostAddress(address); }
    const QHostAddress& listenAddress() const { return m_listenAddress; }
    quint16 listenPort() const { return m_u16ListenPort; }
    void setListenPort(quint16 port) { m_u16ListenPort = port; }

    State state() const { return m_enumState; }
    Error error() const { return m_enumError; }
    QString errorString() const { return m_sError; }
    const QString& name() const { return m_sName; }
    void setName(const QString& name) { m_sName = name; }
    int maxConnections() const { return m_nMaxConnections; }
    void setMaxConnections(int n) { m_nMaxConnections = n; }
    bool canAcceptNewConnection() const;

    MCServerInfo serverInfo() const; // thread-safe

public:
    // Override QTcpServer methods
    bool listen();
    void close();
    bool isClosed() const { return state() == ClosedState; }
    void waitForClosed() const;

    bool addClient(MCClientThread* client);
    void removeClient(MCClientThread* client);
    int clientCount() const { return m_lstClientThreads.count(); }
    MCClientThread* clientFromId(quint64 clientId);

    bool isHardwareAddressRegistered(quint64 hardwareAddress) const { return m_lstClientsIP.isHardwareAddressRegistered(hardwareAddress); }

public:
    static int defaultMaxConnections();
    static QString stateToString(State state);

signals:
    void error(MCServer::Error error);
    void stateChanged(MCServer::State state);
    void closed();

    void clientError(MCClientThread* client, MCClientThread::Error error);
    void clientTimeout(MCClientThread* client, MCClientPeer::TimeoutNotify notifiedWhen);
    void clientErrorProcessingPacket(MCClientThread* client, MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void clientConnectionRefused(MCClientThread* client, const QString& reason);
    void clientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state);
    void clientConnected(MCClientThread* client, bool unknownClientInTheServerHistory);
    void clientDisconnected(MCClientThread* client);
    void clientFinished(MCClientThread* client);

private slots:
    void clientError_(MCClientThread::Error error);
    void clientTimeout_(MCClientPeer::TimeoutNotify notifiedWhen);
    void clientErrorProcessingPacket_(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void clientConnectionStateChanged_(MCClientThread::ConnectionState state);
    void clientDisconnected_();
    void clientFinished_();

protected:
    void incomingConnection(int socketDescriptor);

private:
    inline MCClientThread* senderClientThread_() const { return qobject_cast<MCClientThread*>(sender()); }
    void setError_(Error error, bool signal = true);
    void setState_(State state);
    void refuseClientConnection_(MCClientThread* client, const QString& reason = QString());

private:
    typedef QSet<MCClientThread*> ClientThreadsList;

    static MCServer* s_instance;

    QString m_sName;
    int m_nMaxConnections;
    State m_enumState;

    Error m_enumError;
    QString m_sError;

    QHostAddress m_listenAddress;
    quint16 m_u16ListenPort;

    ClientThreadsList m_lstClientThreads;
    MCServerTableIP m_lstClientsIP;
};

#endif // SERVER_H
