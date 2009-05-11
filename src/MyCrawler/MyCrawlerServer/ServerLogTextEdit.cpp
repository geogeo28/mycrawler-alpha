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

#include <QTime>

#include "Debug/Logger.h"
#include "ServerLogTextEdit.h"

MCServerLogTextEdit::MCServerLogTextEdit(QWidget* parent)
  : QTextEdit(parent)
{}

void MCServerLogTextEdit::write(Icon icon, const QString& message, const QString& style) {
  QString img;
  switch (icon) {
    case ErrorIcon:
      img = "<img src=\":/Log/ErrorIcon\" />&nbsp;";
      break;
    case InformationIcon:
      img = "<img src=\":/Log/InformationIcon\" />&nbsp;";
      break;

    default:;
  }

  moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  insertHtml(
    QString("<table><tr><td>[%1]&nbsp;%2<td><td style=\"%4\">%3</td></tr></table>")
    .arg(QTime::currentTime().toString(Qt::ISODate))
    .arg(img)
    .arg(message)
    .arg(style)
  );
  moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
}
