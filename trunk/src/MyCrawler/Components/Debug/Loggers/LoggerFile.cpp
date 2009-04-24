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
#include "Debug/Logger.h"
#include "Debug/Loggers/LoggerFile.h"
#include "Exceptions/MCException.h"
#include <QFile>

void CLoggerFile::cleanAll_() {
  // Delete the file stream
  if (m_pFile) { delete m_pFile; }
}

CLoggerFile::CLoggerFile(ILogger::LogLevel level, const QString& file, WriteMode mode) throw(_MCException)
  : ILogger(level),
    m_pFile(NULL)
{ 
  // Set the open mode of the file
  QFile::OpenMode openMode = QFile::WriteOnly | QFile::Text;
  if (mode == CLoggerFile::AppendMode) { openMode |= QFile::Append; }

  try {
    m_pFile = new QFile(file);

    // Could not open the file (throw an exception)
    if (!m_pFile->open(openMode)) {
      throw MCException(
        QString("Could not open the file '%1' to log messages.").arg(file), m_pFile->errorString()
      );
    }

    // Attachs the device to the Text Stream of the logger
    setDevice(m_pFile);

    writeHeader_();
  }
  catch (...) {
    cleanAll_();
    throw;
  }
}

CLoggerFile::~CLoggerFile() {
  writeFooter_();

  m_pFile->close();
  delete m_pFile;
}

void CLoggerFile::writeHeader_() {
  textStream << "  ======================================================================================\n"
             << "  "_MYCRAWLER_APPNAME_" v"_MYCRAWLER_VERSION_"\n"
             << "  Copyright (c) by "_MYCRAWLER_AUTHOR_"\n\n"
             << "  Compiled with "_COMPILER_DESCRIPTION_" at "_COMPILER_BUILD_DATE_"\n"
             << "  ======================================================================================\n";
}

void CLoggerFile::writeFooter_() {
  textStream << "\n  ======================================================================================\n"
             << "  Ending the " << ILogger::currentDate() << " at " << ILogger::currentTime() << "\n"
             << "  ======================================================================================\n";
}
