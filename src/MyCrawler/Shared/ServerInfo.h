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

class QHostAddress;
class QString;

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

    bool isValid() const;

    QHostAddress ip() const { return m_ip; }
    void setIp(const QHostAddress& ip) { m_ip = ip; }
    quint16 port() const { return m_u16Port; }
    void setPort(quint16 port) { m_u16Port = port; }

    QString name() const { return m_sName; }
    void setName(const QString& name) { m_sName = name; }
    int ping() const { return m_nPing; }
    void setPing(int ping) { m_nPing = ping; }

    int users() const { return m_nUsers; }
    void setUsers(int users) { m_nUsers = users; }
    int maxUsers() const { return m_nMaxUsers; }
    void setMaxUsers(int maxUsers) { m_nMaxUsers = maxUsers; }

    Priority priority() const { return m_enumPriority; }
    void setPriority(Priority priority) { m_enumPriority = priority; }

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
    QHostAddress m_ip;
    quint16 m_u16Port;

    QString m_sName;
    int m_nPing;

    int m_nUsers;
    int m_nMaxUsers;

    Priority m_enumPriority;
};

QDataStream& operator<<(QDataStream& out, const MCServerInfo& serverInfo);
QDataStream& operator>>(QDataStream& in, MCServerInfo& serverInfo);

Q_DECLARE_METATYPE(MCServerInfo::Priority);

#endif // SERVERINFO_H
