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

#include <cstdio>

#include <QStringList>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>

#include "NetworkInfo.h"

class CNetworkInfoPrivate : public QSharedData
{
  public:
    CNetworkInfoPrivate();

    bool valid;

    QString peerName;
    QHostAddress peerAddress;
    quint16 peerPort;

    QHostAddress ip;
    QHostAddress gateway;
    QHostAddress broadcast;
    QHostAddress netmask;
    int prefixLength;

    QString humanReadableName;
    quint64 hardwareAddress;
    QString hardwareAddressString;

    QString hostName;
    QString hostDomain;
};

CNetworkInfoPrivate::CNetworkInfoPrivate()
  : valid(false),
    peerPort(0), prefixLength(-1), hardwareAddress(0x0)
{}


void CNetworkInfo::init_() {
  if (d->hostName.isNull())   { d->hostName = QHostInfo::localHostName(); }
  if (d->hostDomain.isNull()) { d->hostDomain = QHostInfo::localDomainName(); }

  setFull_();
}

void CNetworkInfo::setFull_() {
  quint32 gateway = CNetworkInfo::gatewayFromBroadcastAndNetmask(broadcast().toIPv4Address(), netmask().toIPv4Address());
  if (gateway != 0x00) { d->gateway = QHostAddress(gateway); }
  else { d->gateway = QHostAddress(); }

  d->hardwareAddressString = CNetworkInfo::hardwareAddressToString(d->hardwareAddress);
}

CNetworkInfo::CNetworkInfo()
  : d(new CNetworkInfoPrivate)
{}

CNetworkInfo::CNetworkInfo(
  const QString& peerName, const QHostAddress& peerAddress, quint16 peerPort,
  const QHostAddress& ip, const QHostAddress& broadcast, const QHostAddress& netmask, int prefixLength,
  const QString& humanReadableName, quint64 hardwareAddress,
  const QString& hostName, const QString& hostDomain
)
  : d(new CNetworkInfoPrivate)
{
  d->valid = true;

  d->peerName = peerName;
  d->peerAddress = peerAddress;
  d->peerPort = peerPort;

  d->ip = ip;
  d->broadcast = broadcast;
  d->netmask = netmask;
  d->prefixLength = prefixLength;

  d->humanReadableName = humanReadableName;
  d->hardwareAddress = hardwareAddress;

  d->hostName = hostName;
  d->hostDomain = hostDomain;

  init_();
}

CNetworkInfo::CNetworkInfo(
  const QHostAddress& ip, const QHostAddress& broadcast, const QHostAddress& netmask, int prefixLength,
  const QString& humanReadableName, quint64 hardwareAddress,
  const QString& hostName, const QString& hostDomain
)
  : d(new CNetworkInfoPrivate)
{
  d->valid = true;

  d->ip = ip;
  d->broadcast = broadcast;
  d->netmask = netmask;
  d->prefixLength = prefixLength;

  d->humanReadableName = humanReadableName;
  d->hardwareAddress = hardwareAddress;

  d->hostName = hostName;
  d->hostDomain = hostDomain;

  init_();
}

CNetworkInfo::CNetworkInfo(const CNetworkInfo &other)
  : d(other.d)
{}

CNetworkInfo& CNetworkInfo::operator=(const CNetworkInfo& networkInfo) {
  d = networkInfo.d;
  return *this;
}

CNetworkInfo::~CNetworkInfo()
{
  // QSharedDataPointer takes care of deleting for us
}

bool CNetworkInfo::isValid() const {
  return d->valid;
}

QString CNetworkInfo::peerName() const {
  return d->peerName;
}

void CNetworkInfo::setPeerName(const QString& peerName) {
  d->peerName = peerName;
}

QHostAddress CNetworkInfo::peerAddress() const {
  return d->peerAddress;
}

void CNetworkInfo::setPeerAddress(const QHostAddress& peerAddress) {
  d->peerAddress = peerAddress;
}

quint16 CNetworkInfo::peerPort() const {
  return d->peerPort;
}

void CNetworkInfo::setPeerPort(quint16 port) {
  d->peerPort = port;
}

QHostAddress CNetworkInfo::ip() const {
  return d->ip;
}

QHostAddress CNetworkInfo::gateway() const {
  return d->gateway;
}

QHostAddress CNetworkInfo::broadcast() const {
  return d->broadcast;
}

QHostAddress CNetworkInfo::netmask() const {
  return d->netmask;
}

int CNetworkInfo::prefixLength() const {
  return d->prefixLength;
}

QString CNetworkInfo::hostName() const {
  return d->hostName;
}

QString CNetworkInfo::hostDomain() const {
  return d->hostDomain;
}

QString CNetworkInfo::humanReadableName() const {
  return d->humanReadableName;
}

quint64 CNetworkInfo::hardwareAddress() const {
  return d->hardwareAddress;
}

QString CNetworkInfo::hardwareAddressString() const {
  return d->hardwareAddressString;
}

void CNetworkInfo::write(QDataStream& out) const {
  out << peerName();
  out << peerAddress();
  out << peerPort();

  out << ip();
  out << broadcast();
  out << netmask();
  out << prefixLength();

  out << hostName();
  out << hostDomain();

  out << humanReadableName();
  out << hardwareAddress();
}

void CNetworkInfo::read(QDataStream& in) {
  in >> d->peerName;
  in >> d->peerAddress;
  in >> d->peerPort;

  in >> d->ip;
  in >> d->broadcast;
  in >> d->netmask;
  in >> d->prefixLength;

  in >> d->hostName;
  in >> d->hostDomain;

  in >> d->humanReadableName;
  in >> d->hardwareAddress;

  d->valid = true;

  setFull_();
}

CNetworkInfo CNetworkInfo::fromInterfaceByIp(const QHostAddress& address) {
  foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
    foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
      if (entry.ip() == address) {
        return CNetworkInfo(
          address, entry.broadcast(), entry.netmask(), entry.prefixLength(),
          interface.humanReadableName(),
          CNetworkInfo::fromHardwareAddressString(interface.hardwareAddress())
        );
      }
    }
  }

  return CNetworkInfo();
}

quint64 CNetworkInfo::fromHardwareAddressString(const QString& hardwareAddress, bool* ok) {
  quint64 macAdr = 0x0;
  QStringList lstDigits = hardwareAddress.split(":");
  foreach (const QString& sDigit, lstDigits) {
    bool r;
    quint8 d = sDigit.toUInt(&r, 16);
    if (r == false) {
      if (ok != NULL) { *ok = false; }
      return 0x00;
    }

    macAdr = (macAdr << 8) | d;
  }

  if (ok != NULL) { *ok = true; }

  return macAdr;
}

QString CNetworkInfo::hardwareAddressToString(quint64 hardwareAddress) {
  static char buffer[2 + 1];

  QString s;
  for (int  i = 0; i < 8; ++i) {
    sprintf(buffer, "%02X", (int)((hardwareAddress & 0xFF00000000000000ULL) >> 56));
    s += QString::fromAscii(buffer);
    hardwareAddress <<= 8;
    if (i != 7) { s += ":"; }
  }

  return s;
}

quint32 CNetworkInfo::gatewayFromBroadcastAndNetmask(quint32 broadcast, quint32 netmask) {
  // Set 1 were the netmask contains 0
  for (int i = 0; i < 4; ++i) {
    int left = (i * 8);
    if ((netmask & (0xFF << left)) == 0) {
      netmask |= (1 << left);
    }
  }

  return (broadcast & netmask);
}

QDataStream& operator<<(QDataStream& out, const CNetworkInfo& networkInfo) {
  networkInfo.write(out);
  return out;
}

QDataStream& operator>>(QDataStream& in, CNetworkInfo& networkInfo) {
  networkInfo.read(in);
  return in;
}
