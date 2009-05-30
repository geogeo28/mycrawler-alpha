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

#include <QFileInfo>
#include <QNetworkProxy>

#include "Config/Config.h"
#include "Config/Settings.h"
#include "Debug/Exception.h"
#include "Debug/Logger.h"

#include "ServerApplication.h"
#include "Server.h"
#include "ServerHistory.h"
#include "ServerMainWindow.h"

MCServerApplication* MCServerApplication::s_instance = NULL;

MCServerApplication* MCServerApplication::instance() {
  return s_instance;
}

void MCServerApplication::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCServerApplication::init_() {
  m_pMainWindow = new MCServerMainWindow();
  m_pUrlsInQueue = new MCUrlsCollection();
}

void MCServerApplication::cleanAll_() {
  MCServer::destroy();
  MCServerHistory::destroy();

  delete m_pMainWindow;
  delete m_pUrlsInQueue;

  cleanupResources();
}


MCServerApplication::MCServerApplication(int &argc, char** argv)
  : IApplication(argc, argv)
{ 
  Assert(s_instance == NULL);

  s_instance = this;

  // Set application information
  setInformation(_MYCRAWLER_APPNAME_" (server)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
  setApplicationVersion(_MYCRAWLER_SERVER_VERSION_);

  // Install base components
  installTranslator();
  installLoggers();

  // Install settings manager
  CSettings::setPath(CSettings::XmlFormat, CSettings::UserScope, applicationDirPath());
  CSettings::setMethodWriteValue(CSettings::NotEmptyValues);
  installSettings("settings");
  MCSettings->setLayoutPrefixKey("WidgetsLayouts");

  // Init resources
  initResources();

  init_();

  installLoggerMsgBox(mainWindow());
}

MCServerApplication::~MCServerApplication() {
  cleanAll_();
  ILogger::Debug() << "Destroyed.";
}

void MCServerApplication::initResources() {
  ILogger::Debug() << "Init resources.";

  Q_INIT_RESOURCE(widgets);
  Q_INIT_RESOURCE(shared);
  Q_INIT_RESOURCE(resources);
}

void MCServerApplication::cleanupResources() {
  ILogger::Debug() << "Clean-up resources.";

  Q_CLEANUP_RESOURCE(widgets);
  Q_CLEANUP_RESOURCE(shared);
  Q_CLEANUP_RESOURCE(resources);
}

void MCServerApplication::loadSettings() {
  loadSettingsServerConnection();
  loadSettingsProxyConfiguration();
  loadSettingsServerHistory();
}

void MCServerApplication::saveSettings() {
  saveSettingsServerHistory();
}

void MCServerApplication::loadServerHistory(const QString& fileName) {
  ILogger::Debug() << QString("Load the server history from the file '%1'.").arg(fileName);
  MCServerHistory::instance()->load(fileName);
}

void MCServerApplication::saveServerHistory(const QString& fileName) {
  ILogger::Debug() << QString("Save the server history in the file '%1'.").arg(fileName);
  MCServerHistory::instance()->save(fileName);
}

void MCServerApplication::loadSettingsServerConnection() {
  AssertCheckPtr(settings());

  ILogger::Debug() << "Load 'ServerConnectionConfiguration' settings.";
  settings()->beginGroup("ServerConnectionConfiguration");
    MCServer::instance()->setListenAddress(settings()->value("Address", MCSettingsApplication::DefaultServerAddress).toString());
    MCServer::instance()->setListenPort(settings()->value("Port", MCSettingsApplication::DefaultServerPort).toUInt());
    MCServer::instance()->setName(settings()->value("Name", MCSettingsApplication::DefaultServerName).toString());
    MCServer::instance()->setMaxConnections(settings()->value("MaxConnections", MCSettingsApplication::DefaultServerMaxConnections).toInt());
  settings()->endGroup();
}

void MCServerApplication::saveSettingsServerConnection(
  const QString& address, quint16 port,
  const QString& name,
  int maxConnections
)
{
  AssertCheckPtr(settings());

  ILogger::Debug() << "Save 'ServerConnectionConfiguration' settings.";
  settings()->beginGroup("ServerConnectionConfiguration");
    settings()->setValue("Address", address);
    settings()->setValue("Port", port);
    settings()->setValue("Name", name);
    settings()->setValue("MaxConnections", maxConnections);
  settings()->endGroup();
}

void MCServerApplication::loadSettingsProxyConfiguration() {
  AssertCheckPtr(settings());

  ILogger::Debug() << "Load 'ProxyConfiguration' settings.";
  QNetworkProxy proxy(QNetworkProxy::NoProxy);
  settings()->beginGroup("ProxyConfiguration");
    proxy.setHostName(settings()->value("HostName", MCSettingsApplication::DefaultProxyHostName).toString());
    proxy.setPort(settings()->value("Port", MCSettingsApplication::DefaultProxyPort).toInt());
    proxy.setUser(settings()->value("UserName", MCSettingsApplication::DefaultProxyUserName).toString());
    proxy.setPassword(QByteArray::fromBase64(settings()->value("UserPassword", MCSettingsApplication::DefaultProxyUserPassword).toByteArray()));

    if (settings()->value("Use", MCSettingsApplication::DefaultProxyUse).toBool() == true) {
      proxy.setType(QNetworkProxy::HttpProxy);
    }
  settings()->endGroup();
  IApplication::setProxy(proxy);
}

void MCServerApplication::saveSettingsProxyConfiguration(
  bool useProxy,
  const QString& hostName, quint16 port,
  const QString& userName, const QString& password
)
{
  AssertCheckPtr(settings());

  ILogger::Debug() << "Save 'ProxyConfiguration' settings.";
  settings()->beginGroup("ProxyConfiguration");
    settings()->setValue("Use", useProxy);
    settings()->setValue("HostName", hostName);
    settings()->setValue("Port", port);
    settings()->setValue("UserName", userName);
    settings()->setValue("UserPassword", password.toUtf8().toBase64());
  settings()->endGroup();
}

void MCServerApplication::loadSettingsServerHistory() {
  AssertCheckPtr(settings());

  if (settings()->value("AdvancedOptions/ServerSaveHistory", MCSettingsApplication::DefaultServerSaveHistory).toBool() == true) {
    // Load server history
    QFileInfo file(MCSettingsApplication::ServerHistoryFileName);
    if (file.exists()) {
      loadServerHistory(MCSettingsApplication::ServerHistoryFileName);
    }
  }
}

void MCServerApplication::saveSettingsServerHistory() {
  AssertCheckPtr(settings());

  if (settings()->value("AdvancedOptions/ServerSaveHistory", MCSettingsApplication::DefaultServerSaveHistory).toBool() == true) {
    saveServerHistory(MCSettingsApplication::ServerHistoryFileName);
  }
}

void MCServerApplication::run() {
  ILogger::Debug() << "Running...";

  mainWindow()->setup();
  mainWindow()->show();
}
