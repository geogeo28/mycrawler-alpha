/****************************************************************************
 * @(#) QTabWidget extended.
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

#ifndef MYQTABWIDGET_H
#define MYQTABWIDGET_H

#include <QtGui>

class MyQTabWidget : public QTabWidget
{
public:
    MyQTabWidget(QWidget* parent = 0)
      : QTabWidget(parent)
    {}
    virtual ~MyQTabWidget() {}

    QSize sizeHint() const { return baseSize(); }

    void setTabBarHidden(bool hide) { tabBar()->setHidden(hide); }
};

#endif // MYQTABWIDGET_H