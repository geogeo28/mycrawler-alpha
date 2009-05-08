/****************************************************************************
 * @(#) Multi-threads client protocol component.
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

#ifndef CLIENTPEER_H
#define CLIENTPEER_H

#include <QtNetwork>

class MCClientPeer : public QTcpSocket
{
    Q_OBJECT

public:
    typedef enum {
      UnknownTimeoutNotify,
      ConnectTimeoutNotify,
      HandShakeTimeoutNotify,
      PeerTimeoutNotify,
    } TimeoutNotify;

private:
    typedef enum {
      HandShakePacket,
      KeepAlivePacket,
      NotificationPacket
    } PacketType;

public:
    MCClientPeer(QObject* parent = NULL);
    ~MCClientPeer();

public:
    static int timeout() { return s_nTimeout; }
    static int connectTimeout() { return s_nConnectTimeout; }
    static int handShakeTimeout() { return s_nHandShakeTimeout; }
    static int keepAliveInterval() { return s_nKeepAliveInterval; }
    static void setTimeout(int sec) { s_nTimeout = sec * 1000; }
    static void setConnectTimeout(int sec) { s_nConnectTimeout = sec * 1000; }
    static void setHandShakeTimeout(int sec) { s_nHandShakeTimeout = sec * 1000; }
    static void setKeepAliveInterval(int sec) { s_nKeepAliveInterval = sec * 1000; }

    void setConnected();

public:
    static QString stateToString(QAbstractSocket::SocketState state);
    static QString timeoutNotifyToString(MCClientPeer::TimeoutNotify notify);

signals:
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void packetKeepAliveSent();

public slots:
    void connectionRefused();
    void disconnect(int msecs = 30000);

private slots:
    void connectionStateChanged_(QAbstractSocket::SocketState state);
    void processIncomingData_();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void connecting_();
    void connected_();
    void closing_();
    void disconnected_();
    void processPacket_();

private:
    static int s_nTimeout;
    static int s_nConnectTimeout;
    static int s_nHandShakeTimeout;
    static int s_nKeepAliveInterval;

    int m_idTimeoutTimer;
    int m_idKeepAliveTimer;
    bool m_bInvalidateTimeout;

    bool m_bReceivedHandShake, m_bSentHandShake;
    quint32 m_u32PacketSize, m_u32PacketType;
};

#endif // CLIENTPEER_H
