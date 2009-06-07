/****************************************************************************
 * @(#) Exceptions manager (root).
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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <QString>

#define ThrowException(m, r)           throw CException(m, r, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define ThrowFileException(f, m, r)    throw CFileException(f, m, r, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define ThrowFileAccessException(f, r) throw CFileAccessException(f, r, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define ThrowFileFormatException(f, r) throw CFileFormatException(f, r, __PRETTY_FUNCTION__, __FILE__, __LINE__)

class CException : public std::exception
{
public:
    virtual QString type() const { return QString("CException"); }

    CException(
      const QString& message, const QString& remainder,
      const char* func = NULL, const char* file = NULL, int line = 0
    );
    virtual ~CException() throw();

    const QString& remainder() const {return m_sRemainder;}
    const char* function() const {return m_pFunc;}
    const char* file() const {return m_pFile;}
    int line() const {return m_nLine;}

    virtual const QString& message() const throw() {return m_sMessage;}
    virtual const char* what() const throw();
    virtual QString fullMessage() const throw();
    virtual QString description() const throw();
    virtual void print() const throw();
    virtual void dialog() const throw();

private:
    const QString m_sMessage;
    const QString m_sRemainder;
    const char* m_pFunc;
    const char* m_pFile;
    int m_nLine;
};

struct CAssertException : public CException
{
  QString type() const { return QString("CAssertException"); }

  CAssertException(const QString& condition, const char* func, const char* file, int line)
    : CException("Unsatisfied condition", condition, func, file, line)
  {}
};

#ifdef QT_DEBUG
  #define Assert(condition) if (!(condition)) throw CAssertException(#condition, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
  inline void DoNothing(bool) {}
  #define Assert(condition) DoNothing(!(condition))
#endif

#define AssertCheckPtr(p) Assert(p != NULL)

struct CFileException : public CException
{
  QString type() const { return QString("CFileException"); }

  CFileException(const QString& fileName, const QString& message, const QString& remainder, const char* func, const char* file, int line)
    : CException(
        QString("Unable to perform an operation on the file '%1'. %2").arg(fileName).arg(message),
        remainder,
        func, file, line
      )
  {}
};

struct CFileAccessException : public CFileException
{
  QString type() const { return QString("CFileAccessException"); }

  CFileAccessException(const QString& fileName, const QString& remainder, const char* func, const char* file, int line)
    : CFileException(fileName, "The process cannot access to the file.", remainder, func, file, line)
  {}
};

struct CFileFormatException : public CFileException
{
  QString type() const { return QString("CFileFormatException"); }

  CFileFormatException(const QString& fileName, const QString& remainder, const char* func, const char* file, int line)
    : CFileException(fileName, "File format not supported.", remainder, func, file, line)
  {}
};

#endif
