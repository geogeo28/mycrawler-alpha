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

class MCClient : public QObject
{
    Q_OBJECT

public:
    static MCClient* instance();
    static void destroy();

public:
    MCClient(QObject* parent = NULL);
    ~MCClient();

public:
    // Wrapper
    QAbstractSocket::SocketError error() const { return m_clientPeer.error(); }
    QString errorString() const { return m_clientPeer.errorString(); }
    QAbstractSocket::SocketState state() const { return m_clientPeer.state(); }

    void connectToHost(const QString& address, quint16 port);
    void connectToHost(const QHostAddress& address, quint16 port) { connectToHost(address.toString(), port); }

signals:
    void error(QAbstractSocket::SocketError error);
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void connectionStateChanged(QAbstractSocket::SocketState state);
    void connected();
    void disconnected();

public slots:
    void disconnect(int msecs = 30000);

private:
    static MCClient* s_instance;
    MCClientPeer m_clientPeer;

};

#endif // CLIENT_H
