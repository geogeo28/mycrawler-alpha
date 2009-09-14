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

#include <QObject>
#include <QTextStream>
#include <QList>
#include <QMultiMap>
#include <QMutex>

class QString;
class QIODevice;

class ILogger;

/*!
  \note Locks the mutex of ILogger passed in argument until the instance is destroyed.
 */
class CLoggerManipulator
{
  friend class ILogger;

  public:
    CLoggerManipulator(int level = 0, QList<ILogger*> lstLoggers = QList<ILogger*>());
    CLoggerManipulator(int level, ILogger* logger);
    ~CLoggerManipulator();

    template <class T> CLoggerManipulator& operator<<(const T& text);

  private:
    int m_enumWriteLevel;
    QList<ILogger*> m_lstLoggers;
};

class ILogger : public QObject
{
    Q_OBJECT

    friend class CLoggerManipulator;

public:
    typedef enum {
      NoLevel           = 0,
      DebugLevel        = 1,
      TraceLevel        = 2,
      NoticeLevel       = 4,
      WarningLevel      = 8,
      ErrorLevel        = 16,
      InformationLevel  = 32,

      NoticeLevels   = TraceLevel | NoticeLevel | WarningLevel | ErrorLevel | InformationLevel,
      AllLevels      = DebugLevel | TraceLevel | NoticeLevel | WarningLevel | ErrorLevel | InformationLevel
    } LogLevel;

private:
    void init_();

public:
    static void attachLogger(ILogger* logger); // thread-safe
    static void detachLogger(ILogger* logger); // thread-safe

    template <class T> ILogger& operator<<(const T& text); // thread-safe
    void flush(); // thread-safe
    QString* string() { return m_textStream.string(); }
    QIODevice* device() { return m_textStream.device(); }

    CLoggerManipulator log(); // thread-safe
    CLoggerManipulator log(LogLevel level); // thread-safe
    void write(const char* format, ...); // thread-safe
    void write(LogLevel level, const char* format, ...); // thread-safe

    int levels() const { QMutexLocker locker(&mutex); return m_nRegisteredLevels; } // thread-safe
    bool writeDateTime() const { QMutexLocker locker(&mutex); return m_bWriteDateTime; } // thread-safe
    bool writeLevel() const { QMutexLocker locker(&mutex); return m_bWriteLevel; } // thread-safe
    bool flushStream() const { QMutexLocker locker(&mutex); return m_bFlushStream; } // thread-safe
    void setWriteDateTime(bool status) { QMutexLocker locker(&mutex); m_bWriteDateTime = status; } // thread-safe
    void setWriteLevel(bool status) { QMutexLocker locker(&mutex); m_bWriteLevel = status; } // thread-safe
    void setFlushStream(bool status) { QMutexLocker locker(&mutex); m_bFlushStream = status; } // thread-safe

 public:
    static void Log(LogLevel level, const char* format, ...); // thread-safe
    static CLoggerManipulator Log(LogLevel level) { QMutexLocker locker(&s_mutex); return Log_(level, NULL); } // thread-safe
    static CLoggerManipulator Debug_(const char* func, const void* object)  { QMutexLocker locker(&s_mutex); return Log_(DebugLevel, func, object); } // thread-safe
    static CLoggerManipulator Trace()       { QMutexLocker locker(&s_mutex); return Log_(TraceLevel); } // thread-safe
    static CLoggerManipulator Notice()      { QMutexLocker locker(&s_mutex); return Log_(NoticeLevel); } // thread-safe
    static CLoggerManipulator Warning()     { QMutexLocker locker(&s_mutex); return Log_(WarningLevel); } // thread-safe
    static CLoggerManipulator Error()       { QMutexLocker locker(&s_mutex); return Log_(ErrorLevel); } // thread-safe
    static CLoggerManipulator Information() { QMutexLocker locker(&s_mutex); return Log_(InformationLevel); } // thread-safe

protected:
    static QString currentTime();
    static QString currentDate();
    static QString logLevelToString(LogLevel level);

protected:
    ILogger(int level, QObject* parent = NULL); // thread-safe
    virtual ~ILogger(); // thread-safe

    void setDevice(QIODevice* device);
    void setString(QString* string);
    virtual void write(LogLevel level, const QString& message) { Q_UNUSED(level); Q_UNUSED(message); *this << "\n"; }

private:
    static CLoggerManipulator Log_(LogLevel level, const char* func = NULL, const void* object = NULL);

private:
    void write_();
    void write_(LogLevel level);
    
private:
    static QMultiMap<int, ILogger*> s_loggersCollection;

private:
    static QMutex s_mutex;
    mutable QMutex mutex;

    QTextStream m_textStream;
    int m_nRegisteredLevels;
    bool m_bWriteDateTime;
    bool m_bWriteLevel;
    bool m_bFlushStream;
};

// Thread-safe writing operation
template <class T>
ILogger& ILogger::operator<<(const T& text) {
  QMutexLocker locker(&mutex);
  m_textStream << text;

  return *this;
}

template <class T>
CLoggerManipulator& CLoggerManipulator::operator<<(const T& text)
{
  foreach (ILogger* logger, m_lstLoggers) {
    *logger << text;
  }

  return *this;
}

#define CDebug()    Debug_(__PRETTY_FUNCTION__, NULL)
#define Debug()     Debug_(__PRETTY_FUNCTION__, this)

#endif // ILOGGER_H
