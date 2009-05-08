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

#include "Config/Config.h"
#include "Config/Settings.h"
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
}

MCClientApplication::MCClientApplication(int &argc, char** argv)
  : IApplication(argc, argv)
{ 
  Assert(s_instance == NULL);

  s_instance = this;

  // Set application informations
  setInformations(_MYCRAWLER_APPNAME_" (client)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
  setApplicationVersion(_MYCRAWLER_CLIENT_VERSION_);

  // Install base components
  installTranslator();
  installLoggers();

  // Install settings manager
  CSettings::setPath(CSettings::XmlFormat, CSettings::UserScope, applicationDirPath());
  CSettings::setMethodWriteValue(CSettings::NotEmptyValues);
  installSettings("settings");

  init_();

  installLoggerMsgBox(mainWindow());
}

MCClientApplication::~MCClientApplication() {
  cleanAll_();
  ILogger::Debug() << "Destroyed.";
}

void MCClientApplication::run() {
  ILogger::Debug() << "Running...";

  mainWindow()->show();
}
