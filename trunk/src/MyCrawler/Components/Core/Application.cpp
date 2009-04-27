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

#include "Core/Application.h"
#include "Debug/Logger.h"
#include "Debug/Loggers/LoggerConsole.h"
#include "Debug/Loggers/LoggerFile.h"
#include "Debug/Loggers/LoggerDebug.h"

void IApplication::cleanAll_() {
  if (m_pLoggerConsole) { delete m_pLoggerConsole; m_pLoggerConsole = NULL; }
  if (m_pLoggerFile) { delete m_pLoggerFile; m_pLoggerFile = NULL; }

  #ifdef QT_DEBUG
    delete m_pLoggerDebug; m_pLoggerDebug = NULL;
  #endif
}

IApplication::IApplication(int &argc, char **argv)
  : QApplication(argc, argv),
    m_pLoggerConsole(NULL), m_pLoggerFile(NULL)
{
  #ifdef QT_DEBUG
    m_pLoggerDebug = NULL;
  #endif

  #if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
  #else
    QApplication::setQuitOnLastWindowClosed(true);
  #endif
}

IApplication::~IApplication()
{
  cleanAll_();
}

void IApplication::setInformations(const QString& name, const QString& organizationName, const QString& organizationDomain) {
  setApplicationName(name);
  setOrganizationName(organizationName);
  setOrganizationDomain(organizationDomain);
}

void IApplication::installTranslator(const QString& name) {
  QTranslator *translator = new QTranslator(this);
  translator->load(name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  QApplication::installTranslator(translator);
}

void IApplication::installLoggers() {
  m_pLoggerConsole = new CLoggerConsole(ILogger::AllLevel);
  m_pLoggerFile = new CLoggerFile(ILogger::AllLevel & ~ILogger::DebugLevel, applicationName() + ".log", CLoggerFile::OverwriteMode);
  m_pLoggerDebug = new CLoggerDebug("debug.log", CLoggerDebug::AppendMode);

  ILogger::attachLogger(m_pLoggerConsole);
  ILogger::attachLogger(m_pLoggerFile);
  ILogger::attachLogger(m_pLoggerDebug);
}
