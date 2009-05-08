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
#include <QPointer>

#include "Config/Settings.h"

class QWidget;

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
    static void setInformation(const QString& name, const QString& organizationName = QString(), const QString& organizationDomain = QString());

    void installTranslator(const QString& name = QLatin1String("qt_") + QLocale::system().name());
    void installLoggers();
    void installLoggerMsgBox(QWidget* widgetParent);
    void installSettings(const QString& fileName, const QString& folderName = QString("\\"), CSettings::Scope = CSettings::UserScope);

    CSettings* settings() { return m_pSettings; }

    virtual void run() =0;

private:
    QPointer<CLoggerConsole> m_pLoggerConsole;
    QPointer<CLoggerFile> m_pLoggerFile;
    QPointer<CLoggerMsgBox> m_pLoggerMsgBox;
    #ifdef QT_DEBUG
      QPointer<CLoggerDebug> m_pLoggerDebug;
    #endif

    QPointer<CSettings> m_pSettings;
};

#endif // APPLICATION_H
