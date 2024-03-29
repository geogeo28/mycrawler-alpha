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

#include <QObject>
#include <QString>
#include <QDate>
#include <QTime>
#include <cstdarg>

#include "Debug/Logger.h"
#include "Debug/Exception.h"

CLoggerManipulator::CLoggerManipulator(int level, QList<ILogger*> lstLoggers)
  : m_enumWriteLevel(level), m_lstLoggers(lstLoggers)
{
  foreach (ILogger* logger, lstLoggers) {
    logger->mutex.lock();
  }
}

CLoggerManipulator::CLoggerManipulator(int level, ILogger* logger)
  : m_enumWriteLevel(level)
{
  logger->mutex.lock();

  AssertCheckPtr(logger);
  m_lstLoggers.push_back(logger);
}

CLoggerManipulator::~CLoggerManipulator() {
  foreach (ILogger* logger, m_lstLoggers) {
    Assert(logger && (logger->device() || logger->string()));

    // Call virtual method for a special device (see CLoggerMsgBox).
    logger->write(static_cast<ILogger::LogLevel>(m_enumWriteLevel), *(logger->string()));

    // Flush stream if set
    if (logger->flushStream() == true) {
      logger->flush();
    }

    logger->mutex.unlock();
  }
}


QMutex ILogger::s_mutex;
QMultiMap<int, ILogger*> ILogger::s_loggersCollection = QMultiMap<int, ILogger*>();

void ILogger::init_() {
  setWriteDateTime(true);
  setWriteLevel(true);
  setFlushStream(false);
}

ILogger::ILogger(int level, QObject* parent)
  : QObject(parent),
    mutex(QMutex::Recursive),
    m_nRegisteredLevels(level)
{
  QMutexLocker locker(&mutex);

  init_();
}

ILogger::~ILogger()
{
  QMutexLocker locker(&mutex);

  ILogger::detachLogger(this);
}

void ILogger::attachLogger(ILogger* logger) {
  QMutexLocker locker(&s_mutex);

  AssertCheckPtr(logger);

  int levels = logger->levels();
  int l = 1;
  while (levels) {
    if (levels & 1) { s_loggersCollection.insertMulti(l, logger); }

    levels = levels >> 1;
    l = l << 1;
  }
}

void ILogger::detachLogger(ILogger* logger) {
  QMutexLocker locker(&s_mutex);

  AssertCheckPtr(logger);

  QMutableMapIterator<int, ILogger*> it(s_loggersCollection);
  while (it.hasNext()) {
    if (it.next().value() == logger) {
      it.remove();
    }
  }
}

void ILogger::flush() {  
  QMutexLocker locker(&mutex);

  m_textStream.flush();
}

CLoggerManipulator ILogger::log() {
  QMutexLocker locker(&mutex);

  write_();
  return CLoggerManipulator(NoLevel, this);
}

CLoggerManipulator ILogger::log(LogLevel level) {
  QMutexLocker locker(&mutex);

  write_(level);
  return CLoggerManipulator(level, this);
}

void ILogger::write(const char* format, ...) {
  QMutexLocker locker(&mutex);

  va_list params;
  va_start(params, format);
  log() << QString().vsprintf(format, params);
  va_end(params);
}

void ILogger::write(LogLevel level, const char* format, ...) {
  QMutexLocker locker(&mutex);

  va_list params;
  va_start(params, format);
  log(level) << QString().vsprintf(format, params);
  va_end(params);
}

void ILogger::Log(LogLevel level, const char* format, ...) {
  QMutexLocker locker(&s_mutex);

  va_list params;
  va_start(params, format);
  ILogger::Log_(level, NULL) << QString().vsprintf(format, params);
  va_end(params);
}

QString ILogger::currentTime() { return QTime::currentTime().toString(Qt::ISODate); }

QString ILogger::currentDate() { return QDate::currentDate().toString(Qt::ISODate); }

QString ILogger::logLevelToString(LogLevel level) {
  switch (level) {
    case DebugLevel:       return QT_TRANSLATE_NOOP(ILogger, "Debug");
    case TraceLevel:       return QT_TRANSLATE_NOOP(ILogger, "Trace");
    case NoticeLevel:      return QT_TRANSLATE_NOOP(ILogger, "Notice");
    case WarningLevel:     return QT_TRANSLATE_NOOP(ILogger, "Warning");
    case ErrorLevel:       return QT_TRANSLATE_NOOP(ILogger, "Error");
    case InformationLevel: return QT_TRANSLATE_NOOP(ILogger, "Information");

    default:
      return QT_TRANSLATE_NOOP(ILogger, "Level unknown");
  }
}

void ILogger::setDevice(QIODevice* device) {
  AssertCheckPtr(device);

  m_textStream.setDevice(device);
}

void ILogger::setString(QString* string) {
  AssertCheckPtr(string);

  m_textStream.setString(string);
}

CLoggerManipulator ILogger::Log_(LogLevel level, const char* func, const void* object) {
  // Disable debugging
  #ifndef QT_DEBUG
    Q_UNUSED(func);
    Q_UNUSED(object);
    if (level == DebugLevel) { return CLoggerManipulator(); }
  #endif

  // No loggers attached, returns a logger manipulator empty. It's equivalent to a null stream.
  if (s_loggersCollection.contains(level) == false) {
    return CLoggerManipulator();
  }

  // Allow to write content into loggers if the level of log passed in argument is set within registered levels
  QList<ILogger*> lstLoggers(s_loggersCollection.values(level));
  foreach (ILogger* logger, lstLoggers) {
    AssertCheckPtr(logger);

    logger->write_(level);

    // Write debug information
    #ifdef QT_DEBUG
      if ((level == DebugLevel) && func) {
        if (object != NULL) { *logger << object << " "; }
        *logger << "(" << func << ") ";
      }
    #endif
  }

  return CLoggerManipulator(level, lstLoggers);
}

void ILogger::write_() {
  Assert(device() || string());

  // Write date and time if set
  if (writeDateTime() == true) {
    *this << "[" << ILogger::currentDate() << " " << ILogger::currentTime() << "] ";
  }
}

void ILogger::write_(LogLevel level) {
  Assert(device() || string());

  // Write date and time
  write_();

  // Write level if set
  if (writeLevel() == true) {
    *this << "[" << ILogger::logLevelToString(level) << "] ";
  }
}
