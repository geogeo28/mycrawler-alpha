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

#include "MCServerApplication.h"
#include "MCServerMainWindow.h"

MCServerApplication* MCServerApplication::s_instance = NULL;

MCServerApplication* MCServerApplication::instance() {
  return s_instance;
}

void MCServerApplication::init_() {
  ILogger::Debug() << "Allocate memory for the application's main window.";
  MainWindow = new MCServerMainWindow();
}

void MCServerApplication::cleanAll_() {
  ILogger::Debug() << "Unallocate memory for the application's main window.";
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

  ILogger::Debug() << "Creating application components.";
  init_();
}

MCServerApplication::~MCServerApplication() {
  ILogger::Debug() << "Destroying application components.";
  cleanAll_();
}

void MCServerApplication::run() {
  ILogger::Debug() << "Application is running.";

  MainWindow->show();

  // Default configuration if the settings file isn't present
  /*if (Settings->isEmpty()) {
    ILogger::Warning() << "The settings file was not found on your computer. The default configuration will be loaded.\n" \
                          "Please changed the application' parameters with the dialog box of configuration.";
  }*/
}
