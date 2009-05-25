/****************************************************************************
 * @(#) Client component. Application base class.
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

#ifndef CLIENTAPPLICATION_H
#define CLIENTAPPLICATION_H

#include <QPointer>

#include "Core/Application.h"
#include "SettingsApplication.h"
#include "ClientMainWindow.h"

class MCClientApplication : public IApplication
{
private:
    void init_();
    void cleanAll_();

public:
    static MCClientApplication* instance();
    static void destroy();

    MCClientApplication(int &argc, char** argv);
    ~MCClientApplication();

 public:
    MCClientMainWindow* mainWindow() { return m_pMainWindow; }

  // Resources
 public:
    void initResources();
    void cleanupResources();

 // Settings
 public:
    void loadSettings();
    void saveSettings();

    void loadServersList(const QString& fileName);
    void saveServersList(const QString& fileName);

    void loadSettingsProxyConfiguration();
    void saveSettingsProxyConfiguration(
      bool useProxy,
      const QString& hostName = QString(), quint16 port = 0,
      const QString& userName = QString(), const QString& password = QString()
    );

public:
    void run();

private:
    static MCClientApplication* s_instance;
    QPointer<MCClientMainWindow> m_pMainWindow;
};

#define MCApp         MCClientApplication::instance()
#define MCSettings    MCApp->settings()

#endif // CLIENTAPPLICATION_H
