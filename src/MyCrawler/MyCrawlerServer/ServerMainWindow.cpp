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

const char* SettingCurrentForm = "CurrentForm";

void MCServerMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCServerApplication::applicationName() + " v" + _MYCRAWLER_SERVER_VERSION_);

  // If this window have not parameters, place the window on the center of the screen if possible
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

void MCServerMainWindow::setupMainToolBar_() {
  QString sActionName = MCApp->settings()->value(SettingCurrentForm, "doMainToolBarClients").toString();
  QAction* action = qFindChild<QAction*>(this, sActionName);
  if (action == NULL) {
    ILogger::Notice() << QString("Invalid value '%1' for the setting '%2'.")
                         .arg(sActionName)
                         .arg(SettingCurrentForm);
    action = doMainToolBarClients;
  }

  on_mainToolBar_actionTriggered(action);
}

void MCServerMainWindow::setupMenu_() {
  // View menu (Toolbars)
  menuViewToolBars->addAction(mainToolBar->toggleViewAction());

  // Help menu
  QObject::connect(doHelpAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MCServerMainWindow::setupComponents_() {
  QObject::connect(MCServer::instance(), SIGNAL(error(MCServer::Error)), this, SLOT(slotServerError(MCServer::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(clientError(MCClientThread*, MCClientThread::Error)), this, SLOT(slotClientError(MCClientThread*, MCClientThread::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)), this, SLOT(slotClientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)));
  QObject::connect(MCServer::instance(), SIGNAL(clientTimeout(MCClientThread*, MCClientPeer::TimeoutNotify)), this, SLOT(slotClientTimeout(MCClientThread*,MCClientPeer::TimeoutNotify)));
  QObject::connect(MCServer::instance(), SIGNAL(clientErrorProcessingPacket(MCClientThread*,MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SLOT(slotClientErrorProcessingPacket(MCClientThread*,MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
}

void MCServerMainWindow::saveSettings_() {
  ILogger::Debug() << "Save settings.";
  MCApp->settings()->setValue(SettingCurrentForm, m_pActionCurrentForm->objectName());
  MCApp->settings()->saveLayout(this); // Window layout
}

void MCServerMainWindow::cleanAll_() {

}

void MCServerMainWindow::closeWindow_() {
  saveSettings_();
}

MCServerMainWindow::MCServerMainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_pActionCurrentForm(NULL)
{
  ILogger::Debug() << "Construct.";

  setupUi(this);

  try {
    setupWindow_();
    setupMainToolBar_();
    setupMenu_();
    setupComponents_();
  }
  catch(...) {
    cleanAll_();
    throw;
  }
}

MCServerMainWindow::~MCServerMainWindow() {
  closeWindow_();
  cleanAll_();

  ILogger::Debug() << "Destroyed.";
}

void MCServerMainWindow::on_buttonServerListen_clicked() {
  // TODO : Disconnect button and close all connections

  QString address = textServerAddress->text();
  quint16 port = textServerPort->text().toUShort();

  ILogger::Trace() << QString("The server listening the address %1 on the port %2.")
                      .arg(address)
                      .arg(port);

  MCServer::instance()->listen(QHostAddress(address), port);
}

void MCServerMainWindow::on_mainToolBar_actionTriggered(QAction* action) {
  QVariant mcFormIndex = action->property("mcFormIndex");
  if (mcFormIndex.isValid()) {
    // New action clicked
    if (action != m_pActionCurrentForm) {
      tabWidgetForms->setCurrentIndex(mcFormIndex.toInt());

      // Uncheck the preview action
      if (m_pActionCurrentForm != NULL) {
        m_pActionCurrentForm->setChecked(false);
      }
    }

    action->setChecked(true);
    m_pActionCurrentForm = action;
  }
}

void MCServerMainWindow::on_doMainToolBarConnectDisconnect_triggered() {
  // Connect
  if (MCServer::instance()->isListening() == false) {
    if (connectServer_() == true) {
      doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/ConnectedIcon"));
      ILogger::Information() << QString("Listening the address %1 on the port %2...")
                                .arg(MCServer::instance()->serverAddress().toString())
                                .arg(MCServer::instance()->serverPort());
    }
  }
  // Disconnect
  else {
    int nClients = MCServer::instance()->countClients();
    if (nClients > 0) {
      int button = QMessageBox::warning(
        this, QApplication::applicationName(),
        QString("%1 client(s) are currently connected.\n" \
                "Do you want to continue and close all connections ?")
        .arg(nClients),
        QMessageBox::Yes | QMessageBox::No
      );

      if (button == QMessageBox::No) {
        return;
      }
    }

    MCServer::instance()->close();
    doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/UnconnectedIcon"));
  }
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

  ILogger::Error() << QString("Error on the client %1 : %2 (%3)%4.")
                      .arg(client->threadInfo().peerAddressAndPort())
                      .arg(client->errorString())
                      .arg(error)
                      .arg(remainder);
}

void MCServerMainWindow::slotClientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state) {
  AssertCheckPtr(client);

  ILogger::Trace() << QString("Connection state changed to the client %1 : %2 (%3)")
                      .arg(client->threadInfo().peerAddressAndPort())
                      .arg(MCClientThread::connectionStateToString(state))
                      .arg(state);
}

void MCServerMainWindow::slotClientTimeout(MCClientThread* client, MCClientPeer::TimeoutNotify notifiedWhen) {
  ILogger::Error() << QString(
      "The client %1 not responding (%2).\n" \
      "Check your Internet connection.")
      .arg(client->threadInfo().peerAddressAndPort())
      .arg(MCClientPeer::timeoutNotifyToString(notifiedWhen));
}

void MCServerMainWindow::slotClientErrorProcessingPacket(
  MCClientThread* client, MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size,
  bool aborted
)
{
  ILogger::Error() << QString("Error processing a packet of the client %1.\n" \
                              "(Type = %2, Size = %3) %4 (%5) \n" \
                              "%6")
                      .arg(client->threadInfo().peerAddressAndPort())
                      .arg(type)
                      .arg(size)
                      .arg(MCClientPeer::packetErrorToString(error))
                      .arg(error)
                      .arg(
                        (aborted == true)?
                        "To prevent of a DoS attack, the connection with the client was aborted.":
                        "Trying recover the packet.");
}

bool MCServerMainWindow::connectServer_() {
  QString address = textServerAddress->text();
  quint16 port = textServerPort->text().toUShort();

  if (!MCServer::instance()->listen(QHostAddress(address), port)) {
    ILogger::Error() << QString("Could not listen the address %1 on the port %2.\n" \
                                "%3 (%4).")
                        .arg(address)
                        .arg(port)
                        .arg(MCServer::instance()->errorString())
                        .arg(MCServer::instance()->serverError());
    return false;
  }

  return true;
}

void MCServerMainWindow::disconnectServer_() {
  MCServer::instance()->disconnect();
}
