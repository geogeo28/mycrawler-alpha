/****************************************************************************
 * @(#) QTreeWidget extended.
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

#ifndef MYQTREEWIDGET_H
#define MYQTREEWIDGET_H

#include <QtGui>
#include <QContextMenuEvent>
#include <QPointer>

class QSettings;

typedef struct {
  const char* icon;
  const char* name;
  int width;
  bool hidden;
} MyQTreeWidgetHeaderItem;

class MyQTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    MyQTreeWidget(QWidget* parent = NULL);
    virtual ~MyQTreeWidget();

    virtual void setup() =0;

public:
    QMenu* contextMenu() const { return m_pContextMenu; }
    void saveLayout(QSettings* settings, const QString& keyName = QString());
    void loadLayout(QSettings* settings, const QString& keyName = QString());

public slots:
    void showColumnFromAction(bool show);

private slots:
    void slotSortIndicatorChanged_(int logicalIndex, Qt::SortOrder order);

protected:
    void setupHeader(const MyQTreeWidgetHeaderItem headers[], int nColumns);
    QMenu* columnsMenu();

    virtual void setupContextMenu();
    virtual void setContextMenu(QMenu* menu) { m_pContextMenu = menu; }

    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    int m_nColumnSortedIndex;
    QPointer<QMenu> m_pContextMenu;
};

#endif // MYQTREEWIDGET_H
