/****************************************************************************
 * @(#) Application core component.
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

class CLoggerConsole;
class CLoggerFile;
class CLoggerDebug;

class IApplication : public QApplication
{
private:
    void cleanAll_();

protected:
    IApplication(int &argc, char **argv);
    virtual ~IApplication();    

public:
    static void setInformations(const QString& name, const QString& organizationName = QString(), const QString& organizationDomain = QString());

    void installTranslator(const QString& name = QLatin1String("qt_") + QLocale::system().name());
    void installLoggers();

    virtual void run() =0;

private:
    CLoggerConsole* m_pLoggerConsole;
    CLoggerFile* m_pLoggerFile;
    #ifdef QT_DEBUG
      CLoggerDebug* m_pLoggerDebug;
    #endif
};

#endif // APPLICATION_H
