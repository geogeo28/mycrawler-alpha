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

class MCServerInfoPrivate : public QSharedData
{
  public:
    MCServerInfoPrivate();

    QHostAddress ip;
    quint16 port;

    QString name;
    int ping;

    int users;
    int maxUsers;

    MCServerInfo::Priority priority;
};

MCServerInfoPrivate::MCServerInfoPrivate()
  : port(0), ping(-1),
    users(-1), maxUsers(-1),
    priority(MCServerInfo::NormalPriority)
{}

MCServerInfo::MCServerInfo()
  : d(new MCServerInfoPrivate)
{}

MCServerInfo::MCServerInfo(
  const QHostAddress& ip, quint16 port,
  const QString& name, int ping,
  int users, int maxUsers,
  Priority priority
)
 : d(new MCServerInfoPrivate)
{
  d->ip = ip;
  d->port = port;

  // If name is an empty value, set with ip address
  if (name.trimmed().isEmpty()) {
    d->name = ip.toString();
  }
  // Set with name string
  else {
    d->name = name;
  }

  d->ping = ping;

  d->users = users;
  d->maxUsers = maxUsers;

  d->priority = priority;
}

MCServerInfo::MCServerInfo(const MCServerInfo &other)
  : d(other.d)
{}

MCServerInfo& MCServerInfo::operator=(const MCServerInfo& serverInfo) {
  d = serverInfo.d;
  return *this;
}

MCServerInfo::~MCServerInfo()
{
  // QSharedDataPointer takes care of deleting for us
}

bool MCServerInfo::isValid() const {
  return (
       ((d->ip != QHostAddress::Null) && (d->ip != QHostAddress::Any))
    && (d->port != 0)
  );
}

QHostAddress MCServerInfo::ip() const {
  return d->ip;
}

void MCServerInfo::setIp(const QHostAddress& ip) {
  d->ip = ip;
}

quint16 MCServerInfo::port() const {
  return d->port;
}

void MCServerInfo::setPort(quint16 port) {
  d->port = port;
}

QString MCServerInfo::name() const {
  return d->name;
}

void MCServerInfo::setName(const QString& name) {
  d->name = name;
}

int MCServerInfo::ping() const {
  return d->ping;
}

void MCServerInfo::setPing(int ping) {
  d->ping = ping;
}

int MCServerInfo::users() const {
  return d->users;
}

void MCServerInfo::setUsers(int users) {
  d->users = users;
}

int MCServerInfo::maxUsers() const {
  return d->maxUsers;
}

void MCServerInfo::setMaxUsers(int maxUsers) {
  d->maxUsers = maxUsers;
}

MCServerInfo::Priority MCServerInfo::priority() const {
  return d->priority;
}

void MCServerInfo::setPriority(Priority priority) {
  d->priority = priority;
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

  out << ping();

  out << users();
  out << maxUsers();

  out << priority();
}

void MCServerInfo::read(QDataStream& in) {
  quint32 ip;
  int priority;

  in >> ip;
  in >> d->port;

  in >> d->name;

  in >> d->ping;

  in >> d->users;
  in >> d->maxUsers;

  in >> priority;

  d->ip = QHostAddress(ip);
  d->priority = static_cast<Priority>(priority);
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
