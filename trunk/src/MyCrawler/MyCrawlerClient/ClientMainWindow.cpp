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

#include <QHostAddress>

#include "Debug/Exception.h"
#include "Debug/Logger.h"

#include "ClientMainWindow.h"
#include "DialogPreferences.h"
#include "ClientApplication.h"
#include "ServerInfo.h"
#include "ServersList.h"

void MCClientMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCClientApplication::applicationName() + " v" + _MYCRAWLER_CLIENT_VERSION_);

  // If this window have not parameters, place the window on the center of the screen if possible
  if (!MCSettings->loadLayout(this, "ClientMainWindow")) {
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

void MCClientMainWindow::setupMainToolBar_() {
  // Connect buttons
  QObject::connect(doMainToolBarPreferences, SIGNAL(triggered()), this, SLOT(on_doFilePreferences_triggered()));

  // Set default page
  QString sActionName = MCSettings->value(MCSettingsApplication::SettingTagCurrentForm, MCSettingsApplication::DefaultMainWindowForm).toString();
  QAction* action = qFindChild<QAction*>(this, sActionName);
  if (action == NULL) {
    ILogger::Notice() << QString("Invalid value '%1' for the setting '%2'.")
                         .arg(sActionName)
                         .arg(MCSettingsApplication::SettingTagCurrentForm);
    action = doMainToolBarClientLog;
  }

  tabWidgetForms->attachToolBar(mainToolBar, action);
}

void MCClientMainWindow::setupMenu_() {
  // View menu (Toolbars)
  menuViewToolBars->addAction(mainToolBar->toggleViewAction());

  // Help menu
  QObject::connect(doHelpAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MCClientMainWindow::setupForms_() {
  // Client log
  treeWidgetClientLog->setup();
  MCSettings->loadLayout<QTreeWidget>(treeWidgetClientLog, "ClientLogTreeWidget");
  treeWidgetClientLog->setupHeaderContextMenu();

  // Servers
  treeWidgetServers->setup();
  MCSettings->loadLayout<QTreeWidget>(treeWidgetServers, "ServersTreeWidget");
  treeWidgetServers->setupHeaderContextMenu();

  // Crawler (must be deprecated)
  treeWidgetThreadsInfos->setup(MCClientApplication::instance()->networkManager());
}

void MCClientMainWindow::setupStatusBar_() {
  statusbar->showMessage("Unconnected");
}

void MCClientMainWindow::setupComponents_() {
  qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
  qRegisterMetaType<MCClient::ConnectionState>("MCClient::ConnectionState");

  // Connect signals/slots
  QObject::connect(MCClient::instance(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotClientError(QAbstractSocket::SocketError)), Qt::QueuedConnection);
  QObject::connect(MCClient::instance(), SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SLOT(slotClientTimeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(MCClient::instance(), SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SLOT(slotClientErrorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
  QObject::connect(MCClient::instance(), SIGNAL(connectionStateChanged(MCClient::ConnectionState)), this, SLOT(slotClientConnectionStateChanged(MCClient::ConnectionState)), Qt::QueuedConnection);
}

void MCClientMainWindow::cleanAll_() {

}

void MCClientMainWindow::closeWindow_() {
  // Save window settings
  MCSettings->setValue(MCSettingsApplication::SettingTagCurrentForm, tabWidgetForms->actionCurrentForm()->objectName());

  MCSettings->saveLayout<QTreeWidget>(treeWidgetClientLog, "ClientLogTreeWidget");
  MCSettings->saveLayout<QTreeWidget>(treeWidgetServers, "ServersTreeWidget");
  MCSettings->saveLayout(this, "ClientMainWindow"); // Window layout

  MCApp->saveSettings();
}

MCClientMainWindow::MCClientMainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_bPreviouslyConnected(false),
    m_bCancelConnection(false)
{
  ILogger::Debug() << "Construct.";

  setupUi(this);
}

MCClientMainWindow::~MCClientMainWindow() {
  cleanAll_();

  ILogger::Debug() << "Destroyed.";
}

void MCClientMainWindow::setup() {
  setupWindow_();
  setupMainToolBar_();
  setupMenu_();
  setupForms_();
  setupStatusBar_();
  setupComponents_();

  // Auto connect ?
  if (MCSettings->value("AdvancedOptions/ServersConnectAtStartup", MCSettingsApplication::DefaultServersConnectAtStartup).toBool() == true) {
    on_doMainToolBarConnectDisconnect_triggered();
  }
}

void MCClientMainWindow::flushServersToConnectList() {
  m_lstServersToConnect.clear();
}

void MCClientMainWindow::on_doFilePreferences_triggered() {
  MCDialogPreferences dlgPreferences(this);
  dlgPreferences.exec();
}

void MCClientMainWindow::on_doMainToolBarConnectDisconnect_triggered() {
  // Disconnect
  if (MCClient::instance()->connectionState() != MCClient::UnconnectedState) {
    m_bCancelConnection = true;
    disconnectClient_();
    return;
  }

  // List of servers sorted by priority
  m_lstServersToConnect = MCServersList::instance()->serversListSorted();
  if (m_lstServersToConnect.isEmpty()) {
    ILogger::Error() << "The servers list is empty. You must add a new server entry.";
  }
  else {
    m_bPreviouslyConnected = false;
    connectClient_(m_lstServersToConnect.takeFirst());
  }
}

void MCClientMainWindow::on_buttonAddServer_clicked() {
  QHostAddress ip;
  quint16 port = spinBoxAddServerPort->value();
  QString name = textAddServerName->text();

  // Check the format of the IP address
  if (ip.setAddress(textAddServerIP->text()) == false) {
    ILogger::Error() << "The IP address you entered is not well formated.";
    return;
  }

  // The IP address already exists
  if (MCServersList::instance()->ipExists(ip.toIPv4Address()) == true) {
    ILogger::Error() << "The IP address you entered as a new address already exists.";
    return;
  }

  // Check if the IP address is valid
  MCServerInfo serverInfo(ip, port, name);
  if (serverInfo.isValid() == false) {
    ILogger::Error() << "Invalid IP address.";
    return;
  }

  // Add the server into the list
  MCServersList::instance()->addServer(serverInfo);

  // Clear the input area
  textAddServerIP->clear();
  spinBoxAddServerPort->setValue(8080);
  textAddServerName->clear();
}

void MCClientMainWindow::slotClientError(QAbstractSocket::SocketError error) {
  treeWidgetClientLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("Error : %1.")
      .arg(MCClient::instance()->errorString()),
    Qt::red, QFont::Bold
  );
}

void MCClientMainWindow::slotClientTimeout(MCClientPeer::TimeoutNotify notifiedWhen) {
  treeWidgetClientLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("The server not responding (%2).")
      .arg(MCClientPeer::timeoutNotifyToString(notifiedWhen)),
    Qt::red, QFont::Bold
  );
}

void MCClientMainWindow::slotClientErrorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted) {
  treeWidgetClientLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("Error processing a packet of the server.\n" \
            "(Type = %1, Size = %2) %3 (%4)\n" \
            "%5")
      .arg(type)
      .arg(size)
      .arg(MCClientPeer::packetErrorToString(error))
      .arg(error)
      .arg(
      (aborted == true)?
      "To prevent of a DoS attack, the connection with the server was aborted.":
      "Packet dropped."),
    Qt::red, QFont::Bold
  );
}

void MCClientMainWindow::slotClientConnectionStateChanged(MCClient::ConnectionState state) {
  QString message;
  QColor color = Qt::black;

  const MCServerInfo& serverInfo = MCClient::instance()->serverInfo();

  switch (state) {
    // Connecting
    case MCClient::ConnectingState:
    {
      // Log message
      message = QString("Connecting to %1 (%2)...")
                .arg(serverInfo.name())
                .arg(serverInfo.ipAndPortString());
      color = Qt::blue;

      // Button connected
      doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/UnconnectedIcon"));
      doMainToolBarConnectDisconnect->setIconText("Cancel");
      doMainToolBarConnectDisconnect->setText("Cancel");

      buttonConnectDisconnect->setText("Connecting...");

      statusbar->showMessage(QString("Connecting to %1...").arg(serverInfo.name()));

      break;
    }

    // Connected
    case MCClient::ConnectedState:
    {
      // Log message
      message = QString("Connected to %1 (%2)...")
                .arg(serverInfo.name())
                .arg(serverInfo.ipAndPortString());
      color = Qt::darkGreen;

      // Button connected
      doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/ConnectedIcon"));
      doMainToolBarConnectDisconnect->setIconText("Disconnect");
      doMainToolBarConnectDisconnect->setText("Disconnect");

      buttonConnectDisconnect->setText("Disconnect");

      statusbar->showMessage(QString("Connected to %1").arg(serverInfo.name()));

      // Add the current server in the list to servers to connect (used to reconnect)
      m_bPreviouslyConnected = true;
      m_lstServersToConnect.push_front(MCClient::instance()->serverInfo());

      break;
    }

    // Closing
    case MCClient::ClosingState:
    {
      // Disable Connect/Disconnect button
      doMainToolBarConnectDisconnect->setIconText("Closing...");
      doMainToolBarConnectDisconnect->setText("Closing...");
      doMainToolBarConnectDisconnect->setEnabled(false);

      buttonConnectDisconnect->setText("Closing...");
      buttonConnectDisconnect->setEnabled(false);

      statusbar->showMessage("Closing...");

      return;
    }

    // Unconnected
    case MCClient::UnconnectedState:
    {
      // Log message
      message = "Disconnected.";
      color = Qt::red;

      // Enable Connect/Disconnect button
      doMainToolBarConnectDisconnect->setEnabled(true);
      doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/UnconnectedIcon"));
      doMainToolBarConnectDisconnect->setIconText("Connect");
      doMainToolBarConnectDisconnect->setText("Connect");

      buttonConnectDisconnect->setEnabled(true);
      buttonConnectDisconnect->setText("Connect"); 

      statusbar->showMessage("Unconnected");

      bool tmpPreviouslyConnected = m_bPreviouslyConnected;
      bool tmpCancelConnection = m_bCancelConnection;

      m_bPreviouslyConnected = false;
      m_bCancelConnection = false;

      if (tmpCancelConnection == false) {
        if ((tmpPreviouslyConnected == false)
         || ((tmpPreviouslyConnected == true) && (MCSettings->value("AdvancedOptions/ServersAutoReconnect", MCSettingsApplication::DefaultServersAutoReconnect).toBool() == true)))
        {
          if (m_lstServersToConnect.isEmpty() == false) {
            connectClient_(m_lstServersToConnect.takeFirst());
          }

          // Don't show a message if not connection was established
          if (tmpPreviouslyConnected == false) {
            return;
          }
        }
      }

      break;
    }

    default:
      return;
  }

  treeWidgetClientLog->write(CLogTreeWidget::InformationIcon, message, color, QFont::Bold);
}

void MCClientMainWindow::closeEvent(QCloseEvent* event) {
  closeWindow_();
  event->accept();
}

void MCClientMainWindow::connectClient_(const MCServerInfo& serverInfo) {
  MCApp->loadSettingsProxyConfiguration();
  MCClient::instance()->connectToHost(serverInfo);
}

void MCClientMainWindow::disconnectClient_() {
  MCClient::instance()->disconnect();
}
