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
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QList>

#include "Utilities/NetworkInfo.h"

#include "ServerInfo.h"

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

    typedef enum {
      KeepAliveAcknowledgmentPacket = 0xFFFF,
      KeepAlivePacket = 0,
      AuthenticationPacket,
      ConnectionRefusedPacket,
      ServerInfoRequestPacket = 1024, // Starting requests
      SeedUrlsRequestPacket,
      ServerInfoResponsePacket = 2048, // Starting responses
      SeedUrlsResponsePacket,
    } PacketType;

    typedef enum {
      PacketUnknownError,
      PacketSizeError,
      PacketTypeError,
      ProtocolIdError,
      ProtocolVersionError,
      AuthenticationError
    } PacketError;

public:
    MCClientPeer(QObject* parent = NULL);
    ~MCClientPeer();

public:
    static int DefaultTimeout;
    static int DefaultConnectTimeout;
    static int DefaultHandShakeTimeout;
    static int DefaultKeepAliveInterval;
    static bool DefaultRequireAuthentication;

public:
    int timeout() { return m_nTimeout; }
    int connectTimeout() { return m_nConnectTimeout; }
    int handShakeTimeout() { return m_nHandShakeTimeout; }
    int keepAliveInterval() { return m_nKeepAliveInterval; }
    bool requireAuthentication() { return m_bRequireAuthentication; } // Server mode

    void setTimeout(int sec) { m_nTimeout = sec * 1000; }
    void setConnectTimeout(int sec) { m_nConnectTimeout = sec * 1000; }
    void setHandShakeTimeout(int sec) { m_nHandShakeTimeout = sec * 1000; }
    void setKeepAliveInterval(int sec) { m_nKeepAliveInterval = sec * 1000; }
    void setRequireAuthentication(bool requireAuthentication) { m_bRequireAuthentication = requireAuthentication;}

    bool isAuthenticated() const { return m_bAuthenticated; }
    const CNetworkInfo& networkInfo() const { return m_networkInfo; }

    void sendPacket(PacketType type, const QByteArray& data = QByteArray());
    template <class T> void sendPacket(PacketType type, const T& data);  

public:
    static QString stateToString(QAbstractSocket::SocketState state);
    static QString timeoutNotifyToString(TimeoutNotify notify);
    static QString packetTypeToString(PacketType type);
    static QString packetErrorToString(PacketError error);

signals:
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted);
    void handShakeReceived();
    void authenticated(const CNetworkInfo& info);
    void packetSent(MCClientPeer::PacketType type, quint32 size);

    void serverInfoRequest();
    void seedUrlsRequest();

    void serverInfoResponse(const MCServerInfo& serverInfo);
    void seedUrlsResponse(const QList<QString>& urls);

public slots:
    void refuseConnection(const QString& reason = QString());
    void disconnect(int msecs = 30000);
    void sendHandShake() { sendHandShakePacket_(); }
    void sendAuthentication() { sendAuthenticationPacket_(); }

    void sendServerInfoRequest() { sendServerInfoRequestPacket_(); }
    void sendSeedUrlsRequest() { sendSeedUrlsRequestPacket_(); }

    void sendServerInfoResponse(const MCServerInfo& serverInfo) { sendServerInfoResponsePacket_(serverInfo); }
    void sendSeedUrlsResponse(const QList<QString>& urls) { sendSeedUrlsResponsePacket_(urls); }

private slots:
    void connectionStateChanged_(QAbstractSocket::SocketState state);
    void processIncomingData_();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void errorProcessingPacket_(MCClientPeer::PacketError error, bool aborted = true);
    void sendHandShakePacket_();
    void sendAuthenticationPacket_();
    void sendConnectionRefusedPacket_(const QString& reason = QString());

    void sendServerInfoRequestPacket_();
    void sendSeedUrlsRequestPacket_();

    void sendServerInfoResponsePacket_(const MCServerInfo& serverInfo);
    void sendSeedUrlsResponsePacket_(const QList<QString>& urls);

    CNetworkInfo processAuthenticationPacket_(QDataStream& data);
    void processConnectionRefusedPacket_(QDataStream& data);

    void processServerInfoRequestPacket_();
    void processSeedUrlsRequestPacket_();

    void processServerInfoResponsePacket_(QDataStream& data);
    void processSeedUrlsResponsePacket_(QDataStream& data);

private:
    void connecting_();
    void connected_();
    void closing_();
    void disconnected_();
    void processPacket_();

private:
    int m_nTimeout;
    int m_nConnectTimeout;
    int m_nHandShakeTimeout;
    int m_nKeepAliveInterval;
    bool m_bRequireAuthentication;

    int m_idTimeoutTimer;
    int m_idKeepAliveTimer;
    bool m_bInvalidateTimeout;

    bool m_bReceivedHandShake, m_bSentHandShake;
    quint32 m_u32PacketSize;
    quint16 m_u16PacketType;

    bool m_bAuthenticated;
    CNetworkInfo m_networkInfo;
    QTime m_timeStartPingRequest;
};

template <class T> void MCClientPeer::sendPacket(PacketType type, const T& data) {
  QByteArray bytes;
  QDataStream in(&bytes, QIODevice::WriteOnly);
  in.setVersion(SerializationVersion);

  in << data;
  sendPacket(type, bytes);
}

#endif // CLIENTPEER_H
