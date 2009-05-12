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

#include "GraphApplication.h"
#include "GraphMainWindow.h"

MCGraphApplication* MCGraphApplication::s_instance = NULL;

MCGraphApplication* MCGraphApplication::instance() {
  return s_instance;
}

void MCGraphApplication::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCGraphApplication::init_() {
  m_pMainWindow = new MCGraphMainWindow();
}

void MCGraphApplication::cleanAll_() {
  if (m_pMainWindow) { delete m_pMainWindow; }
}

MCGraphApplication::MCGraphApplication(int &argc, char** argv)
  : IApplication(argc, argv)
{
  Assert(s_instance == NULL);

  s_instance = this;

  // Set application information
  setInformation(_MYCRAWLER_APPNAME_" (graph)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
  setApplicationVersion("Not versioned");

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

MCGraphApplication::~MCGraphApplication() {
  cleanAll_();
  ILogger::Debug() << "Destroyed.";
}

void MCGraphApplication::run() {
  ILogger::Debug() << "Running...";

  mainWindow()->show();
}
