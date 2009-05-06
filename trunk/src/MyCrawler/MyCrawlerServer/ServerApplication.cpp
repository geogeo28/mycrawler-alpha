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

#include "ServerApplication.h"
#include "Server.h"
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
  MainWindow = new MCServerMainWindow();
}

void MCServerApplication::cleanAll_() {
  MCServer::destroy();
  if (MainWindow) { delete MainWindow; }
}

MCServerApplication::MCServerApplication(int &argc, char** argv)
  : IApplication(argc, argv)
{ 
  Assert(s_instance == NULL);

  s_instance = this;

  // Set application informations
  setInformations(_MYCRAWLER_APPNAME_" (server)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
  setApplicationVersion(_MYCRAWLER_SERVER_VERSION_);

  // Install base components
  installTranslator();
  installLoggers();

  // Install settings manager
  CSettings::setPath(CSettings::XmlFormat, CSettings::UserScope, applicationDirPath());
  CSettings::setMethodWriteValue(CSettings::NotEmptyValues);
  installSettings("settings");

  init_();

  installLoggerMsgBox(MainWindow);
}

MCServerApplication::~MCServerApplication() {
  cleanAll_();
  ILogger::Debug() << "Destroyed.";
}

void MCServerApplication::run() {
  ILogger::Debug() << "Running...";

  MainWindow->show();
}
