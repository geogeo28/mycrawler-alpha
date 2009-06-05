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
#include <QByteArray>
#include <QDataStream>

#include "Debug/Exception.h"

#include "Macros.h"
#include "ServerHistory.h"
#include "ClientThread.h"

static const char* HistoryFileMagic = "MyCrawlerServerHistory";
static const quint16 HistoryFileMagicSize = 22;
static const quint16 HistoryFileVersion = 0x0100;

MCServerHistory* MCServerHistory::s_instance = NULL;

MCServerHistory* MCServerHistory::instance() {
  if (s_instance == NULL) {
    s_instance = new MCServerHistory();
  }

  return s_instance;
}

void MCServerHistory::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCServerHistory::init_() {

}

void MCServerHistory::cleanAll_() {

}

MCServerHistory::MCServerHistory()
{
  init_();
}

MCServerHistory::~MCServerHistory() {
  cleanAll_();
}

bool MCServerHistory::isHardwareAddressRegistered(quint64 hardwareAddress) const {
  return m_lstClients.contains(hardwareAddress);
}

void MCServerHistory::addClient(const CNetworkInfo& networkInfo) {  
  quint64 hardwareAddress = networkInfo.hardwareAddress();

  Assert(hardwareAddress != 0x0);
  Assert(isHardwareAddressRegistered(hardwareAddress) == false);

  m_lstClients.insert(hardwareAddress, networkInfo);
}

void MCServerHistory::addClient(MCClientThread* client) {
  AssertCheckPtr(client);

  CNetworkInfo networkInfo(client->networkInfo());
  networkInfo.setPeerName(client->peerName());
  networkInfo.setPeerAddress(client->peerAddress());
  networkInfo.setPeerPort(client->peerPort());

  addClient(networkInfo);
}

bool MCServerHistory::removeClient(quint64 hardwareAddress) {
  Assert(hardwareAddress != 0x0);
  Assert(isHardwareAddressRegistered(hardwareAddress) == true);

  return (m_lstClients.remove(hardwareAddress) == 1);
}

bool MCServerHistory::removeClient(MCClientThread* client) {
  AssertCheckPtr(client);

  return removeClient(client->networkInfo().hardwareAddress());
}

QList<CNetworkInfo> MCServerHistory::allClients() const {
  return m_lstClients.values();
}

void MCServerHistory::write(QDataStream& out) const {
  out << m_lstClients;
}

void MCServerHistory::read(QDataStream& in) {
  // Warning : Data are not checked
  in >> m_lstClients;
}

void MCServerHistory::save(const QString& fileName) const throw(CFileException) {
  QFile file;
  file.setFileName(fileName);
  bool succeed = file.open(QFile::WriteOnly);

  // Cannot open the file
  if (succeed == false) {
    ThrowFileAccessException(fileName, file.errorString());
  }

  MC_DATASTREAM_WRITE(buffer, bytes, out);

  out.writeRawData(HistoryFileMagic, HistoryFileMagicSize);
  out << HistoryFileVersion;
  this->write(out);

  file.write(bytes);
}

void MCServerHistory::load(const QString& fileName) throw(CFileException) {
  QFile file;
  file.setFileName(fileName);
  bool succeed = file.open(QFile::ReadOnly);

  // Cannot open the file
  if (succeed == false) {
    ThrowFileAccessException(fileName, file.errorString());
  }

  // Read magic
  QByteArray magic = file.read(HistoryFileMagicSize);
  if (magic.startsWith(HistoryFileMagic) == false) {
    ThrowFileFormatException(fileName, "Invalid magic descriptor.");
  }

  // Prepare to read content
  MC_DATASTREAM_READ(&file, in);

  // Check version
  quint16 version; in >> version;
  if (version != HistoryFileVersion) {
    ThrowFileFormatException(fileName, "Version not supported.");
    return;
  }

  this->read(in);
}

QDataStream& operator<<(QDataStream& out, const MCServerHistory& history) {
  history.write(out);
  return out;
}

QDataStream& operator>>(QDataStream& in, MCServerHistory& history) {
  history.read(in);
  return in;
}
