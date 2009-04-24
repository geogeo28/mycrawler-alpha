/****************************************************************************
 * @(#) Multi-threads exceptions manager (root).
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

#ifndef MCEXCEPTION_H
#define MCEXCEPTION_H

#include <exception>
#include <QString>

#define MCException(m, r)     _MCException(m, r, __PRETTY_FUNCTION__, __FILE__, __LINE__)

class _MCException : public std::exception
{
public:
    _MCException(
      const QString& message, const QString& remainder,
      const char* func, const char* file, int line
    );
    virtual ~_MCException() throw();

    virtual QString type() const {return QString("MCException");}

    const QString& remainder() const {return m_sRemainder;}
    const char* function() const {return m_pFunc;}
    const char* file() const {return m_pFile;}
    int line() const {return m_nLine;}
    virtual const QString& message() const throw() {return m_sMessage;}
    virtual const char* what() const throw();
    virtual QString description() const throw();
    virtual void dialog() const throw();

private:
    const QString m_sMessage;
    const QString m_sRemainder;
    const char* m_pFunc;
    const char* m_pFile;
    int m_nLine;
};

#endif
