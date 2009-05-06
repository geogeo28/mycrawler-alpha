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

#include <QApplication>
#include <QMessageBox>

#include <iostream>
#include "Debug/Exception.h"
#include "Debug/Logger.h"

CException::CException(
  const QString& message, const QString& remainder,
  const char* func, const char* file, int line
)
  : std::exception(),
    m_sMessage(message), m_sRemainder(remainder),
    m_pFunc(func), m_pFile(file), m_nLine(line)
{
  ILogger::Debug() << "Construct.";
}

CException::~CException() throw()
{
  ILogger::Debug() << "Destroyed.";
}

const char* CException::what() const throw() {
  return m_sMessage.toLatin1().data();
}

QString CException::fullMessage() const throw() {
  QString msg(message());
  if (!remainder().isEmpty())
    msg += "\nDetails : " + remainder();
  return msg;
}

QString CException::description() const throw() {
  // A simple exception without a trace
  if (function() == NULL) {
    return (
      QString(
        "An exception %1 occured :\n" \
        "%2\n" \
        "Application will now terminate."
      ).arg(type())
       .arg(fullMessage())
    );
  }
  // A standard exception with a trace
  else {
    return (
      QString(
        "An exception %1 occured during the call of :\n" \
        "'%2' (%4:%5)\n%6\n" \
        "Application will now terminate."
      ).arg(type())
       .arg(function())
       .arg(file())
       .arg(line())
       .arg(fullMessage())
    );
  }
}

void CException::print() const throw() {
  // Print the exception in the console
  std::cout << description().toLatin1().constData() << std::endl;
}
void CException::dialog() const throw() {
  // No widget (print the exception in the console)
  if (qApp == NULL) {
    print();
  }
  // Show a dialog box in the current widget
  else {
    QMessageBox::critical(
      NULL, QApplication::applicationName(),
      description(),
      QMessageBox::Ok
    );
  }
}
