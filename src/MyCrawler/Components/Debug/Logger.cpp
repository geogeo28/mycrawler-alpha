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
    logger->m_mutex.lock();
  }
}

CLoggerManipulator::CLoggerManipulator(int level, ILogger* logger)
  : m_enumWriteLevel(level)
{
  AssertCheckPtr(logger);

  logger->m_mutex.lock();
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

    logger->m_mutex.unlock();
  }
}


QMultiMap<int, ILogger*> ILogger::s_loggersCollection = QMultiMap<int, ILogger*>();

void ILogger::init_() {
  setWriteDateTime(true);
  setWriteLevel(true);
  setFlushStream(false);
}

ILogger::ILogger(int level)
  : m_mutex(QMutex::Recursive),
    m_nRegisteredLevels(level)
{
  init_();
}

ILogger::~ILogger()
{
  ILogger::detachLogger(this);
}

void ILogger::attachLogger(ILogger* logger) {
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
  AssertCheckPtr(logger);

  QMutableMapIterator<int, ILogger*> it(s_loggersCollection);
  while (it.hasNext()) {
    if (it.next().value() == logger) {
      it.remove();
    }
  }
}

// Thread-safe flush TextStream
void ILogger::flush() {  
  m_mutex.lock();
  m_textStream.flush();
  m_mutex.unlock();
}

CLoggerManipulator ILogger::log() {
  write_();
  return CLoggerManipulator(NoLevel, this);
}

CLoggerManipulator ILogger::log(LogLevel level) {
  write_(level);
  return CLoggerManipulator(level, this);
}

void ILogger::write(const char* format, ...) {
  va_list params;
  va_start(params, format);
  log() << QString().vsprintf(format, params);
  va_end(params);
}

void ILogger::write(LogLevel level, const char* format, ...) {
  va_list params;
  va_start(params, format);
  log(level) << QString().vsprintf(format, params);
  va_end(params);
}

void ILogger::Log(LogLevel level, const char* format, ...) {
  va_list params;
  va_start(params, format);
  ILogger::Log_(level, NULL) << QString().vsprintf(format, params);
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
    case TraceLevel:
      s = QT_TRANSLATE_NOOP(ILogger, "Trace");
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
  AssertCheckPtr(device);

  m_mutex.lock();
  m_textStream.setDevice(device);
  m_mutex.unlock();
}

void ILogger::setString(QString* string) {
  AssertCheckPtr(string);

  m_mutex.lock();
  m_textStream.setString(string);
  m_mutex.unlock();
}

CLoggerManipulator ILogger::Log_(LogLevel level, const char* func, void* object) {
  // Disable debugging
  #ifndef QT_DEBUG
    if (level == DebugLevel) { return CLoggerManipulator(NULL); }
  #endif

  // No loggers attached, returns a logger manipulator empty. It's equivalent to a null stream.
  if (s_loggersCollection.contains(level) == false) {
    return CLoggerManipulator();
  }

  // Allow to write content into loggers if the level of log passed in argument is set within registered levels
  QList<ILogger*> lstLoggers(s_loggersCollection.values(level));
  foreach (ILogger* logger, lstLoggers) {
    logger->write_(level);

    // Write debug informations
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
