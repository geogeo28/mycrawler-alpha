/****************************************************************************
 * @(#) Logger class which used to log a message into a message box.
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

#ifndef LOGGERMSGBOX_H
#define LOGGERMSGBOX_H

#include <QMessageBox>
#include <QIODevice>

#include "Debug/Logger.h"

class QWidget;

class CLoggerMsgBox : public ILogger
{
public:
    CLoggerMsgBox(QWidget* widgetParent, int level = ILogger::WarningLevel | ILogger::ErrorLevel);

protected:
    void write(LogLevel level, const QString& message);

private:

    QWidget* m_widgetParent;
    QString m_sBuffer;
};

#endif // LOGGERMSGBOX_H
