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
      PeerTimeoutNotify,
    } TimeoutNotify;

public:
    MCClientPeer(QObject* parent = NULL);
    ~MCClientPeer();

public:
    static int timeout() { return s_nTimeout; }
    static int connectTimeout() { return s_nConnectTimeout; }
    static int keepAliveInterval() { return s_nKeepAliveInterval; }
    static void setTimeout(int n) { s_nTimeout = n * 1000; }
    static void setConnectTimeout(int n) { s_nConnectTimeout = n * 1000; }
    static void setKeepAliveInterval(int n) { s_nKeepAliveInterval = n * 1000; }

    void setConnected();

signals:
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void packetKeepAliveSended();

public slots:
    void connectionRefused();

private slots:
    void processIncomingData_();
    void connected_();

protected:
    void timerEvent(QTimerEvent *event);

private:
    static int s_nTimeout;
    static int s_nConnectTimeout;
    static int s_nKeepAliveInterval;

    int m_idTimeoutTimer;
    int m_idKeepAliveTimer;
    bool m_bInvalidateTimeout;

    bool m_bReceivedHandShake;

};

#endif // CLIENTPEER_H
