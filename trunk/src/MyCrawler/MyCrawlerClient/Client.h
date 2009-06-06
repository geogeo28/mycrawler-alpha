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
#include <QByteArray>

#include "ClientPeer.h"
#include "ServerInfo.h"

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
      ConnectedState,
      ClosingState
    } ConnectionState;

    typedef enum {
      UnavailableState,
      IdleState,
      CrawlState,
      SendNodesState
    } State;

public:
    static MCClient* instance();
    static void destroy();   

private:
    void init_();

public:
    MCClient(QObject* parent = NULL);
    ~MCClient();

public:
    static int DefaultSeedUrlRequestInterval;

public:
    int seedUrlRequestInterval() const { return m_nSeedUrlRequestInterval; }
    void setSeedUrlRequestInterval(int sec) { m_nSeedUrlRequestInterval = sec * 1000; }

    MCServerInfo serverInfo() const { return m_serverInfo; }

    // Wrapper
    QAbstractSocket::SocketError error() const { return m_clientPeer.error(); }
    QString errorString() const { return m_clientPeer.errorString(); }

    State state() const { return m_enumState; }
    bool setState(State state);

    void connectToHost(const MCServerInfo& serverInfo);
    void connectToHost(const QHostAddress& address, quint16 port) { connectToHost(MCServerInfo(address, port)); }

    ConnectionState connectionState() const { return m_enumConnectionState; }
    bool hasHandShakeReceived() const { return m_bHandShakeReceived; }
    bool isConnectionRefused() const { return m_bConnectionRefused; }

    void sendSeedUrlRequest();

    void sendDataNodes(int n, const QByteArray& nodes);
    void sendLinkNodes(int n, const QByteArray& links);

public:
    static QString connectionStateToString(ConnectionState state);
    static QString stateToString(State state);

signals:
    void error(QAbstractSocket::SocketError error);
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, MCClientPeer::ErrorBehavior errorBehavior);
    void connectionStateChanged(MCClient::ConnectionState state);
    void connected();
    void disconnected();
    void stateChanged(MCClient::State state);
    void seedUrlReceived(MCUrlInfo urlInfo);

public slots:
    void disconnect(int msecs = 30000);

private slots:
    void peerError_(QAbstractSocket::SocketError socketError);
    void peerStateChanged_(QAbstractSocket::SocketState socketState);
    void peerConnected_();
    void peerRequestDenied_(MCClientPeer::PacketType requestPacketType);
    void peerHandShakeReceived_();
    void peerServerInfoResponse_(const MCServerInfo& serverInfo);
    void peerSeedUrlResponse_(const QString& url, quint32 depth);

protected:
    void timerEvent(QTimerEvent *event);

private:
    void initConnection_();
    void killAllTimers_();
    void connecting_();
    void connected_();
    void closing_();
    void disconnected_();
    void setConnectionState_(ConnectionState state);
    void setState_(State state);

private:
    static MCClient* s_instance;

    MCClientPeer m_clientPeer;
    MCServerInfo m_serverInfo;

    ConnectionState m_enumConnectionState;
    bool m_bHandShakeReceived;
    bool m_bConnectionRefused;

    State m_enumState;

    int m_nSeedUrlRequestInterval;
    int m_idSeedUrlRequestTimer;
};

#endif // CLIENT_H
