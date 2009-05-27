/****************************************************************************
 * @(#) Client component.
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

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QHostAddress>

#include "ClientPeer.h"
#include "ServerInfo.h"

class CNetworkInfo;
class MCServerInfo;

class MCClient : public QObject
{
    Q_OBJECT

public:
    typedef enum {
      InvalidState,
      UnconnectedState,
      HostLookupState,
      ConnectingState,
      AuthenticatingState,
      ConnectedState,
      ClosingState
    } ConnectionState;

public:
    static MCClient* instance();
    static void destroy();   

private:
    void init_();

public:
    MCClient(QObject* parent = NULL);
    ~MCClient();

public:
    const MCServerInfo& serverInfo() const { return m_serverInfo; }

    // Wrapper
    QAbstractSocket::SocketError error() const { return m_clientPeer.error(); }
    QString errorString() const { return m_clientPeer.errorString(); }

    void connectToHost(const MCServerInfo& serverInfo);
    void connectToHost(const QHostAddress& address, quint16 port) { connectToHost(MCServerInfo(address, port)); }

    ConnectionState connectionState() const { return m_enumConnectionState; }
    bool isAuthenticated() const { return m_bAuthenticated; }
    bool isConnectionRefused() const { return m_bConnectionRefused; }

public:
    static QString connectionStateToString(ConnectionState state);

signals:
    void error(QAbstractSocket::SocketError error);
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void connectionStateChanged(MCClient::ConnectionState state);
    void connected();
    void disconnected();

public slots:
    void disconnect(int msecs = 30000);

private slots:
    void peerError_(QAbstractSocket::SocketError socketError);
    void peerStateChanged_(QAbstractSocket::SocketState socketState);
    void peerAuthenticated_(const CNetworkInfo& networkInfo);
    void peerServerInfoResponse_(const MCServerInfo& serverInfo);

private:
    void setConnectionState_(ConnectionState state, bool signal);

private:
    static MCClient* s_instance;
    MCClientPeer m_clientPeer;
    MCServerInfo m_serverInfo;

    ConnectionState m_enumConnectionState;
    bool m_bAuthenticated;

    bool m_bConnectionRefused;
};

#endif // CLIENT_H
