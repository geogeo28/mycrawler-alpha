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

#include <QTcpSocket>
#include <QList>
#include <QQueue>
#include <QMap>

class QString;
class QTimerEvent;
class QDataStream;

class CNetworkInfo;
class MCServerInfo;

class MCClientPeerRequestInfo;
class MCClientPeerRequestsQueuesContainer;

class MCClientPeerPrivate;

class MCClientPeer : public QTcpSocket
{
    Q_OBJECT

public:
    enum {
      SystemPacketsStart   = 0,    SystemPacketsEnd = 1023,
      RequestPacketsStart  = 1024, RequestPacketsEnd = 2047,
      ResponsePacketsStart = 2048, ResponsePacketsEnd = 3071,
      MessagePacketsStart  = 3072, MessagePacketsEnd = 4096
    };

    typedef enum {
      UnknownTimeoutNotify,
      ConnectTimeoutNotify,
      HandShakeTimeoutNotify,
      PendingRequestTimeoutNotify,
      PeerTimeoutNotify,
    } TimeoutNotify;

    typedef enum {
      KeepAliveAcknowledgmentPacket = 0xFFFF,
      KeepAlivePacket = SystemPacketsStart,
      AuthenticationPacket,
      ConnectionRefusedPacket,

      ServerInfoRequestPacket = RequestPacketsStart, // Starting requests
      SeedUrlsRequestPacket,

      ServerInfoResponsePacket = ResponsePacketsStart, // Starting responses
      SeedUrlsResponsePacket,
    } PacketType;

    typedef enum {
      PacketUnknownError,
      PacketSizeError,
      PacketTypeError,
      ProtocolIdError,
      ProtocolVersionError,
      AuthenticationError,
      ResponseWithoutRequestError
    } PacketError;        

public:
    MCClientPeer(QObject* parent = NULL);
    ~MCClientPeer();

private:
    Q_DISABLE_COPY(MCClientPeer);

public:
    static int DefaultTimeout;
    static int DefaultConnectTimeout;
    static int DefaultHandShakeTimeout;
    static int DefaultPendingRequestTimeout;
    static int DefaultKeepAliveInterval;
    static bool DefaultRequireAuthentication;

public:
    int timeout() const;
    int connectTimeout() const;
    int handShakeTimeout() const;
    int pendingRequestTimeout() const;
    int keepAliveInterval() const;
    bool requireAuthentication() const; // Server mode

    void setTimeout(int sec);
    void setConnectTimeout(int sec);
    void setHandShakeTimeout(int sec);
    void setPendingRequestTimeout(int sec);
    void setKeepAliveInterval(int sec);
    void setRequireAuthentication(bool requireAuthentication);

    bool isAuthenticated() const;
    CNetworkInfo networkInfo() const;
    const MCClientPeerRequestsQueuesContainer& pendingRequests() const;
    QString dumpPendingRequests() const;

public:
    static QString stateToString(QAbstractSocket::SocketState state);
    static QString timeoutNotifyToString(TimeoutNotify notify);
    static QString packetTypeToString(PacketType packetType);
    static QString packetErrorToString(PacketError error);

signals:
    void timeout(MCClientPeer::TimeoutNotify notifiedWhen);
    void errorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType packetType, quint32 packetSize, bool aborted);
    void handShakeReceived();
    void authenticated(const CNetworkInfo& info);
    void packetSent(MCClientPeer::PacketType packetType, quint32 packetSize);
    void packetReceived(MCClientPeer::PacketType packetType, quint32 packetSize, const MCClientPeerRequestInfo requestInfo);

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

    void sendPacket_(PacketType packetType, const QByteArray& data = QByteArray());
    template <class T> void sendPacket_(PacketType packetType, const T& data);

    void sendHandShakePacket_();
    void sendAuthenticationPacket_();
    void sendConnectionRefusedPacket_(const QString& reason = QString());

    void sendServerInfoRequestPacket_();
    void sendSeedUrlsRequestPacket_();

    void sendServerInfoResponsePacket_(const MCServerInfo& serverInfo);
    void sendSeedUrlsResponsePacket_(const QList<QString>& urls);

 private:
    CNetworkInfo processAuthenticationPacket_(QDataStream& data);
    void processConnectionRefusedPacket_(QDataStream& data);

    void processServerInfoRequestPacket_();
    void processSeedUrlsRequestPacket_();

    void processServerInfoResponsePacket_(const MCClientPeerRequestInfo& requestInfo, QDataStream& data);
    void processSeedUrlsResponsePacket_(const MCClientPeerRequestInfo& requestInfo, QDataStream& data);

private:
    void initConnection_();
    void killAllTimers_();
    void connecting_();
    void connected_();
    void closing_();
    void disconnected_();
    MCClientPeerRequestInfo registerRequest_(PacketType requestPacketType);
    MCClientPeerRequestInfo unregisterRequest_(PacketType requestPacketType);
    void processPacket_(PacketType packetType, const MCClientPeerRequestInfo& requestInfo);

private:
    QSharedDataPointer<MCClientPeerPrivate> d;
    friend class MCClientPeerPrivate;
};


class MCClientPeerRequestInfoPrivate;

class MCClientPeerRequestInfo
{
    friend class MCClientPeer;

public:
    MCClientPeerRequestInfo(); // Create an invalid request info
    MCClientPeerRequestInfo(MCClientPeer::PacketType requestPacketType);
    MCClientPeerRequestInfo(const MCClientPeerRequestInfo &other);
    MCClientPeerRequestInfo& operator=(const MCClientPeerRequestInfo& requestInfo);
    ~MCClientPeerRequestInfo();

    bool isValid() const;

    MCClientPeer::PacketType requestPacketType() const;
    MCClientPeer::PacketType responsePacketType() const;

    bool responseReceived() const;

    QTime beginTime() const;
    QTime endTime() const;
    int delay() const; // -1 if the response was not received

// Can only be changed by MCClientPeer
private:
    void responseReceivedNotify();

private:
    QSharedDataPointer<MCClientPeerRequestInfoPrivate> d;
    friend class MCClientPeerRequestInfoPrivate;
};

class MCClientPeerRequestsQueuesContainer
{
public:
    typedef QQueue<MCClientPeerRequestInfo> RequestsQueue;
    typedef QMap<MCClientPeer::PacketType,RequestsQueue> RequestsQueuesContainer;

public:
    MCClientPeerRequestsQueuesContainer();

    int requestCount(MCClientPeer::PacketType requestPacketType) const;
    MCClientPeerRequestInfo addRequest(MCClientPeer::PacketType requestPacketType);
    MCClientPeerRequestInfo takeRequest(MCClientPeer::PacketType requestPacketType);

    QString dumpPendingRequests() const;

private:
    RequestsQueuesContainer m_requestsQueuesContainer;
};

#endif // CLIENTPEER_H
