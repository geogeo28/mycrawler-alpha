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

#include <QPointer>

#include "Debug/Logger.h"
#include "Debug/Exception.h"

class QFile;
class QString;

class AbstractLoggerFile : public ILogger
{
    Q_OBJECT

public:
    typedef enum {
      OverwriteMode,
      AppendMode
    } WriteMode;

private:
    void cleanAll_();

protected:
    AbstractLoggerFile(
      int level, const QString& file, WriteMode mode = OverwriteMode, QObject* parent = NULL
    ) throw(CException);
    virtual ~AbstractLoggerFile();

private:
    QPointer<QFile> m_pFile;
};

class CLoggerFile : public AbstractLoggerFile
{
    Q_OBJECT

public:
    CLoggerFile(
      int level = ILogger::AllLevel, const QString& file = "output.log", WriteMode mode = OverwriteMode, QObject* parent = NULL
    ) throw(CException);
    ~CLoggerFile();
};

#endif // CLOGGERFILE_H
