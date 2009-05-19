/****************************************************************************
 * @(#) MyCrawler server. Dialog preferences.
 * GUI interface
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

#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include <QtGui>

#include "ui_DialogPreferences.h"

class MCDialogPreferences : public QDialog,
                            private Ui_DialogPreferences
{
public:
    MCDialogPreferences(QWidget* parent = NULL);

public slots:
    int exec();
    void accept();

private:
    void readServerConnectionConfiguration_();
    void writeServerConnectionConfiguration_();

    void readProxyConfiguration_();
    void writeProxyConfiguration_();

    void readAdvancedOptions_();
    void writeAdvancedOptions_();

private:
    void readSettings_();
    void writeSettings_();
};

#endif // DIALOGPREFERENCES_H
