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
#include "Debug/Logger.h"

#include "ClientMainWindow.h"
#include "ClientApplication.h"

void MCClientMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCClientApplication::applicationName() + " v" + _MYCRAWLER_CLIENT_VERSION_);

  // If this window have not parameters, place the window on the center of the screen if possible
  ILogger::Debug() << "Try to load the setting window layout.";
  if (!MCApp->settings()->loadLayout(this)) {
    QDesktopWidget desktopWidget;
    int x = (desktopWidget.width() - this->width()) / 2;
    int y = (desktopWidget.height() - this->height()) / 2;
    if ((x >= 0) && (y >= 0)) {
      this->move(x, y);
    }
    else {
      this->setWindowState(Qt::WindowMaximized);
    }
  }
}

void MCClientMainWindow::setupComponents_() {
  ILogger::Debug() << "Setup signals/slots connections.";
}

void MCClientMainWindow::cleanAll_() {

}

void MCClientMainWindow::closeWindow_() {
  ILogger::Debug() << "Save the setting window layout.";
  MCApp->settings()->saveLayout(this);
}

MCClientMainWindow::MCClientMainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  ILogger::Debug() << "Construct.";

  setupUi(this);

  try {
    setupWindow_();
    setupComponents_();
  }
  catch(...) {
    cleanAll_();
    throw;
  }
}

MCClientMainWindow::~MCClientMainWindow()
{
  closeWindow_();
  cleanAll_();

  ILogger::Debug() << "Destroyed.";
}
