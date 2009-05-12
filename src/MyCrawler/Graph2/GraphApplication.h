/****************************************************************************
 * @(#)
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

#ifndef GRAPHAPPLICATION_H
#define GRAPHAPPLICATION_H

#include <QPointer>

#include "Core/Application.h"
#include "GraphMainWindow.h"

class MCGraphApplication : public IApplication
{
private:
    void init_();
    void cleanAll_();

public:
    static MCGraphApplication* instance();
    static void destroy();

    MCGraphApplication(int &argc, char** argv);
    ~MCGraphApplication();

    MCGraphMainWindow* mainWindow() { return m_pMainWindow; }

    void run();

private:
    static MCGraphApplication* s_instance;
    QPointer<MCGraphMainWindow> m_pMainWindow;
};

#define MCApp   	    MCGraphApplication::instance()
#define MCSettings    MCApp->settings()

#endif // GRAPHAPPLICATION_H
