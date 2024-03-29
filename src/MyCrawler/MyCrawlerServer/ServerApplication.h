/****************************************************************************
 * @(#) Multi-threads server component. Application base class.
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

#ifndef SERVERAPPLICATION_H
#define SERVERAPPLICATION_H

#include <QString>
#include <QPointer>

#include "Core/Application.h"
#include "UrlsCollection.h"

#include "SettingsApplication.h"
#include "ServerMainWindow.h"

class MCServerApplication : public IApplication
{
private:
    void init_();
    void cleanAll_();

public:
    static MCServerApplication* instance();
    static void destroy();

    MCServerApplication(int &argc, char** argv);
    ~MCServerApplication();
 
 public:
    MCServerMainWindow* mainWindow() const { return m_pMainWindow; }
    MCUrlsCollection* urlsInQueue() const { return m_pUrlsInQueue; }
    MCUrlsCollection* urlsCrawled() const { return m_pUrlsCrawled; }

 // Resources
 public:
    void initResources();
    void cleanupResources();

 // Settings
 public:
    void loadSettings();
    void saveSettings();

    void loadServerHistory(const QString& fileName);
    void saveServerHistory(const QString& fileName);

    void loadSettingsServerConnection();
    void saveSettingsServerConnection(
      const QString& address, quint16 port,
      const QString& name,
      int maxConnections
    );

    void loadSettingsProxyConfiguration();
    void saveSettingsProxyConfiguration(
      bool useProxy,
      const QString& hostName = QString(), quint16 port = 0,
      const QString& userName = QString(), const QString& password = QString()
    );

    void loadSettingsServerHistory();
    void saveSettingsServerHistory();

public:    
    void run();

private:
    static MCServerApplication* s_instance;

    QPointer<MCServerMainWindow> m_pMainWindow;
    QPointer<MCUrlsCollection> m_pUrlsInQueue;
    QPointer<MCUrlsCollection> m_pUrlsCrawled;
};

#define MCApp         MCServerApplication::instance()
#define MCSettings    MCApp->settings()

#endif // SERVERAPPLICATION_H
