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

#include <QStringList>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>

#include "NetworkInfo.h"

CNetworkInfo::CNetworkInfo()
  : m_bValid(false),
    m_u16PeerPort(0),
    m_nPrefixLength(-1),
    m_u64HardwareAddress(0x0)
{}

CNetworkInfo::CNetworkInfo(
  const QString& peerName, const QHostAddress& peerAddress, quint16 peerPort,
  const QHostAddress& ip, const QHostAddress& broadcast, const QHostAddress& netmask, int prefixLength,
  const QString& humanReadableName, quint64 hardwareAddress,
  const QString& hostName, const QString& hostDomain
)
  : m_bValid(true),
    m_sPeerName(peerName), m_peerAddress(peerAddress), m_u16PeerPort(peerPort),
    m_ip(ip), m_broadcast(broadcast), m_netmask(netmask), m_nPrefixLength(prefixLength),
    m_sHumanReadableName(humanReadableName), m_u64HardwareAddress(hardwareAddress)
{
  m_sHostName = (hostName.isNull())?
                QHostInfo::localHostName():
                hostName;

  m_sHostDomain = (hostDomain.isNull())?
                  QHostInfo::localDomainName():
                  hostDomain;

  m_sHardwareAddress = CNetworkInfo::hardwareAddressToString(hardwareAddress);
}

CNetworkInfo::CNetworkInfo(
  const QHostAddress& ip, const QHostAddress& broadcast, const QHostAddress& netmask, int prefixLength,
  const QString& humanReadableName, quint64 hardwareAddress,
  const QString& hostName, const QString& hostDomain
)
  : m_bValid(true),
    m_u16PeerPort(0),
    m_ip(ip), m_broadcast(broadcast), m_netmask(netmask), m_nPrefixLength(prefixLength),
    m_sHumanReadableName(humanReadableName), m_u64HardwareAddress(hardwareAddress)
{
  m_sHostName = (hostName.isNull())?
                QHostInfo::localHostName():
                hostName;

  m_sHostDomain = (hostDomain.isNull())?
                  QHostInfo::localDomainName():
                  hostDomain;

  m_sHardwareAddress = CNetworkInfo::hardwareAddressToString(hardwareAddress);
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
  in >> m_sPeerName;
  in >> m_peerAddress;
  in >> m_u16PeerPort;

  in >> m_ip;
  in >> m_broadcast;
  in >> m_netmask;
  in >> m_nPrefixLength;

  in >> m_sHostName;
  in >> m_sHostDomain;

  in >> m_sHumanReadableName;
  in >> m_u64HardwareAddress;

  m_sHardwareAddress = CNetworkInfo::hardwareAddressToString(m_u64HardwareAddress);
  m_bValid = true;
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
  quint64 macAdr = 0x00;
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
  QString s;
  for (int  i = 0; i < 8; ++i) {
    s += QString::number((int)((hardwareAddress & 0xFF00000000000000ULL) >> 48)).toUpper();
    hardwareAddress <<= 8;
    if (i != 7) { s += ":"; }
  }

  return s;
}

QDataStream& operator<<(QDataStream& out, const CNetworkInfo& networkInfo) {
  networkInfo.write(out);
  return out;
}

QDataStream& operator>>(QDataStream& in, CNetworkInfo& networkInfo) {
  networkInfo.read(in);
  return in;
}
