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
#include "Debug/Exception.h"
#include "Debug/Logger.h"
#include "Debug/Loggers/LoggerConsole.h"
#include "Debug/Loggers/LoggerFile.h"
#include "Debug/Loggers/LoggerDebug.h"

void IApplication::cleanAll_() {
  // Delete settings instance
  if (m_pSettings) { delete m_pSettings; }

  // Delete loggers
  if (m_pLoggerConsole) { delete m_pLoggerConsole; m_pLoggerConsole = NULL; }
  if (m_pLoggerFile) { delete m_pLoggerFile; m_pLoggerFile = NULL; }
  if (m_pLoggerMsgBox) { delete m_pLoggerMsgBox; m_pLoggerMsgBox = NULL; }

  #ifdef QT_DEBUG
    delete m_pLoggerDebug; m_pLoggerDebug = NULL;
  #endif 
}

IApplication::IApplication(int &argc, char **argv)
  : QApplication(argc, argv),
    m_pLoggerConsole(NULL), m_pLoggerFile(NULL), m_pLoggerMsgBox(NULL)
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

void IApplication::installSettings(const QString& fileName, const QString& folderName, CSettings::Scope scope) {
  if (m_pSettings) {
    ILogger::Error() << "Settings manager was previously installed.";
    return;
  }

  m_pSettings = new CSettings(fileName, folderName, scope);
}

void IApplication::installLoggers() {
  if (m_pLoggerConsole || m_pLoggerFile) {
    ILogger::Error() << "Loggers (console, file) were previously installed.";
    return;
  }

  // Standard loggers
  m_pLoggerConsole = new CLoggerConsole(ILogger::NoticeLevel);
  m_pLoggerFile = new CLoggerFile(ILogger::NoticeLevel, applicationName() + ".log", CLoggerFile::OverwriteMode);

  ILogger::attachLogger(m_pLoggerConsole);
  ILogger::attachLogger(m_pLoggerFile);

  // Logger of debugging
  #ifdef QT_DEBUG
    m_pLoggerDebug = new CLoggerDebug("debug.log", CLoggerDebug::AppendMode);
    ILogger::attachLogger(m_pLoggerDebug);
  #endif
}

void IApplication::installLoggerMsgBox(QWidget* widgetParent) {
  AssertCheckPtr(widgetParent);

  if (m_pLoggerMsgBox) {
    ILogger::Error() << "Logger (message box) was previously installed.";
    return;
  }

  m_pLoggerMsgBox = new CLoggerMsgBox(widgetParent, ILogger::InformationLevel | ILogger::WarningLevel | ILogger::ErrorLevel);
  ILogger::attachLogger(m_pLoggerMsgBox);
}
