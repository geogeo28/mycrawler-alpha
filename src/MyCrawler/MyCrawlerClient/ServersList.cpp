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

#include <QFile>

#include "Macros.h"
#include "ServersList.h"
#include "ServerInfo.h"

static const char* ServersListFileMagic = "MyCrawlerServersList";
static const quint16 ServersListFileMagicSize = 20;
static const quint16 ServersListFileVersion = 0x0100;

static MCServerInfo InvalidServerInfo = MCServerInfo();

MCServersList* MCServersList::s_instance = NULL;

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

  return it.value();
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

void MCServersList::removeAllServers() {
  m_lstServers.clear();
  emit allServersRemoved();
}

void MCServersList::updateServer(quint32 ip, const MCServerInfo& serverInfo) {
  if (ipExists(ip) == false) { return; }

  MCServerInfo& lServerInfo = fromIp(ip);
  lServerInfo.update(serverInfo);

  emit serverUpdated(lServerInfo);
}

QList<MCServerInfo> MCServersList::allServers() const {
  return m_lstServers.values();
}

QList<MCServerInfo> MCServersList::serversListSorted() const {
  QList<MCServerInfo> lstServers = m_lstServers.values();
  qSort(lstServers);
  return lstServers;
}

void MCServersList::write(QDataStream& out) const {
  out << m_lstServers;
}

void MCServersList::read(QDataStream& in) {
  // Warning : Data are not checked.
  in >> m_lstServers;
}

void MCServersList::save(const QString& fileName) const throw(CFileException) {
  QFile file;
  file.setFileName(fileName);
  bool succeed = file.open(QFile::WriteOnly);

  // Cannot open the file
  if (succeed == false) {
    ThrowFileAccessException(fileName, file.errorString());
  }


  MC_DATASTREAM_WRITE(data, out);

  out.writeRawData(ServersListFileMagic, ServersListFileMagicSize);
  out << ServersListFileVersion;
  this->write(out);

  file.write(data);
}

void MCServersList::load(const QString& fileName) throw(CFileException) {
  QFile file;
  file.setFileName(fileName);
  bool succeed = file.open(QFile::ReadOnly);

  // Cannot open the file
  if (succeed == false) {
    ThrowFileAccessException(fileName, file.errorString());
  }

  // Read magic
  QByteArray magic = file.read(ServersListFileMagicSize);
  if (magic.startsWith(ServersListFileMagic) == false) {
    ThrowFileFormatException(fileName, "Invalid magic descriptor.");
  }

  // Prepare to read content
  MC_DATASTREAM_READ(&file, in);

  // Check version
  quint16 version; in >> version;
  if (version != ServersListFileVersion) {
    ThrowFileFormatException(fileName, "Version not supported.");
    return;
  }

  this->read(in);
}
