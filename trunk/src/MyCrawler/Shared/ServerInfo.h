/****************************************************************************
 * @(#) Server Information class.
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

#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QSharedData>
#include <QHostAddress>
#include <QMetaType>

class QString;

class MCServerInfoPrivate;

class MCServerInfo
{
public:
    typedef enum {
      LowPriority,
      NormalPriority,
      HighPriority
    } Priority;

public:
    MCServerInfo();
    MCServerInfo(
      const QHostAddress& ip, quint16 port,
      const QString& name = QString(), int ping = -1,
      int users = -1, int maxUsers = -1,
      Priority priority = NormalPriority
    );

    MCServerInfo(const MCServerInfo &other);
    MCServerInfo& operator=(const MCServerInfo& serverInfo);
    ~MCServerInfo();

    bool isValid() const;

    QHostAddress ip() const;
    void setIp(const QHostAddress& ip);
    quint16 port() const;
    void setPort(quint16 port);

    QString name() const;
    void setName(const QString& name);
    int ping() const;
    void setPing(int ping);

    int users() const;
    void setUsers(int users);
    int maxUsers() const;
    void setMaxUsers(int maxUsers);

    Priority priority() const;
    void setPriority(Priority priority);

    QString ipAndPortString() const { return QString("%1:%2").arg(ip().toString()).arg(port()); }

    void update(const MCServerInfo& serverInfo);

public:
    void write(QDataStream& out) const;
    void read(QDataStream& in);

public:
    bool operator<(const MCServerInfo& serverInfo) const;

public:
    static QString priorityToString(Priority priority);

private:
    QSharedDataPointer<MCServerInfoPrivate> d;
    friend class MCServerInfoPrivate;
};

QDataStream& operator<<(QDataStream& out, const MCServerInfo& serverInfo);
QDataStream& operator>>(QDataStream& in, MCServerInfo& serverInfo);

Q_DECLARE_METATYPE(MCServerInfo::Priority);

#endif // SERVERINFO_H
