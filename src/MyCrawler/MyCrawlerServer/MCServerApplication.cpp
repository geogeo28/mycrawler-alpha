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

#include "Config.h"
#include "MCServerApplication.h"

MCServerApplication* MCServerApplication::s_instance = NULL;

MCServerApplication* MCServerApplication::instance() {
  return s_instance;
}

MCServerApplication::MCServerApplication(int &argc, char** argv)
  : IApplication(argc, argv)
{
  Assert(s_instance == NULL);

  s_instance = this;

  setInformations(_MYCRAWLER_APPNAME_" (server)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
  setApplicationVersion(_MYCRAWLER_SERVER_VERSION_);

  installTranslator();
  installLoggers();
}

void MCServerApplication::run() {
  ILogger::Debug() << QString("Test a debug message (int %1)").arg(42);
  ILogger::Warning() << "Test a warning message (int " << 55 << ")";
  ILogger::Log(ILogger::ErrorLevel) << "Test an error message (int " << 688 << ")";
  ILogger::Log(ILogger::InformationLevel, "Test an information message (int %d)", 654);
}
