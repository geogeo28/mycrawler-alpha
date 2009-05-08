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

#include "ClientPeer.h"

class MCClientThreadInfo {
  public:
    MCClientThreadInfo(
      const QString& peerName = QString(),
      const QHostAddress& peerAddress = QHostAddress(),
      quint16 peerPort = 0
    );

    const QString& peerName() const { return m_sPeerName; }
    const QHostAddress& peerAddress() const { return m_peerAddress; }
    quint16 peerPort() const { return m_u16PeerPort;}
    QString peerAddressAndPort() const;

    void setPeerName(const QString& peerName) { m_sPeerName = peerName; }
    void setPeerAddress(const QHostAddress& peerAddress) { m_peerAddress = peerAddress; }
    void setPeerPort(quint16 peerPort) { m_u16PeerPort = peerPort; }

  private:
    QString m_sPeerName;
    QHostAddress m_peerAddress;
    quint16 m_u16PeerPort;
};

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
      ConnectedState,
      ClosingState,
      ListeningState
    } ConnectionState;

public:
    MCClientThread(int socketDescriptor, QObject* parent = NULL);
    ~MCClientThread();

    const MCClientPeer* clientPeer() { return m_pClientPeer; }
    const MCClientThreadInfo& threadInfo() const { return m_threadInfo; }
    Error error() const { return m_enumError; }
    QString errorString() const { return m_sError; }
    ConnectionState connectionState() const { return m_enumConnectionState; }

public:
    static QString connectionStateToString(ConnectionState state);

signals:
    void error(MCClientThread::Error error);
    void connectionStateChanged(MCClientThread::ConnectionState state);
    void connected();
    void disconnected();
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);

private slots:
    void peerError_(QAbstractSocket::SocketError socketError);
    void peerStateChanged_(QAbstractSocket::SocketState socketState);

protected:
    void run();

private:
    void setError_(Error error, bool signal = true);
    void setConnectionState_(ConnectionState state, bool signal);

private:
    QMutex m_mutex;

    MCClientThreadInfo m_threadInfo;

    Error m_enumError;
    QString m_sError;

    int m_nSocketDescriptor;
    QPointer<MCClientPeer> m_pClientPeer;
    ConnectionState m_enumConnectionState;
};

#endif // CLIENTTHREAD_H
