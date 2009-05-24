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

#include "Debug/Exception.h"

#include "ServersList.h"

static MCServerInfo InvalidServerInfo = MCServerInfo();

MCServersList* MCServersList::s_instance = NULL;

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
  if (m_sName.isEmpty()) { m_sName = m_ip.toString(); }
}

bool MCServerInfo::isValid() const {
  return (
    (m_ip != QHostAddress::Null) && (m_ip != QHostAddress::Any))
    && (m_u16Port != 0);
}

bool MCServerInfo::operator<(const MCServerInfo& serverInfo) const {
  return (this->priority() >= serverInfo.priority());
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

MCServersList* MCServersList::instance() {
  if (s_instance == NULL) {
    s_instance = new MCServersList();
  }

  return s_instance;
}

void MCServersList::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCServersList::init_() {

}

void MCServersList::cleanAll_() {

}

MCServersList::MCServersList(QObject* parent)
  : QObject(parent)
{
  init_();
}

MCServersList::~MCServersList() {
  cleanAll_();
}

bool MCServersList::ipExists(quint32 ip) const {
  return m_lstServers.contains(ip);
}

MCServerInfo& MCServersList::fromIp(quint32 ip) {
  ServersList::Iterator it = m_lstServers.find(ip);
  if (it == m_lstServers.end()) {
    return InvalidServerInfo;
  }

  return *it;
}

void MCServersList::addServer(const MCServerInfo& serverInfo) {
  Assert(serverInfo.isValid());

  m_lstServers.insert(serverInfo.ip().toIPv4Address(), serverInfo);
  emit serverAdded(serverInfo);
}

bool MCServersList::removeServer(quint32 ip) {
  bool success = (m_lstServers.remove(ip) == 1);
  if (success == true) {
    emit serverRemoved(ip);
  }

  return success;
}

QList<MCServerInfo> MCServersList::serversListSorted() const {
  QList<MCServerInfo> lstServers = m_lstServers.values();
  qSort(lstServers);
  return lstServers;
}
