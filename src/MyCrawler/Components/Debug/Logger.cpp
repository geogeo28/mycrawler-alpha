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

#include "Debug/Logger.h"
#include <QObject>
#include <QString>
#include <QDate>
#include <QTime>
#include <cstdarg>
#include <typeinfo>

ILogger* ILogger::s_instance = NULL;

ILogger::ILogger(LogLevel level)
  : m_nRegisteredLevels(level)
{}

ILogger::~ILogger()
{}

void ILogger::setLogger(ILogger* logger) {
  s_instance = logger;
}

CLoggerManipulator ILogger::Log(LogLevel level, const char* func) {
  bool bAllowWrite = (level & m_nRegisteredLevels);

  // Allows to write content into the logger if the level of log passed in argument is set within registered levels
  if (bAllowWrite == true) {
    textStream << "[" << ILogger::currentDate() << " " << ILogger::currentTime() << "] "
               << "[" << ILogger::logLevelToString(level) << "] ";

    // Write debug informations
    if ((level == DebugLevel) && (func != NULL)) {
        textStream << "(" << func << ") ";
    }

    return CLoggerManipulator(&textStream);
  }

  // Logger manipulator without Text Stream. It's equivalent to a null stream.
  return CLoggerManipulator(NULL);
}

void ILogger::LogC(LogLevel level, const char* format, ...) {
  va_list params;
  va_start(params, format);
  Log(level) << QString().vsprintf(format, params);
  va_end(params);
}

QString ILogger::currentTime() { return QTime::currentTime().toString(Qt::ISODate); }

QString ILogger::currentDate() { return QDate::currentDate().toString(Qt::ISODate); }

QString ILogger::logLevelToString(LogLevel level) {
  QString s;

  switch (level) {
    case DebugLevel:
      s = QT_TRANSLATE_NOOP(ILogger, "Debug");
      break;
    case WarningLevel:
      s = QT_TRANSLATE_NOOP(ILogger, "Warning");
      break;
    case ErrorLevel:
      s = QT_TRANSLATE_NOOP(ILogger, "Error");
      break;
    case InformationLevel:
      s = QT_TRANSLATE_NOOP(ILogger, "Information");
      break;

    default:;
  }

  return s;
}

void ILogger::setDevice(QIODevice* device) {
  Q_CHECK_PTR(device);

  textStream.setDevice(device);
}
