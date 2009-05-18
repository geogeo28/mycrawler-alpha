/****************************************************************************
 * @(#) Multi-threads client thread.
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

#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QMutex>
#include <QPointer>
#include <QHostAddress>

#include "Utilities/NetworkInfo.h"

#include "ClientPeer.h"

class MCClientThread : public QThread
{
    Q_OBJECT

public:
    typedef enum {
      NoError,
      UnknownError,
      ClientPeerError
    } Error;

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
    MCClientThread(int socketDescriptor, QObject* parent = NULL);
    ~MCClientThread();

    const MCClientPeer* clientPeer() { QMutexLocker locker(&mutex); return m_pClientPeer; }

    QString peerName() { QMutexLocker locker(&mutex); return m_sPeerName; }
    QHostAddress peerAddress() { QMutexLocker locker(&mutex); return m_peerAddress; }
    quint16 peerPort() { QMutexLocker locker(&mutex); return m_u16PeerPort; }
    QString peerAddressAndPort() { QMutexLocker locker(&mutex); return QString("%1:%2").arg(peerAddress().toString()).arg(peerPort()); }
    const CNetworkInfo& clientInfo() { QMutexLocker locker(&mutex); return m_clientInfo; }

    bool isLocalClient() { QMutexLocker locker(&mutex); return (peerAddress() == QHostAddress::LocalHost); }
    bool isRemoteClient() { QMutexLocker locker(&mutex); return !isLocalClient(); }

    Error error() { QMutexLocker locker(&mutex); return m_enumError; }
    QString errorString() { QMutexLocker locker(&mutex); return m_sError; }
    ConnectionState connectionState() { QMutexLocker locker(&mutex); return m_enumConnectionState; }
    bool isAuthenticated() { QMutexLocker locker(&mutex); return m_bAuthenticated; }
    bool isConnectionRefused() { QMutexLocker locker(&mutex); return m_bConnectionRefused; }

public:
    static QString connectionStateToString(ConnectionState state);

signals:
    void error(MCClientThread::Error error);
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void connectionStateChanged(MCClientThread::ConnectionState state);
    void connected();
    void disconnected();
    void authenticated(const CNetworkInfo& info);

public slots:
    void refuseConnection(const QString& reason = QString());

private slots:
    void peerError_(QAbstractSocket::SocketError socketError);
    void peerStateChanged_(QAbstractSocket::SocketState socketState);
    void peerAuthenticated_(const CNetworkInfo& info);

protected:
    void run();

signals:
    void callPeerRefuseConnection_(const QString& reason);

private:
    void setError_(Error error, bool signal = true);
    void setConnectionState_(ConnectionState state, bool signal);

private:
    mutable QMutex mutex;

    Error m_enumError;
    QString m_sError;

    int m_nSocketDescriptor;
    QPointer<MCClientPeer> m_pClientPeer;

    QString m_sPeerName;
    QHostAddress m_peerAddress;
    quint16 m_u16PeerPort;
    CNetworkInfo m_clientInfo;

    ConnectionState m_enumConnectionState;
    bool m_bAuthenticated;

    bool m_bConnectionRefused;
};

#endif // CLIENTTHREAD_H
