/****************************************************************************
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

#include <QString>
#include <QTime>
#include <QByteArray>
#include <QDataStream>

#include "Debug/Exception.h"
#include "Debug/Logger.h"
#include "Utilities/NetworkInfo.h"

#include "Macros.h"
#include "ClientPeer.h"
#include "ServerInfo.h"
#include "UrlInfo.h"

int MCClientPeer::DefaultTimeout = 120 * 1000;
int MCClientPeer::DefaultConnectTimeout = 60 * 1000;
int MCClientPeer::DefaultHandShakeTimeout = 60 * 1000;
int MCClientPeer::DefaultPendingRequestTimeout = 60 * 1000;
int MCClientPeer::DefaultKeepAliveInterval = 30 * 1000;
bool MCClientPeer::DefaultRequireAuthentication = true;

static const char ProtocolId[] = "MyCrawler Protocol";
static const char ProtocolIdSize = 18;
static const quint16 ProtocolVersion = 0x0100;
static const quint16 ProtocolHeaderSize = ProtocolIdSize + sizeof(ProtocolVersion) + 8;
static const quint32 PacketHeaderSize = 6;

class MCClientPeerPrivate : public QSharedData
{
  public:
    MCClientPeerPrivate();

    int timeout;
    int connectTimeout;
    int handShakeTimeout;
    int pendingRequestTimeout;
    int keepAliveInterval;
    bool requireAuthentication;

    int timeoutTimer;
    int pendingRequestTimeoutTimer;
    int keepAliveTimer;
    bool invalidateTimeout;

    bool receivedHandShake, sentHandShake;
    quint32 packetSize;
    quint16 packetType;

    bool authenticated;
    CNetworkInfo networkInfo;

    MCClientPeerRequestsQueuesContainer pendingRequests;
};

MCClientPeerPrivate::MCClientPeerPrivate()
  : timeout(MCClientPeer::DefaultTimeout),
    connectTimeout(MCClientPeer::DefaultConnectTimeout),
    handShakeTimeout(MCClientPeer::DefaultHandShakeTimeout),
    pendingRequestTimeout(MCClientPeer::DefaultPendingRequestTimeout),
    keepAliveInterval(MCClientPeer::DefaultKeepAliveInterval),
    requireAuthentication(MCClientPeer::DefaultRequireAuthentication),

    timeoutTimer(0), pendingRequestTimeoutTimer(0),
    keepAliveTimer(0), invalidateTimeout(false),
    receivedHandShake(false), sentHandShake(false),
    packetSize(0), packetType(0),
    authenticated(false)
{}

MCClientPeer::MCClientPeer(QObject* parent)
  : QTcpSocket(parent),
    d(new MCClientPeerPrivate)
{
  ILogger::Debug() << "Construct.";

  QObject::connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectionStateChanged_(QAbstractSocket::SocketState)));
  QObject::connect(this, SIGNAL(readyRead()), this, SLOT(processIncomingData_()));
}

MCClientPeer::~MCClientPeer() {
  // QSharedDataPointer takes care of deleting for us

  // Pending Requests Dump
  ILogger::Debug() << dumpPendingRequests();

  ILogger::Debug() << "Destroyed.";
}

int MCClientPeer::timeout() const {
  return d->timeout;
}

int MCClientPeer::connectTimeout() const {
  return d->connectTimeout;
}

int MCClientPeer::handShakeTimeout() const {
  return d->handShakeTimeout;
}

int MCClientPeer::pendingRequestTimeout() const {
  return d->pendingRequestTimeout;
}

int MCClientPeer::keepAliveInterval() const {
  return d->keepAliveInterval;
}

bool MCClientPeer::requireAuthentication() const {
  return d->requireAuthentication;
}

void MCClientPeer::setTimeout(int sec) {
  Assert(sec >= 0);
  d->timeout = sec * 1000;
}

void MCClientPeer::setConnectTimeout(int sec) {
  Assert(sec >=0 );
  d->connectTimeout = sec * 1000;
}

void MCClientPeer::setHandShakeTimeout(int sec) {
  Assert(sec >= 0);
  d->handShakeTimeout = sec * 1000;
}

void MCClientPeer::setPendingRequestTimeout(int sec) {
  Assert(sec >=0 );
  d->pendingRequestTimeout = sec * 1000;
}

void MCClientPeer::setKeepAliveInterval(int sec) {
  Assert(sec >= 0);
  d->keepAliveInterval = sec * 1000;
}

void MCClientPeer::setRequireAuthentication(bool requireAuthentication) {
  d->requireAuthentication = requireAuthentication;
}

bool MCClientPeer::isAuthenticated() const {
  return d->authenticated;
}

CNetworkInfo MCClientPeer::networkInfo() const {
  return d->networkInfo;
}

const MCClientPeerRequestsQueuesContainer& MCClientPeer::pendingRequests() const {
  return d->pendingRequests;
}

QString MCClientPeer::dumpPendingRequests() const {
  return pendingRequests().dumpPendingRequests();
}

QString MCClientPeer::stateToString(QAbstractSocket::SocketState state) {
  switch (state) {
    case QAbstractSocket::HostLookupState:   return QT_TRANSLATE_NOOP(MCClientPeer, "Host Lookup");
    case QAbstractSocket::ConnectingState:   return QT_TRANSLATE_NOOP(MCClientPeer, "Connecting");
    case QAbstractSocket::ConnectedState:    return QT_TRANSLATE_NOOP(MCClientPeer, "Connected");
    case QAbstractSocket::BoundState:        return QT_TRANSLATE_NOOP(MCClientPeer, "Bound");
    case QAbstractSocket::ClosingState:      return QT_TRANSLATE_NOOP(MCClientPeer, "Closing");
    case QAbstractSocket::ListeningState:    return QT_TRANSLATE_NOOP(MCClientPeer, "Listening");

    default:
      return QT_TRANSLATE_NOOP(MCClientThread, "Unconnected");
  }
}

QString MCClientPeer::timeoutNotifyToString(TimeoutNotify notify) {
  switch (notify) {
    case ConnectTimeoutNotify:        return QT_TRANSLATE_NOOP(MCClientPeer, "Connection timed out");
    case HandShakeTimeoutNotify:      return QT_TRANSLATE_NOOP(MCClientPeer, "Handshake timed out");
    case PendingRequestTimeoutNotify: return QT_TRANSLATE_NOOP(MCClientPeer, "Pending Request timed out");
    case PeerTimeoutNotify:           return QT_TRANSLATE_NOOP(MCClientPeer, "Peer timed out");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown timeout");
  }
}

QString MCClientPeer::packetTypeToString(PacketType packetType) {
  switch (packetType) {
    // SYSTEM
    case KeepAliveAcknowledgmentPacket: return QT_TRANSLATE_NOOP(MCClientPeer, "KeepAlive (acknowledgment)");
    case KeepAlivePacket:               return QT_TRANSLATE_NOOP(MCClientPeer, "KeepAlive");
    case AuthenticationPacket:          return QT_TRANSLATE_NOOP(MCClientPeer, "Authentication");
    case ConnectionRefusedPacket:       return QT_TRANSLATE_NOOP(MCClientPeer, "Connection refused");
    case RequestDeniedPacket:           return QT_TRANSLATE_NOOP(MCClientPeer, "Request denied");

    // REQUESTS
    case ServerInfoRequestPacket:       return QT_TRANSLATE_NOOP(MCClientPeer, "Server Info Request");
    case SeedUrlRequestPacket:          return QT_TRANSLATE_NOOP(MCClientPeer, "Seed Url Request");

    // RESPONSES
    case ServerInfoResponsePacket:      return QT_TRANSLATE_NOOP(MCClientPeer, "Server Info Response");
    case SeedUrlResponsePacket:         return QT_TRANSLATE_NOOP(MCClientPeer, "Seed Url Response");

    // COMPRESSED MESSAGES
    case DataNodesMessagePacket:        return QT_TRANSLATE_NOOP(MCClientPeer, "Data Nodes message (compressed)");
    case LinkNodesMessagePacket:        return QT_TRANSLATE_NOOP(MCClientPeer, "Link Nodes message (compressed)");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown packet type");
  }
}

QString MCClientPeer::packetErrorToString(PacketError error) {
  switch (error) {
    case PacketSizeError:             return QT_TRANSLATE_NOOP(MCClientPeer, "Packet size error");
    case PacketTypeError:             return QT_TRANSLATE_NOOP(MCClientPeer, "Packet type unknown");
    case ProtocolIdError:             return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown protocol");
    case ProtocolVersionError:        return QT_TRANSLATE_NOOP(MCClientPeer, "Could not establish a communication with the peer because the protocol version is different");
    case AuthenticationError:         return QT_TRANSLATE_NOOP(MCClientPeer, "No authentication message was received");
    case ResponseWithoutRequestError: return QT_TRANSLATE_NOOP(MCClientPeer, "A response was received whereas no request has been sent");
    case InvalidPacketContentError:   return QT_TRANSLATE_NOOP(MCClientPeer, "Invalid Packet Content");
    case CorruptedDataPacketError:    return QT_TRANSLATE_NOOP(MCClientPeer, "Corrupted Data Packet");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown packet error");
  }
}

QString MCClientPeer::errorBehaviorToString(ErrorBehavior errorBehavior) {
  switch (errorBehavior) {
    case AbortBehavior:      return QT_TRANSLATE_NOOP(MCClientPeer, "Connection aborted");
    case DropPacketBehavior: return QT_TRANSLATE_NOOP(MCClientPeer, "Packet dropped");
    case ContinueBehavior:   return QT_TRANSLATE_NOOP(MCClientPeer, "Packet ignored");

    default:
      return QT_TRANSLATE_NOOP(MCClientPeer, "Unknown behavior");
  }
}

void MCClientPeer::refuseConnection(const QString& reason) {
  ILogger::Debug() << "The connection was aborted (reason = " << reason << ").";

  //sendConnectionRefusedPacket_(reason);
  disconnectFromHost();
}

void MCClientPeer::disconnect(int msecs) {
  // Close the connection
  if (state() == MCClientPeer::ConnectingState) {
    ILogger::Debug() << "Close the connection.";
    abort();
  }
  // Force to close the connection properly
  else if (state() != MCClientPeer::UnconnectedState) {
    ILogger::Debug() << "Disconnect the peer.";
    disconnectFromHost();

    if ((msecs > 0) && (state() != MCClientPeer::UnconnectedState)) {
      ILogger::Debug() << "Waiting to disconnect...";
      waitForDisconnected(msecs);
    }
  }
}

void MCClientPeer::connectionStateChanged_(QAbstractSocket::SocketState state) {
  ILogger::Debug() << QString("Connection state changed : %1 (%2).")
                      .arg(stateToString(state))
                      .arg(state);

  switch (state) {
    // Connecting...
    case ConnectingState:
      connecting_();
      break;

    // Connected
    case ConnectedState:
      connected_();
      break;

    // Closing
    case ClosingState:
      closing_();
      break;

    // Disconnected
    case UnconnectedState:
      disconnected_();
      break;

    default:;
  }
}

void MCClientPeer::processIncomingData_() {
  d->invalidateTimeout = true;

  // Process Handshake
  if (d->receivedHandShake == false) {
    // Check that we received enough data
    if (bytesAvailable() < ProtocolHeaderSize) {
      return;
    }

    // Check the protocol identification
    QByteArray id = read(ProtocolIdSize);
    if (id.startsWith(ProtocolId) == false) {
      errorProcessingPacket_(ProtocolIdError, AbortBehavior);
      return;
    }

    // Check the protocol version
    MC_DATASTREAM_READ(this, in);

    quint16 ver; in >> ver;
    if (ver != ProtocolVersion) {
      errorProcessingPacket_(ProtocolVersionError, AbortBehavior);
      return;
    }

    // Discard 8 reserved bytes
    (void) read(8);

    // Initialize keep-alive timer
    if (!d->keepAliveTimer) {
      d->keepAliveTimer = startTimer(d->keepAliveInterval);
    }

    d->receivedHandShake = true;
    emit handShakeReceived();

    /*if (d->sentHandShake == false) {
      sendHandShakePacket_();
    }*/
  }

  do {
    // New packet
    if (d->packetSize == 0) {
      // Check that we received enough data
      if (bytesAvailable() < PacketHeaderSize) {
        return;
      }

      // Get packet size and packet type
      MC_DATASTREAM_READ(this, in);

      in >> d->packetSize;
      in >> d->packetType;

      // Prevent of a DoS attack
      if ((d->packetSize < PacketHeaderSize) || (d->packetSize > 512000)) {
        errorProcessingPacket_(PacketSizeError, AbortBehavior);
        return;
      }
    }

    // Attempts to have enough bytes to process the packet
    if (bytesAvailable() < (d->packetSize - PacketHeaderSize)) {
      return;
    }

    PacketType packetType = static_cast<PacketType>(d->packetType);
    MCClientPeerRequestInfo requestInfo; // Invalid request info

    // Response packet
    if ((d->packetType >= ResponsePacketsStart) && (d->packetType <= ResponsePacketsEnd)) {
      // Find the request linked with the response
      PacketType requestPacketType = static_cast<PacketType>(
        (d->packetType - ResponsePacketsStart) + RequestPacketsStart
      );

      requestInfo = unregisterRequest_(requestPacketType);

      // A response was received when no request has been sent
      if (requestInfo.isValid() == false) {
        errorProcessingPacket_(ResponseWithoutRequestError, DropPacketBehavior);
        goto LabelEndOfPacketProcessing;
      }
    }

    // Process packet
    processPacket_(packetType, d->packetSize, requestInfo);

    emit packetReceived(packetType, d->packetSize, requestInfo);

LabelEndOfPacketProcessing:
    d->packetSize = 0;
  } while (bytesAvailable() >= PacketHeaderSize);
}

void MCClientPeer::timerEvent(QTimerEvent *event) {
  // Timeout (Connect, Handshake, Peer)
  if (event->timerId() == d->timeoutTimer) {
    // Disconnect if we timed out; otherwise the timeout is restarted.
    if (d->invalidateTimeout == true) {
      d->invalidateTimeout = false;
    }
    else {
      TimeoutNotify notify = UnknownTimeoutNotify;
      if (state() == ConnectingState) {
        notify = ConnectTimeoutNotify;
      }
      else if (state() == ConnectedState) {
        if (d->receivedHandShake == false) { notify = HandShakeTimeoutNotify; }
        else { notify = PeerTimeoutNotify; }
      }

      ILogger::Debug() << QString("%1 (%2).")
                          .arg(timeoutNotifyToString(notify))
                          .arg(notify);
      emit timeout(notify);
      abort();
    }
  }
  // Pending Request
  else if (event->timerId() == d->pendingRequestTimeoutTimer) {
    emit timeout(PendingRequestTimeoutNotify);
    abort();
  }
  // KeepAlive
  else if (event->timerId() == d->keepAliveTimer) {
    sendPacket_(KeepAlivePacket);
  }

  QTcpSocket::timerEvent(event);
}

void MCClientPeer::errorProcessingPacket_(MCClientPeer::PacketError error, ErrorBehavior errorBehavior) {
  Assert(errorBehavior != UnknownBehavior);

  PacketType packetType = static_cast<PacketType>(d->packetType);

  emit errorProcessingPacket(error, packetType, d->packetSize, errorBehavior);

  switch (errorBehavior) {
    // Packet dropped
    case DropPacketBehavior:
      (void) read(d->packetSize - PacketHeaderSize); // Drop packet data
      break;

    // Continue
    case ContinueBehavior:
      break;

    // Abort the connection
    default:
      abort();
  }
}

void MCClientPeer::sendHandShakePacket_() {
  d->sentHandShake = true;

  // Peer timeout
  if (d->timeoutTimer) {
    killTimer(d->timeoutTimer);
    d->timeoutTimer = 0;
  }
  if (d->timeout > 0) {
    d->timeoutTimer = startTimer(d->timeout);
  }


  MC_DATASTREAM_WRITE(buffer, bytes, out);

  // Protocol
  out.writeRawData(ProtocolId, ProtocolIdSize);
  out << ProtocolVersion;

  // Write protocol data
  write(bytes);
  write(QByteArray(8, '\0')); // Reserved characters
}

void MCClientPeer::sendAuthenticationPacket_() {
  CNetworkInfo networkInfo = CNetworkInfo::fromInterfaceByIp(localAddress());
  networkInfo.setPeerName(peerName());
  networkInfo.setPeerAddress(peerAddress());
  networkInfo.setPeerPort(peerPort());

  sendPacket_(AuthenticationPacket, networkInfo);
}

void MCClientPeer::sendConnectionRefusedPacket_(const QString& reason) {
  sendPacket_(ConnectionRefusedPacket, reason);
}

void MCClientPeer::sendRequestDeniedPacket_(PacketType requestPacketType) {
  quint16 u16PacketType = static_cast<quint16>(requestPacketType);

  Assert((u16PacketType >= RequestPacketsStart) && (u16PacketType <= RequestPacketsEnd));
  sendPacket_(RequestDeniedPacket, u16PacketType);
}

void MCClientPeer::sendServerInfoRequestPacket_() {
  sendPacket_(ServerInfoRequestPacket);
}

void MCClientPeer::sendSeedUrlRequestPacket_() {
  sendPacket_(SeedUrlRequestPacket);
}

void MCClientPeer::sendServerInfoResponsePacket_(const MCServerInfo& serverInfo) {
  sendPacket_(ServerInfoResponsePacket, serverInfo);
}

void MCClientPeer::sendSeedUrlResponsePacket_(const QString& url, quint32 depth) {
  MC_DATASTREAM_WRITE(buffer, bytes, out);
  out << url;
  out << depth;

  sendPacket_(SeedUrlResponsePacket, bytes);
}

void MCClientPeer::sendDataNodesMessagePacket_(int n, const QByteArray& nodes) {
  Q_UNUSED(n);

  sendPacket_(DataNodesMessagePacket, nodes);
}

void MCClientPeer::sendLinkNodesMessagePacket_(int n, const QByteArray& links) {
  Q_UNUSED(n);

  sendPacket_(LinkNodesMessagePacket, links);
}

CNetworkInfo MCClientPeer::processAuthenticationPacket_(QDataStream& data) {
  CNetworkInfo networkInfo;
  data >> networkInfo;

  return networkInfo;
}

void MCClientPeer::processConnectionRefusedPacket_(QDataStream& data) {
  QString reason;
  data >> reason;

  setSocketError(QTcpSocket::ConnectionRefusedError);
  setErrorString(QString("%1. (Connection refused)").arg(reason));

  emit error(QTcpSocket::ConnectionRefusedError);
}

void MCClientPeer::processRequestDeniedPacket_(QDataStream& data) {
  quint16 u16PacketType;
  data >> u16PacketType;

  // Invalid packet content
  if ((u16PacketType < RequestPacketsStart) || (u16PacketType > RequestPacketsEnd)) {
    errorProcessingPacket_(InvalidPacketContentError, ContinueBehavior);
    return;
  }

  PacketType packetType = static_cast<PacketType>(u16PacketType);
  MCClientPeerRequestInfo requestInfo = d->pendingRequests.takeRequest(
    packetType, MCClientPeerRequestsQueuesContainer::LastRequest
  );

  // Request don't exist
  if (requestInfo.isValid() == false) {
    errorProcessingPacket_(ResponseWithoutRequestError, ContinueBehavior);
    return;
  }

  emit requestDenied(packetType);
}

void MCClientPeer::processServerInfoRequestPacket_() {
  emit serverInfoRequest();
}

void MCClientPeer::processSeedUrlRequestPacket_() {
  emit seedUrlRequest();
}

void MCClientPeer::processServerInfoResponsePacket_(const MCClientPeerRequestInfo& requestInfo, QDataStream& data) {
  MCServerInfo serverInfo;
  data >> serverInfo;
  serverInfo.setPing(requestInfo.delay());
  emit serverInfoResponse(serverInfo);
}

void MCClientPeer::processSeedUrlResponsePacket_(const MCClientPeerRequestInfo& requestInfo, QDataStream& data) {
  Q_UNUSED(requestInfo);

  QString url;   data >> url;
  quint32 depth; data >> depth;

  emit seedUrlResponse(url, depth);
}

void MCClientPeer::processDataNodesMessagePacket_(QDataStream& data) {  
  QList<MCUrlInfo> nodes;

  while ((data.atEnd() == false) && (data.status() == QDataStream::Ok)) {
    QString url;   data >> url;
    quint32 depth; data >> depth;

    // Invalid url
    MCUrlInfo urlInfo(url, depth);
    if (urlInfo.isValid() == false) {
      errorProcessingPacket_(InvalidPacketContentError, ContinueBehavior);
    }

    nodes.append(urlInfo);
  }

  // Corrupted Data Packet
  if (data.status() != QDataStream::Ok) {
    errorProcessingPacket_(CorruptedDataPacketError, DropPacketBehavior);
    return;
  }

  emit dataNodesMessage(nodes);
}

void MCClientPeer::processLinkNodesMessagePacket_(QDataStream& data) {
  QList<QByteArray> hashChildren;

  QByteArray hashParent;
  data >> hashParent;

  while ((data.atEnd() == false) && (data.status() == QDataStream::Ok)) {
    QByteArray hashChild;
    data >> hashChild;
    hashChildren.append(hashChild);
  }

  // Corrupted Data Packet
  if (data.status() != QDataStream::Ok) {
    errorProcessingPacket_(CorruptedDataPacketError, DropPacketBehavior);
    return;
  }

  emit linkNodesMessage(hashParent, hashChildren);
}

void MCClientPeer::initConnection_() {
  d->invalidateTimeout = false;
  d->receivedHandShake = false;
  d->sentHandShake = false;
  d->packetType = 0;
  d->packetSize = 0;
  d->authenticated = false;
  d->networkInfo = CNetworkInfo();
}

void MCClientPeer::killAllTimers_() {
  if (d->timeoutTimer) {
    killTimer(d->timeoutTimer);
    d->timeoutTimer = 0;
  }

  if (d->pendingRequestTimeoutTimer) {
    killTimer(d->pendingRequestTimeoutTimer);
    d->pendingRequestTimeoutTimer = 0;
  }

  if (d->keepAliveTimer) {
    killTimer(d->keepAliveTimer);
    d->keepAliveTimer = 0;
  }
}

// Initialize all state variable
void MCClientPeer::connecting_() {
  d->invalidateTimeout = false;
  if (d->connectTimeout > 0) {
    d->timeoutTimer = startTimer(d->connectTimeout);
  }
}

void MCClientPeer::connected_() {
  initConnection_();

  if (d->handShakeTimeout > 0) {
    d->timeoutTimer = startTimer(d->handShakeTimeout);
  }
}

void MCClientPeer::closing_() {
  killAllTimers_();
}

// Clean all state variable
void MCClientPeer::disconnected_() {
  killAllTimers_();

  initConnection_();
}

MCClientPeerRequestInfo MCClientPeer::registerRequest_(PacketType requestPacketType) {
  Assert(
       (static_cast<int>(requestPacketType) >= MCClientPeer::RequestPacketsStart)
    && (static_cast<int>(requestPacketType) <= MCClientPeer::RequestPacketsEnd)
  );

  // Start Pending Request Timeout timer
  if (d->pendingRequestTimeoutTimer) {
    killTimer(d->pendingRequestTimeoutTimer);
    d->pendingRequestTimeoutTimer = 0;
  }
  if (d->pendingRequestTimeout > 0) {
    d->pendingRequestTimeoutTimer = startTimer(d->pendingRequestTimeout);
  }

  return d->pendingRequests.addRequest(requestPacketType);
}

MCClientPeerRequestInfo MCClientPeer::unregisterRequest_(PacketType requestPacketType) {
  Assert(
       (static_cast<int>(requestPacketType) >= MCClientPeer::RequestPacketsStart)
    && (static_cast<int>(requestPacketType) <= MCClientPeer::RequestPacketsEnd)
  );

  // Kill Pending Request Timeout timer
  if (d->pendingRequestTimeoutTimer) {
    killTimer(d->pendingRequestTimeoutTimer);
    d->pendingRequestTimeoutTimer = 0;
  }

  // Return the request proceeded
  MCClientPeerRequestInfo requestInfo = d->pendingRequests.takeRequest(
    requestPacketType, MCClientPeerRequestsQueuesContainer::FirstRequest
  );

  requestInfo.responseReceivedNotify();
  return requestInfo;
}

void MCClientPeer::sendPacket_(PacketType packetType, const QByteArray& data) {
  QByteArray bytesData;

  int nPacketType = static_cast<int>(packetType);

  // Compress data if necessary
  if ((nPacketType >= CompressedMessagePacketsStart) && (nPacketType <= CompressedMessagePacketsEnd)) {
    bytesData = /*qCompress(*/data/*)*/;
  }
  // Not compressed data
  else {
    bytesData = data;
  }

  quint32 packetSize = ((quint32)bytesData.size()) + PacketHeaderSize;

  ILogger::Debug() << QString("Sending the packet : %1 - %2 bytes (%3)...")
                      .arg(packetTypeToString(packetType))
                      .arg(packetSize)
                      .arg(packetType);

  // Request packet
  if ((nPacketType >= RequestPacketsStart) && (nPacketType <= RequestPacketsEnd)) {
    (void) registerRequest_(packetType);
  }

  MC_DATASTREAM_WRITE(buffer, bytesHeader, out);
  out << (quint32)packetSize; // Size of the packet
  out << (quint16)packetType; // Type of the packet

  // Send packet header
  write(bytesHeader);

  // Send packet data
  if (bytesData.isEmpty() == false) {
    write(bytesData);
  }

  emit packetSent(packetType, packetSize);
}

template <class T> void MCClientPeer::sendPacket_(PacketType packetType, const T& data) {
  MC_DATASTREAM_WRITE(buffer, bytes, out);

  out << data;
  sendPacket_(packetType, bytes);
}

void MCClientPeer::processPacket_(PacketType packetType, quint32 packetSize, const MCClientPeerRequestInfo& requestInfo) {
  ILogger::Debug() << QString("Processing the packet : %1 - %2 bytes (%3)...")
                      .arg(packetTypeToString(packetType))
                      .arg(packetSize)
                      .arg(packetType);

  // Extract packet data
  QByteArray rawBytes = read(packetSize - PacketHeaderSize);

  // Inconsistent size
  if ((quint32)rawBytes.size() != (packetSize - PacketHeaderSize)) {
    errorProcessingPacket_(PacketSizeError, DropPacketBehavior);
    return;
  }

  QByteArray bytes;

  // Uncompress if necessary
  int nPacketType = static_cast<int>(packetType);
  if ((nPacketType >= CompressedMessagePacketsStart) && (nPacketType <= CompressedMessagePacketsEnd)) {
    bytes = /*qUncompress(*/rawBytes/*)*/;
  }
  // Raw bytes
  else {
    bytes = rawBytes;
  }

  // Create a data stream to read packet content
  MC_DATASTREAM_READ(bytes, in);

  // Check authentication packet (received just after HandShake message)
  if ((d->requireAuthentication == true) && (d->authenticated == false)) {
    // Could not process the packet because no authentication message was received.
    if (packetType != AuthenticationPacket) {
      errorProcessingPacket_(AuthenticationError, AbortBehavior);
    }
    // Process the authentication packet
    else {
      d->networkInfo = processAuthenticationPacket_(in);
      d->authenticated = true;
      emit authenticated(d->networkInfo);
    }
    return;
  }

  // Packet type
  switch (packetType) {
    /** SYSTEM */
    // KeepAlive (demand)
    case KeepAlivePacket:
    {
      d->keepAliveTimer = startTimer(d->keepAliveInterval); // Restart KeepAlive timer
      sendPacket_(KeepAliveAcknowledgmentPacket);
    }
    break;

    // KeepAlive (acknowledgment)
    case KeepAliveAcknowledgmentPacket:
      break;

    // Connection Refused
    case ConnectionRefusedPacket:
      processConnectionRefusedPacket_(in);
      break;

    // Request Denied
    case RequestDeniedPacket:
      processRequestDeniedPacket_(in);
      break;

    /** REQUESTS */
    // Server Info Request
    case ServerInfoRequestPacket:
      processServerInfoRequestPacket_();
      break;
      
    // Seed Urls Request
    case SeedUrlRequestPacket:
      processSeedUrlRequestPacket_();
      break;

    /** RESPONSES */
    // Server Info Response
    case ServerInfoResponsePacket:
      processServerInfoResponsePacket_(requestInfo, in);
      break;

    case SeedUrlResponsePacket:
      processSeedUrlResponsePacket_(requestInfo, in);
      break;

    /** MESSAGES */
    case DataNodesMessagePacket:
      processDataNodesMessagePacket_(in);
      break;

    case LinkNodesMessagePacket:
      processLinkNodesMessagePacket_(in);
      break;

    default:
      errorProcessingPacket_(PacketTypeError, AbortBehavior);
  }
}

class MCClientPeerRequestInfoPrivate : public QSharedData
{
public:
    MCClientPeerRequestInfoPrivate();
    MCClientPeerRequestInfoPrivate(MCClientPeer::PacketType packetType);

    bool valid;
    MCClientPeer::PacketType requestPacketType;
    bool responseReceived;
    QTime beginTime;
    QTime endTime;
};

MCClientPeerRequestInfoPrivate::MCClientPeerRequestInfoPrivate()
  : valid(false),
    responseReceived(false)
{}

MCClientPeerRequestInfoPrivate::MCClientPeerRequestInfoPrivate(MCClientPeer::PacketType requestPacketType)
  : valid(true),
    requestPacketType(requestPacketType),
    responseReceived(false),
    beginTime(QTime::currentTime())
{
  Assert(
       (static_cast<int>(requestPacketType) >= MCClientPeer::RequestPacketsStart)
    && (static_cast<int>(requestPacketType) <= MCClientPeer::RequestPacketsEnd)
  );
}

MCClientPeerRequestInfo::MCClientPeerRequestInfo()
  : d(new MCClientPeerRequestInfoPrivate)
{}

MCClientPeerRequestInfo::MCClientPeerRequestInfo(MCClientPeer::PacketType packetType)
  : d(new MCClientPeerRequestInfoPrivate(packetType))
{}

MCClientPeerRequestInfo::MCClientPeerRequestInfo(const MCClientPeerRequestInfo &other)
  : d(other.d)
{}

MCClientPeerRequestInfo& MCClientPeerRequestInfo::operator=(const MCClientPeerRequestInfo& requestInfo) {
  d = requestInfo.d;
  return *this;
}

MCClientPeerRequestInfo::~MCClientPeerRequestInfo() {
  // QSharedDataPointer takes care of deleting for us
}

bool MCClientPeerRequestInfo::isValid() const {
  return d->valid;
}

MCClientPeer::PacketType MCClientPeerRequestInfo::requestPacketType() const {
  Assert(isValid() == true);

  return d->requestPacketType;
}

MCClientPeer::PacketType MCClientPeerRequestInfo::responsePacketType() const {
  Assert(isValid() == true);

  return static_cast<MCClientPeer::PacketType>(
    (static_cast<int>(d->requestPacketType) - MCClientPeer::RequestPacketsStart) + MCClientPeer::ResponsePacketsStart
  );
}

bool MCClientPeerRequestInfo::responseReceived() const {
  return d->responseReceived;
}

void MCClientPeerRequestInfo::responseReceivedNotify() {
  Assert(isValid() == true);

  d->responseReceived = true;
  d->endTime = QTime::currentTime();
}

QTime MCClientPeerRequestInfo::beginTime() const {
  return d->beginTime;
}

QTime MCClientPeerRequestInfo::endTime() const {
  return d->endTime;
}

int MCClientPeerRequestInfo::delay() const {
  if (d->responseReceived == false) {
    return -1;
  }

  return d->beginTime.msecsTo(d->endTime);
}

MCClientPeerRequestsQueuesContainer::MCClientPeerRequestsQueuesContainer()
{}

int MCClientPeerRequestsQueuesContainer::requestCount(MCClientPeer::PacketType requestPacketType) const {
  return m_requestsQueuesContainer.value(requestPacketType).count();
}

MCClientPeerRequestInfo MCClientPeerRequestsQueuesContainer::addRequest(MCClientPeer::PacketType requestPacketType) {
  Assert(
       (static_cast<int>(requestPacketType) >= MCClientPeer::RequestPacketsStart)
    && (static_cast<int>(requestPacketType) <= MCClientPeer::RequestPacketsEnd)
  );

  // Create request info
  MCClientPeerRequestInfo requestInfo(requestPacketType);

  RequestsQueuesContainer::Iterator it = m_requestsQueuesContainer.find(requestPacketType);

  // Complete the queue of requests
  if (it != m_requestsQueuesContainer.end()) {
    it.value().enqueue(requestInfo);
  }
  // Add a queue of requests
  else {
    RequestsQueue queue;
    queue.enqueue(requestInfo);
    m_requestsQueuesContainer.insert(requestPacketType, queue);
  }

  return requestInfo;
}

MCClientPeerRequestInfo MCClientPeerRequestsQueuesContainer::takeRequest(MCClientPeer::PacketType requestPacketType, RequestPosition requestPosition) {
  RequestsQueuesContainer::Iterator it = m_requestsQueuesContainer.find(requestPacketType);

  // Dequeues the request info
  if (it != m_requestsQueuesContainer.end()) {
    MCClientPeerRequestInfo requestInfo;

    // Dequeue
    if (requestPosition == FirstRequest) {
      requestInfo = it.value().dequeue();
    }
    // Pop
    else {
      requestInfo = it.value().takeLast();
    }

    // Remove the entry if the queue is empty
    if (it.value().count() == 0) {
      m_requestsQueuesContainer.remove(requestPacketType);
    }

    return requestInfo;
  }
  // Returns an invalid request info
  else {
    return MCClientPeerRequestInfo();
  }
}

QString MCClientPeerRequestsQueuesContainer::dumpPendingRequests() const {
  QString s("Pending Requests Dump : ");
  if (m_requestsQueuesContainer.isEmpty()) {
    s += "(empty)";
  }
  else {
    for (
      RequestsQueuesContainer::ConstIterator it = m_requestsQueuesContainer.begin();
      it != m_requestsQueuesContainer.end();
      ++it
    )
    {
       s += QString("\n%1 (%2) -> %3")
            .arg(MCClientPeer::packetTypeToString(it.key()))
            .arg(it.key())
            .arg(it.value().count());
    }
  }

  return s;
}
