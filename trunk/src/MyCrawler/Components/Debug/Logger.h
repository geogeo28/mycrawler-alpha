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
#include <QList>
#include <QMultiMap>

class QString;
class QIODevice;

class ILogger;

class CLoggerManipulator
{
  public:
    CLoggerManipulator(int level = 0, QList<ILogger*> lstLoggers = QList<ILogger*>());
    CLoggerManipulator(int level, ILogger* logger);
    ~CLoggerManipulator();

    template <class T> CLoggerManipulator& operator<<(const T& text);

  private:
    int m_enumWriteLevel;
    QList<ILogger*> m_lstLoggers;
};

class ILogger
{
    friend class CLoggerManipulator;

public:
    typedef enum {
      NoLevel           = 0,
      DebugLevel        = 1,
      TraceLevel        = 2,
      WarningLevel      = 4,
      ErrorLevel        = 8,
      InformationLevel  = 16,

      NoticeLevel   = TraceLevel |WarningLevel | ErrorLevel | InformationLevel,
      AllLevel      = DebugLevel | TraceLevel | WarningLevel | ErrorLevel | InformationLevel
    } LogLevel;

private:
    void init_();

public:
    static ILogger* instance();
    static void attachLogger(ILogger* logger);
    static void detachLogger(ILogger* logger);

    ILogger(QTextStream* textStream);
    virtual ~ILogger();

    CLoggerManipulator log();
    CLoggerManipulator log(LogLevel level);
    void write(const char* format, ...);
    void write(LogLevel level, const char* format, ...);

    int levels() const { return m_nRegisteredLevels; }
    bool writeDateTime() const {return m_bWriteDateTime; }
    bool writeLevel() const { return m_bWriteLevel; }
    bool flushStream() const { return m_bFlushStream; }
    void setWriteDateTime(bool status) { m_bWriteDateTime = status; }
    void setWriteLevel(bool status) { m_bWriteLevel = status; }
    void setFlushStream(bool status) { m_bFlushStream = status; }

 public:
    static void Log(LogLevel level, const char* format, ...);
    static CLoggerManipulator Log(LogLevel level) { return Log_(level, NULL); }
    static CLoggerManipulator Debug_(const char* func)  {return Log_(DebugLevel, func); }
    static CLoggerManipulator Trace()       { return Log_(TraceLevel); }
    static CLoggerManipulator Warning()     { return Log_(WarningLevel); }
    static CLoggerManipulator Error()       { return Log_(ErrorLevel); }
    static CLoggerManipulator Information() { return Log_(InformationLevel); }

protected:
    static QString currentTime();
    static QString currentDate();
    static QString logLevelToString(LogLevel level);

protected:
    ILogger(int level);

    void setDevice(QIODevice* device);
    virtual void write(LogLevel level, const QString& message) { textStream << "\n"; }

private:
    static CLoggerManipulator Log_(LogLevel level, const char* func = NULL);

private:
    void write_();
    void write_(LogLevel level);
    
public:
    QTextStream textStream;

private:
    static QMultiMap<int, ILogger*> s_loggersCollection;
    int m_nRegisteredLevels;
    bool m_bWriteDateTime;
    bool m_bWriteLevel;
    bool m_bFlushStream;
};

template <class T>
CLoggerManipulator& CLoggerManipulator::operator<<(const T& text)
{
  foreach (ILogger* logger, m_lstLoggers) {
    logger->textStream << text;
  }

  return *this;
}

#define Debug()     Debug_(__PRETTY_FUNCTION__)

#endif // ILOGGER_H
