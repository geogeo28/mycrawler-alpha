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

#include <QPointer>

#include "Core/Application.h"
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
    
    MCServerMainWindow* mainWindow() { return m_pMainWindow; }

    void run();

private:
    static MCServerApplication* s_instance;
    QPointer<MCServerMainWindow> m_pMainWindow;
};

#define MCApp   MCServerApplication::instance()

#endif // SERVERAPPLICATION_H
