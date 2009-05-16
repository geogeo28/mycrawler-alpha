/****************************************************************************
 * @(#) Network information.
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

#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <QString>
#include <QHostAddress>
#include <QDataStream>

class CNetworkInfo
{
public:
    CNetworkInfo();
    CNetworkInfo(
      const QString& peerName, const QHostAddress& peerAddress, quint16 peerPort,
      const QHostAddress& ip, const QHostAddress& broadcast, const QHostAddress& netmask, int prefixLength,
      const QString& humanReadableName = QString(), quint64 hardwareAddress = 0x0,
      const QString& hostName = QString::null, const QString& hostDomain = QString::null
    );

    CNetworkInfo(
      const QHostAddress& ip, const QHostAddress& broadcast, const QHostAddress& netmask, int prefixLength,
      const QString& humanReadableName = QString(), quint64 hardwareAddress = 0x00,
      const QString& hostName = QString::null, const QString& hostDomain = QString::null
    );

    bool isValid() const { return m_bValid; }

    QString peerName() const { return m_sPeerName; }
    void setPeerName(const QString& peerName) { m_sPeerName = peerName; }
    QHostAddress peerAddress() const { return m_peerAddress; }
    void setPeerAddress(const QHostAddress& peerAddress) { m_peerAddress = peerAddress; }
    quint16 peerPort() const { return m_u16PeerPort; }
    void setPeerPort(quint16 port) { m_u16PeerPort = port; }
    QString peerAddressAndPort() const { return QString("%1:%2").arg(peerAddress().toString()).arg(peerPort()); }

    QHostAddress ip() const { return m_ip; }
    QHostAddress broadcast() const { return m_broadcast; }
    QHostAddress netmask() const { return m_netmask; }
    int prefixLength() const { return m_nPrefixLength; }

    QString hostName() const { return m_sHostName; }
    QString hostDomain() const { return m_sHostDomain; }

    QString humanReadableName() const { return m_sHumanReadableName; }
    quint64 hardwareAddress() const { return m_u64HardwareAddress; }
    QString hardwareAddressString() const {return m_sHardwareAddress; }

public:
    void write(QDataStream& out) const;
    void read(QDataStream& in);

public:
    static CNetworkInfo fromInterfaceByIp(const QHostAddress& address);
    static quint64 fromHardwareAddressString(const QString& hardwareAddress, bool* ok = NULL);
    static QString hardwareAddressToString(quint64 hardwareAddress);

private:
    bool m_bValid;

    QString m_sPeerName;
    QHostAddress m_peerAddress;
    quint16 m_u16PeerPort;

    QHostAddress m_ip;
    QHostAddress m_broadcast;
    QHostAddress m_netmask;
    int m_nPrefixLength;

    QString m_sHumanReadableName;
    quint64 m_u64HardwareAddress;
    QString m_sHardwareAddress;

    QString m_sHostName;
    QString m_sHostDomain;
};

QDataStream& operator<<(QDataStream& out, const CNetworkInfo& networkInfo);
QDataStream& operator>>(QDataStream& in, CNetworkInfo& networkInfo);

#endif // NETWORKINFO_H
