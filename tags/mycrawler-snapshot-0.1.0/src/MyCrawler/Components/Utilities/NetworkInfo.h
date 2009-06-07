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

#include <QSharedData>
#include <QString>
#include <QHostAddress>
#include <QDataStream>

class CNetworkInfoPrivate;

class CNetworkInfo
{
private:
    void init_();
    void setFull_();

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

    CNetworkInfo(const CNetworkInfo &other);
    CNetworkInfo& operator=(const CNetworkInfo& networkInfo);
    ~CNetworkInfo();

public:
    bool isValid() const;

    QString peerName() const;
    void setPeerName(const QString& peerName);
    QHostAddress peerAddress() const;
    void setPeerAddress(const QHostAddress& peerAddress);
    quint16 peerPort() const;
    void setPeerPort(quint16 port);
    QString peerAddressAndPort() const { return QString("%1:%2").arg(peerAddress().toString()).arg(peerPort()); }

    QHostAddress ip() const;
    QHostAddress gateway() const;
    QHostAddress broadcast() const;
    QHostAddress netmask() const;
    int prefixLength() const;

    QString hostName() const;
    QString hostDomain() const;

    QString humanReadableName() const;
    quint64 hardwareAddress() const;
    QString hardwareAddressString() const;

public:
    void write(QDataStream& out) const;
    void read(QDataStream& in);

public:
    static CNetworkInfo fromInterfaceByIp(const QHostAddress& address);
    static quint64 fromHardwareAddressString(const QString& hardwareAddress, bool* ok = NULL);
    static QString hardwareAddressToString(quint64 hardwareAddress);
    static quint32 gatewayFromBroadcastAndNetmask(quint32 broadcast, quint32 netmask);

private:
    QSharedDataPointer<CNetworkInfoPrivate> d;
    friend class CNetworkInfoPrivate;
};

QDataStream& operator<<(QDataStream& out, const CNetworkInfo& networkInfo);
QDataStream& operator>>(QDataStream& in, CNetworkInfo& networkInfo);

#endif // NETWORKINFO_H
