/****************************************************************************
 * @(#) Tree Widget of urls crawled.
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

#ifndef URLSCRAWLEDTREEWIDGET_H
#define URLSCRAWLEDTREEWIDGET_H

#include "UrlInfo.h"

#include "Widgets/MyQTreeWidget.h"

class MCUrlsCrawledTreeWidget : public MyQTreeWidget
{
    Q_OBJECT

public:
    enum {
      HashSignatureColumn,
      OrderColumn,
      DepthColumn,
      UrlColumn,
      ParentColumn
    };

private:
    void cleanAll_();

public:
    MCUrlsCrawledTreeWidget(QWidget* parent = NULL);
    ~MCUrlsCrawledTreeWidget();

    void setup();

private slots:
    void slotAddUrl_(MCUrlInfo urlInfo); // Set data ItemUrlInQueue
    void slotRemoveUrl_(MCUrlInfo urlInfo); // Remove data ItemUrlInQueue
};

#endif // URLSCRAWLEDTREEWIDGET_H
