/****************************************************************************
 * @(#) Logger base class.
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

#ifndef ILOGGER_H
#define ILOGGER_H

#include <QTextStream>

class QString;
class QIODevice;

class CLoggerManipulator
{
  public:
    CLoggerManipulator(QTextStream* textStream = NULL)
      : m_pTextStream(textStream)
    {}

    ~CLoggerManipulator() {
      if (m_pTextStream) { *m_pTextStream << "\n"; }
    }

    template <class T> CLoggerManipulator& operator<<(const T& text);

  private:
    QTextStream* m_pTextStream;
};

template <class T> CLoggerManipulator& CLoggerManipulator::operator<<(const T& text) {
  if (m_pTextStream) { *m_pTextStream << text; }
  return *this;
}

class ILogger
{
public:
    typedef enum {
      DebugLevel    = 1,
      WarningLevel  = 2,
      ErrorLevel    = 4,
      InformationLevel     = 8,

      NoticeLevel   = WarningLevel | ErrorLevel | InformationLevel,
      AllLevel      = DebugLevel | WarningLevel | ErrorLevel | InformationLevel
    } LogLevel;

public:
    static ILogger* instance();

    ILogger(QTextStream* textStream);
    virtual ~ILogger();

    void setLogger(ILogger* logger);

    CLoggerManipulator Log(LogLevel level, const char* func = NULL);
    void LogC(LogLevel level, const char* format, ...);

    CLoggerManipulator Debug_(const char* func)  {return Log(DebugLevel, func); }
    CLoggerManipulator Warning()     { return Log(WarningLevel); }
    CLoggerManipulator Error()       { return Log(ErrorLevel); }
    CLoggerManipulator Information() { return Log(InformationLevel); }

protected:
    static QString currentTime();
    static QString currentDate();
    static QString logLevelToString(LogLevel level);

protected:
    ILogger(LogLevel level);

    void setDevice(QIODevice* device);

protected:
    QTextStream textStream;

private:
    static ILogger* s_instance;
    int m_nRegisteredLevels;
};

#define Debug()     Debug_(__PRETTY_FUNCTION__)

#endif // ILOGGER_H
