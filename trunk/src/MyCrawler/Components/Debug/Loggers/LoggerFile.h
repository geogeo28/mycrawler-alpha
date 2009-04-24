/****************************************************************************
 * @(#) Logger class which used to log messages into a file.
 *
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

#ifndef CLOGGERFILE_H
#define CLOGGERFILE_H

#include "Debug/Logger.h"
#include "Exceptions/MCException.h"

class QFile;
class QString;

class CLoggerFile : public ILogger
{
public:
    typedef enum {
      OverwriteMode,
      AppendMode
    } WriteMode;

private:
    void cleanAll_();

public:
    CLoggerFile(ILogger::LogLevel level = ILogger::NoticeLevel, const QString& file = "output.log", WriteMode mode = OverwriteMode) throw(_MCException);
    ~CLoggerFile();

private:
    void writeHeader_();
    void writeFooter_();

private:
    QFile* m_pFile;
};

#endif // CLOGGERFILE_H
