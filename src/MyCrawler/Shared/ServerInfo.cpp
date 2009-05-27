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

#include <QHostAddress>
#include <QString>

#include "ServerInfo.h"

MCServerInfo::MCServerInfo()
 : m_u16Port(0),
   m_nPing(-1),
   m_nUsers(-1), m_nMaxUsers(-1),
   m_enumPriority(NormalPriority)
{}

MCServerInfo::MCServerInfo(
  const QHostAddress& ip, quint16 port,
  const QString& name, int ping,
  int users, int maxUsers,
  Priority priority
)
 : m_ip(ip), m_u16Port(port),
   m_sName(name), m_nPing(ping),
   m_nUsers(users), m_nMaxUsers(maxUsers),
   m_enumPriority(priority)
{
  if (m_sName.trimmed().isEmpty()) { m_sName = m_ip.toString(); }
}

bool MCServerInfo::isValid() const {
  return (
       ((m_ip != QHostAddress::Null) && (m_ip != QHostAddress::Any))
    && (m_u16Port != 0)
  );
}

void MCServerInfo::update(const MCServerInfo& serverInfo) {
  // Set server name only if the value isn't empty
  if (serverInfo.name().trimmed().isEmpty() == false) {
    setName(serverInfo.name());
  }
  setPort(serverInfo.port());
  setPing(serverInfo.ping());
  setUsers(serverInfo.users());
  setMaxUsers(serverInfo.maxUsers());
}

void MCServerInfo::write(QDataStream& out) const {
  out << ip().toIPv4Address();
  out << port();

  out << name();

  out << users();
  out << maxUsers();
}

void MCServerInfo::read(QDataStream& in) {
  quint32 ip;

  in >> ip;
  in >> m_u16Port;

  in >> m_sName;
  in >> m_nUsers;
  in >> m_nMaxUsers;

  m_ip = QHostAddress(ip);
}

bool MCServerInfo::operator<(const MCServerInfo& serverInfo) const {
  return (this->priority() > serverInfo.priority());
}

QString MCServerInfo::priorityToString(Priority priority) {
  switch (priority) {
    case LowPriority:    return QT_TRANSLATE_NOOP(MCServerInfo, "Low");
    case NormalPriority: return QT_TRANSLATE_NOOP(MCServerInfo, "Normal");
    case HighPriority:   return QT_TRANSLATE_NOOP(MCServerInfo, "High");

    default:
      return QT_TRANSLATE_NOOP(MCServerInfo, "Unknown");
  }
}

QDataStream& operator<<(QDataStream& out, const MCServerInfo& serverInfo) {
  serverInfo.write(out);
  return out;
}

QDataStream& operator>>(QDataStream& in, MCServerInfo& serverInfo) {
  serverInfo.read(in);
  return in;
}
