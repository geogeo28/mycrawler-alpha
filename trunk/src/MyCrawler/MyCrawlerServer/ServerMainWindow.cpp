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

#include "ServerMainWindow.h"
#include "ServerApplication.h"
#include "ClientPeer.h"

void MCServerMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCServerApplication::applicationName() + " v" + _MYCRAWLER_SERVER_VERSION_);

  // If this window have not parameters, place the window on the center of the screen if possible
  ILogger::Debug() << "Try to load the setting window layout.";
  if (!MCApp->Settings->loadLayout(this)) {
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

void MCServerMainWindow::setupComponents_() {  
  // Setup server signals/slots connections
  ILogger::Debug() << "Setup server signals/slots connections.";
  QObject::connect(MCServer::instance(), SIGNAL(error(MCServer::Error)), this, SLOT(slotServerError(MCServer::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(clientError(MCClientThread*, MCClientThread::Error)), this, SLOT(slotClientError(MCClientThread*, MCClientThread::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)), this, SLOT(slotClientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)));
}

void MCServerMainWindow::cleanAll_() {

}

void MCServerMainWindow::closeWindow_() {
  ILogger::Debug() << "Save the setting window layout.";
  MCApp->Settings->saveLayout(this);
}

MCServerMainWindow::MCServerMainWindow(QWidget *parent)
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

MCServerMainWindow::~MCServerMainWindow()
{
  closeWindow_();
  cleanAll_();

  ILogger::Debug() << "Destroyed.";
}

void MCServerMainWindow::on_buttonServerListen_clicked() {
  QString address = textServerAddress->text();
  QString port = textServerPort->text();

  ILogger::Trace() << QString("The server listening the address %1 on the port %2.")
                      .arg(address)
                      .arg(port);

  MCServer::instance()->listen(QHostAddress(address), port.toUShort());
}

void MCServerMainWindow::slotServerError(MCServer::Error error) {
  ILogger::Error() << MCServer::instance()->errorString();
}

void MCServerMainWindow::slotClientError(MCClientThread* client, MCClientThread::Error error) {
  AssertCheckPtr(client);

  QString remainder;
  if (error == MCClientThread::ClientPeerError) {
    const MCClientPeer* clientPeer = client->clientPeer();
    remainder = QString("\nDetails : %1 (%2)")
                .arg(clientPeer->errorString())
                .arg(clientPeer->error());
  }

  ILogger::Error() << QString("Client %1 : Error : %2 (%3)%4.")
                      .arg(client->threadInfo().peerAddressAndPort())
                      .arg(client->errorString())
                      .arg(error)
                      .arg(remainder);
}

void MCServerMainWindow::slotClientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state) {
  AssertCheckPtr(client);

  ILogger::Trace() << QString("Client %1 : State changed : %2 (%3)")
                      .arg(client->threadInfo().peerAddressAndPort())
                      .arg(MCClientThread::connectionStateToString(state))
                      .arg(state);
}
