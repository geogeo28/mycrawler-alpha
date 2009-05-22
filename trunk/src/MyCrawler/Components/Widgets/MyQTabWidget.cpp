/****************************************************************************
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

#include "Debug/Exception.h"

#include "MyQTabWidget.h"

MyQTabWidget::MyQTabWidget(QWidget* parent)
  : QTabWidget(parent),
    m_pToolBar(NULL), m_pActionCurrentForm(NULL)
 {}

MyQTabWidget::~MyQTabWidget()
{}

void MyQTabWidget::setTabBarHidden(bool hide) {
  tabBar()->setHidden(hide);
}

void MyQTabWidget::attachToolBar(QToolBar* toolBar, QAction* defaultActionCurrentForm) {
  AssertCheckPtr(toolBar);

  if (m_pToolBar != NULL) {
    detachToolBar();
  }

  setTabBarHidden(true);
  m_pToolBar = toolBar;
  slotToolBarActionTriggered(defaultActionCurrentForm);

  QObject::connect(m_pToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(slotToolBarActionTriggered(QAction*)));
}

void MyQTabWidget::detachToolBar() {
  AssertCheckPtr(m_pToolBar);

  QObject::disconnect(m_pToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(slotToolBarActionTriggered(QAction*)));

  setTabBarHidden(false);
  m_pToolBar = NULL;
  m_pActionCurrentForm = NULL;
}

void MyQTabWidget::slotToolBarActionTriggered(QAction* action) {
  QVariant formIndex = action->property("FormIndex");
  if (formIndex.isValid()) {
    // New action clicked
    if (action != m_pActionCurrentForm) {
      setCurrentIndex(formIndex.toInt());

      // Uncheck the preview action
      if (m_pActionCurrentForm != NULL) {
        m_pActionCurrentForm->setChecked(false);
      }
    }

    action->setChecked(true);
    m_pActionCurrentForm = action;
  }
}
