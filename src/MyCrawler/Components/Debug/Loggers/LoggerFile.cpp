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
#include "Debug/Logger.h"
#include "Debug/Loggers/LoggerFile.h"
#include "Debug/Exception.h"

#include <QApplication>
#include <QFile>

void AbstractLoggerFile::cleanAll_() {

}

AbstractLoggerFile::AbstractLoggerFile(int level, const QString& file, WriteMode mode, QObject* parent) throw(CFileException)
  : ILogger(level, parent)
{ 
  // Set the open mode of the file
  QFile::OpenMode openMode = QFile::WriteOnly | QFile::Text;
  if (mode == AbstractLoggerFile::AppendMode) { openMode |= QFile::Append; }

  try {
    m_pFile = new QFile(file, this);

    // Could not open the file (throw an exception)
    if (!m_pFile->open(openMode)) {
      ThrowFileAccessException(file, m_pFile->errorString());
    }

    // Attach the device to the Text Stream of the logger
    setDevice(m_pFile);
  }
  catch (...) {
    cleanAll_();
    throw;
  }
}

AbstractLoggerFile::~AbstractLoggerFile() {
  cleanAll_();
}

CLoggerFile::CLoggerFile(int level, const QString& file, WriteMode mode, QObject* parent) throw(CFileException)
  : AbstractLoggerFile(level, file, mode, parent)
{
  *this << "  ======================================================================================\n"
        << "  " + qApp->applicationName() + " v" + qApp->applicationVersion() +"\n"
        << "  Copyright (c) by "_MYCRAWLER_AUTHOR_"\n\n"
        << "  Compiled with "_COMPILER_DESCRIPTION_" at "_COMPILER_BUILD_DATE_"\n"
        << "  ======================================================================================\n";
}

CLoggerFile::~CLoggerFile() {
  *this << "\n  ======================================================================================\n"
        << "  Ending the " << ILogger::currentDate() << " at " << ILogger::currentTime() << "\n"
        << "  ======================================================================================\n";
}
