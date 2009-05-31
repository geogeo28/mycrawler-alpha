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

#include <QUrl>

#include "Debug/Exception.h"
#include "Debug/Logger.h"

#include "UrlInfo.h"

#include "ServerMainWindow.h"
#include "DialogPreferences.h"
#include "ServerApplication.h"

void MCServerMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCServerApplication::applicationName() + " v" + _MYCRAWLER_SERVER_VERSION_);

  // If this window have not parameters, place the window on the center of the screen if possible
  if (!MCSettings->loadLayout(this, "ServerMainWindow")) {
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
  // Connect buttons
  QObject::connect(doMainToolBarPreferences, SIGNAL(triggered()), this, SLOT(on_doFilePreferences_triggered()));

  // Set default page
  QString sActionName = MCSettings->value(MCSettingsApplication::SettingTagCurrentForm, MCSettingsApplication::DefaultMainWindowForm).toString();
  QAction* action = qFindChild<QAction*>(this, sActionName);
  if (action == NULL) {
    ILogger::Notice() << QString("Invalid value '%1' for the setting '%2'.")
                         .arg(sActionName)
                         .arg(MCSettingsApplication::SettingTagCurrentForm);
    action = doMainToolBarServerLog;
  }

  tabWidgetForms->attachToolBar(mainToolBar, action);
}

void MCServerMainWindow::setupMenu_() {
  // View menu (Toolbars)
  menuViewToolBars->addAction(mainToolBar->toggleViewAction());

  // Help menu
  QObject::connect(doHelpAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MCServerMainWindow::setupForms_() {
  // Server log
  treeWidgetServerLog->setup();
  MCSettings->loadLayout<QTreeWidget>(treeWidgetServerLog, "ServerLogTreeWidget");
  treeWidgetServerLog->setupHeaderContextMenu();

  // Clients
  treeWidgetClients->setup();
  MCSettings->loadLayout<QTreeWidget>(treeWidgetClients, "ClientsTreeWidget");
  treeWidgetClients->setupHeaderContextMenu();

  // Tasks
  treeWidgetTasksUrlsInQueue->setup();
  MCSettings->loadLayout<QTreeWidget>(treeWidgetTasksUrlsInQueue, "TasksUrlsInQueueTreeWidget");
  treeWidgetTasksUrlsInQueue->setupHeaderContextMenu();

  treeWidgetTasksUrlsInProgress->setup();
  MCSettings->loadLayout<QTreeWidget>(treeWidgetTasksUrlsInProgress, "TasksUrlsInProgressTreeWidget");
  treeWidgetTasksUrlsInProgress->setupHeaderContextMenu();
}

void MCServerMainWindow::setupComponents_() {
  // Components
  m_pProgressDialogCloseClients = new QProgressDialog("Closing all connections...", QString(), 0, 0, this, Qt::Popup);
  m_pProgressDialogCloseClients->setModal(true);

  // Connect signals/slots
  QObject::connect(MCServer::instance(), SIGNAL(error(MCServer::Error)), this, SLOT(slotServerError(MCServer::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(stateChanged(MCServer::State)), this, SLOT(slotServerStateChanged(MCServer::State)));
  QObject::connect(MCServer::instance(), SIGNAL(clientError(MCClientThread*, MCClientThread::Error)), this, SLOT(slotClientError(MCClientThread*, MCClientThread::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(clientTimeout(MCClientThread*, MCClientPeer::TimeoutNotify)), this, SLOT(slotClientTimeout(MCClientThread*,MCClientPeer::TimeoutNotify)));
  QObject::connect(MCServer::instance(), SIGNAL(clientErrorProcessingPacket(MCClientThread*,MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,MCClientPeer::ErrorBehavior)), this, SLOT(slotClientErrorProcessingPacket(MCClientThread*,MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,MCClientPeer::ErrorBehavior)));
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionRefused(MCClientThread*,QString)), this, SLOT(slotClientConnectionRefused(MCClientThread*,QString)));
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)), this, SLOT(slotClientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)));
}

void MCServerMainWindow::cleanAll_() {
  if (m_pProgressDialogCloseClients) { delete m_pProgressDialogCloseClients; }
}

void MCServerMainWindow::closeWindow_() {
  // Save window settings
  MCSettings->setValue(MCSettingsApplication::SettingTagCurrentForm, tabWidgetForms->actionCurrentForm()->objectName());

  MCSettings->saveLayout<QTreeWidget>(treeWidgetServerLog, "ServerLogTreeWidget");
  MCSettings->saveLayout<QTreeWidget>(treeWidgetClients, "ClientsTreeWidget");
  MCSettings->saveLayout<QTreeWidget>(treeWidgetTasksUrlsInQueue, "TasksUrlsInQueueTreeWidget");
  MCSettings->saveLayout<QTreeWidget>(treeWidgetTasksUrlsInProgress, "TasksUrlsInProgressTreeWidget");
  MCSettings->saveLayout(this, "ServerMainWindow"); // Window layout

  MCApp->saveSettings();
}

MCServerMainWindow::MCServerMainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  ILogger::Debug() << "Construct.";

  setupUi(this);
}

MCServerMainWindow::~MCServerMainWindow() {
  cleanAll_();

  ILogger::Debug() << "Destroyed.";
}

void MCServerMainWindow::setup() {
  setupWindow_();
  setupMainToolBar_();
  setupMenu_();
  setupForms_();
  setupComponents_();

  // Auto connect ?
  if (MCSettings->value("AdvancedOptions/ServerAutoConnectAtStartup", MCSettingsApplication::DefaultServerAutoConnectAtStartup).toBool() == true) {
    on_doMainToolBarConnectDisconnect_triggered();
  }
}

void MCServerMainWindow::on_doFilePreferences_triggered() {
  MCDialogPreferences dlgPreferences(this);
  dlgPreferences.exec();
}

void MCServerMainWindow::on_doMainToolBarConnectDisconnect_triggered() {
  // Connect
  if (MCServer::instance()->isListening() == false) {
    connectServer_();
  }
  // Disconnect
  else {
    int nClients = MCServer::instance()->clientCount();
    if (nClients > 0) {
      int button = QMessageBox::warning(
        this, QApplication::applicationName(),
        QString("%1 client(s) is/are currently connected.<br />" \
                "Do you want to continue and close all connections ?")
                .arg(nClients),
        QMessageBox::Yes | QMessageBox::No
      );

      if (button == QMessageBox::No) {
        return;
      }
    }

    disconnectServer_();
  }
}

void MCServerMainWindow::on_buttonTasksAddSeedUrl_clicked() {
  const QString& sUrl = textTasksSeedUrl->text();
  QUrl url = MCUrlInfo::decodedUrl(sUrl);

  // Invalid Url
  if ((url.isValid() == false) || (url.host().isEmpty() == true)) {
    ILogger::Error() << "The Url you entered is not valid.";
    return;
  }

  // Already exists
  if (MCApp->urlsInQueue()->addUrl(url) == false) {
    ILogger::Error() << "The Url you entered already exists.";
    return;
  }

  listWidgetTasksSeedUrls->addItem(url.toString(QUrl::None));

  textTasksSeedUrl->setText("http://");
}

void MCServerMainWindow::slotProgressClientFinished(MCClientThread* client) {
  AssertCheckPtr(client);

  m_pProgressDialogCloseClients->setValue(m_pProgressDialogCloseClients->value() + 1);
}

void MCServerMainWindow::slotServerError(MCServer::Error error) {
  int errcode = error;
  if (error == MCServer::TcpSocketError) {
    errcode = MCServer::instance()->serverError();
  }

  treeWidgetServerLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("%1 (%2).")
      .arg(MCServer::instance()->errorString())
      .arg(errcode),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::slotServerStateChanged(MCServer::State state) {
  QString message;
  QColor color = Qt::black;

  switch (state) {      
    // Listening
    case MCServer::ListeningState:
    {
      // Log message
      message = QString("Listening the address %1 on the port %2...")
                .arg(MCServer::instance()->serverAddress().toString())
                .arg(MCServer::instance()->serverPort());
      color = Qt::darkGreen;

      // Button connected
      doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/ConnectedIcon"));
      doMainToolBarConnectDisconnect->setIconText("Disconnect");
      doMainToolBarConnectDisconnect->setText("Disconnect");
      break;
    }

    // Closing
    case MCServer::ClosingState:
    {
      // Log message
      message = "Closing all client connections...";
      color = Qt::blue;

      // Init progress dialog
      int nClients = MCServer::instance()->clientCount();
      if (nClients > 0) {
        m_pProgressDialogCloseClients->setMaximum(nClients);
        m_pProgressDialogCloseClients->setValue(0);

        QObject::connect(MCServer::instance(), SIGNAL(clientFinished(MCClientThread*)), this, SLOT(slotProgressClientFinished(MCClientThread*)));
        m_pProgressDialogCloseClients->show();
      }

      // Disable Connect/Disconnect button
      doMainToolBarConnectDisconnect->setIconText("Closing...");
      doMainToolBarConnectDisconnect->setText("Closing...");
      doMainToolBarConnectDisconnect->setEnabled(false);

      break;
    }

    // Closed
    case MCServer::ClosedState:
    {
      // Log message
      message = "Connection closed.";
      color = Qt::red;

      // Set signal/slot connection to progress dialog
      MCServer::instance()->disconnect(SIGNAL(clientFinished(MCClientThread*)), this, SLOT(slotProgressClientFinished(MCClientThread*)));
      m_pProgressDialogCloseClients->close();

      // Enable Connect/Disconnect button
      doMainToolBarConnectDisconnect->setEnabled(true);
      doMainToolBarConnectDisconnect->setIcon(QIcon(":/MainToolBar/UnconnectedIcon"));
      doMainToolBarConnectDisconnect->setIconText("Connect");
      doMainToolBarConnectDisconnect->setText("Connect");

      break;
    }

    default:
      return;
  }

  treeWidgetServerLog->write(CLogTreeWidget::InformationIcon, message, color, QFont::Bold);
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

  treeWidgetServerLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("Error on the client %1 : %2 (%3)%4.")
      .arg(client->peerAddressAndPort())
      .arg(client->errorString())
      .arg(error)
      .arg(remainder),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::slotClientTimeout(MCClientThread* client, MCClientPeer::TimeoutNotify notifiedWhen) {
  treeWidgetServerLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("The client %1 not responding (%2).")
      .arg(client->peerAddressAndPort())
      .arg(MCClientPeer::timeoutNotifyToString(notifiedWhen)),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::slotClientErrorProcessingPacket(
  MCClientThread* client, MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size,
  MCClientPeer::ErrorBehavior errorBehavior
)
{
  treeWidgetServerLog->write(
    CLogTreeWidget::ErrorIcon,
    QString("Error processing a packet of the client %1.\n" \
            "(Type = %2, Size = %3) %4 (%5)\n" \
            "%6")
      .arg(client->peerAddressAndPort())
      .arg(type)
      .arg(size)
      .arg(MCClientPeer::packetErrorToString(error))
      .arg(error)
      .arg(
      (errorBehavior == MCClientPeer::AbortBehavior)?
      "To prevent of a DoS attack, the connection with the client was aborted.":
      MCClientPeer::errorBehaviorToString(errorBehavior)),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::slotClientConnectionRefused(MCClientThread* client, const QString& reason) {
  AssertCheckPtr(client);

  QString message = QString("The connection with the client %1 was refused.")
                    .arg(client->peerAddressAndPort());

  if (!reason.isEmpty()) {
    message += "\nReason : " + reason + ".";
  }

  treeWidgetServerLog->write(CLogTreeWidget::ErrorIcon, message, Qt::red, QFont::Bold);
}

void MCServerMainWindow::slotClientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state) {
  AssertCheckPtr(client);

  QColor color = Qt::black;
  if (state == MCClientThread::ConnectedState)           { color = Qt::darkGreen; }
  else if (state == MCClientThread::AuthenticatingState) { color = Qt::blue; }
  else if (state == MCClientThread::UnconnectedState)    { color = Qt::red; }

  treeWidgetServerLog->write(
    CLogTreeWidget::InformationIcon,
    QString("Client %1 : %2 (%3)")
      .arg(client->peerAddressAndPort())
      .arg(MCClientThread::connectionStateToString(state))
      .arg(state),
    color, QFont::Bold
  );
}

void MCServerMainWindow::closeEvent(QCloseEvent* event) {
  disconnectServer_(); // Forces to disconnect all clients
  closeWindow_();
  event->accept();
}

bool MCServerMainWindow::connectServer_() {
  MCApp->loadSettingsServerConnection();
  MCApp->loadSettingsProxyConfiguration();

  if (MCServer::instance()->listenAddress().isNull()) {
    QMessageBox::critical(
      this, QApplication::applicationName(),
      "Could not listen an invalid address.<br />" \
      "Change the server address in Preferences Dialog.",
      QMessageBox::Ok
    );

    on_doFilePreferences_triggered();
    return false;
  }

  if (!MCServer::instance()->listen()) {
    treeWidgetServerLog->write(
      CLogTreeWidget::ErrorIcon,
      QString("Could not listen the address %1 on the port %2.\n" \
              "%3 (%4).")
        .arg(MCServer::instance()->listenAddress().toString())
        .arg(MCServer::instance()->listenPort())
        .arg(MCServer::instance()->errorString())
        .arg(MCServer::instance()->serverError()),
      Qt::red, QFont::Bold
    );
    return false;
  }

  return true;
}

void MCServerMainWindow::disconnectServer_() {
  MCServer::instance()->close();
  MCServer::instance()->waitForClosed();
}
