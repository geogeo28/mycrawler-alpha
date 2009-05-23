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

#include <QNetworkProxy>

#include "Config/Config.h"
#include "Config/Settings.h"
#include "Debug/Exception.h"
#include "Debug/Logger.h"

#include "ClientApplication.h"
#include "Client.h"
#include "ClientMainWindow.h"

MCClientApplication* MCClientApplication::s_instance = NULL;

MCClientApplication* MCClientApplication::instance() {
  return s_instance;
}

void MCClientApplication::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCClientApplication::init_() {
  m_pMainWindow = new MCClientMainWindow();
}

void MCClientApplication::cleanAll_() {
  MCClient::destroy();
  if (m_pMainWindow) { delete m_pMainWindow; }

  cleanupResources();
}

MCClientApplication::MCClientApplication(int &argc, char** argv)
  : IApplication(argc, argv)
{ 
  Assert(s_instance == NULL);

  s_instance = this;

  // Set application information
  setInformation(_MYCRAWLER_APPNAME_" (client)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
  setApplicationVersion(_MYCRAWLER_CLIENT_VERSION_);

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

MCClientApplication::~MCClientApplication() {
  cleanAll_();
  ILogger::Debug() << "Destroyed.";
}

void MCClientApplication::initResources() {
  ILogger::Debug() << "Init resources.";

  Q_INIT_RESOURCE(widgets);
  Q_INIT_RESOURCE(shared);
  Q_INIT_RESOURCE(resources);
}

void MCClientApplication::cleanupResources() {
  ILogger::Debug() << "Clean-up resources.";

  Q_CLEANUP_RESOURCE(widgets);
  Q_CLEANUP_RESOURCE(shared);
  Q_CLEANUP_RESOURCE(resources);
}

void MCClientApplication::loadSettings() {
  loadSettingsProxyConfiguration();
}

void MCClientApplication::saveSettings() {

}

void MCClientApplication::loadSettingsProxyConfiguration() {
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

void MCClientApplication::saveSettingsProxyConfiguration(
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

void MCClientApplication::run() {
  ILogger::Debug() << "Running...";

  mainWindow()->setup();
  mainWindow()->show();
}
