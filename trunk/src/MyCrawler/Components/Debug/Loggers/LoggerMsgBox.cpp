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

#include "Debug/Exception.h"
#include "Debug/Loggers/LoggerMsgBox.h"

CLoggerMsgBox::CLoggerMsgBox(QWidget* widgetParent, int level)
  : ILogger(level),
    m_widgetParent(widgetParent)
{
  AssertCheckPtr(widgetParent);

  setString(&m_sBuffer);

  setWriteDateTime(false);
  setWriteLevel(false);
}

void CLoggerMsgBox::write(LogLevel level, const QString& message) {
  QMessageBox::Icon icon(QMessageBox::NoIcon);
  switch (level) {
    case ILogger::WarningLevel:
      icon = QMessageBox::Warning;
      break;
    case ILogger::ErrorLevel:
      icon = QMessageBox::Critical;
      break;
    case ILogger::InformationLevel:
      icon = QMessageBox::Information;
      break;

    default:;
  }

  // Show the message box
  QMessageBox* m = new QMessageBox(icon, QApplication::applicationName(), message, QMessageBox::Ok, m_widgetParent);
  QObject::connect(m, SIGNAL(finished(int)), m, SLOT(deleteLater())); // Avoid memory leaks
  m->show();

  // Avoid accumulating messages
  m_sBuffer.clear();
}
