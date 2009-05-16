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

#include <QNetworkProxy>

#include "Debug/Exception.h"
#include "Debug/Logger.h"

#include "ServerMainWindow.h"
#include "DialogPreferences.h"
#include "ServerApplication.h"
#include "ClientPeer.h"
#include "ServerLogWidget.h"

const char* SettingCurrentForm = "CurrentForm";

void MCServerMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCServerApplication::applicationName() + " v" + _MYCRAWLER_SERVER_VERSION_);

  // If this window have not parameters, place the window on the center of the screen if possible
  if (!MCApp->settings()->loadLayout(this, "MCServerMainWindow")) {
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

void MCServerMainWindow::setupServerLogForm_() {
  treeWidgetServerLog->setup();
  QObject::connect(buttonServerLogCopy, SIGNAL(clicked()), treeWidgetServerLog, SLOT(copyToClipboard()));
}

void MCServerMainWindow::setupClientsForm_() {
  treeWidgetClients->setup();
}

void MCServerMainWindow::setupComponents_() {
  // Hide TabBar of TabWidgetForms
  tabWidgetForms->setTabBarHidden(true);

  // Components
  m_pProgressDialogCloseClients = new QProgressDialog("Closing all connections...", QString(), 0, 0, this, Qt::Popup);
  m_pProgressDialogCloseClients->setWindowModality(Qt::WindowModal);

  // Connect signals/slots
  QObject::connect(MCServer::instance(), SIGNAL(error(MCServer::Error)), this, SLOT(slotServerError(MCServer::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(stateChanged(MCServer::State)), this, SLOT(slotServerStateChanged(MCServer::State)));
  QObject::connect(MCServer::instance(), SIGNAL(clientError(MCClientThread*, MCClientThread::Error)), this, SLOT(slotClientError(MCClientThread*, MCClientThread::Error)));
  QObject::connect(MCServer::instance(), SIGNAL(clientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)), this, SLOT(slotClientConnectionStateChanged(MCClientThread*, MCClientThread::ConnectionState)));
  QObject::connect(MCServer::instance(), SIGNAL(clientTimeout(MCClientThread*, MCClientPeer::TimeoutNotify)), this, SLOT(slotClientTimeout(MCClientThread*,MCClientPeer::TimeoutNotify)));
  QObject::connect(MCServer::instance(), SIGNAL(clientErrorProcessingPacket(MCClientThread*,MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SLOT(slotClientErrorProcessingPacket(MCClientThread*,MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
}

void MCServerMainWindow::loadSettingsServerConnection_() {
  MCSettings->beginGroup("ServerConnectionConfiguration");
    MCServer::instance()->setListenAddress(MCSettings->value("Address", "").toString());
    MCServer::instance()->setListenPort(MCSettings->value("Port", 8080).toUInt());

    MCServer::instance()->setMaxConnections(MCSettings->value("MaxConnections", MCServer::defaultMaxConnections()).toInt());
  MCSettings->endGroup();
}

void MCServerMainWindow::loadSettingsProxyConfiguration_() {
  QNetworkProxy proxy(QNetworkProxy::NoProxy);
  MCSettings->beginGroup("ProxyConfiguration");
    if (MCSettings->value("Use", false).toBool() == true) {
      proxy.setType(QNetworkProxy::HttpProxy);
      proxy.setHostName(MCSettings->value("HostName").toString());
      proxy.setPort(MCSettings->value("Port", "3128").toInt());
      proxy.setUser(MCSettings->value("UserName").toString());
      proxy.setPassword(QByteArray::fromBase64(MCSettings->value("UserPassword").toByteArray()));
    }
  MCSettings->endGroup();
  IApplication::setProxy(proxy);
}

void MCServerMainWindow::loadSettings_() {
  ILogger::Debug() << "Load settings.";
  loadSettingsServerConnection_();
  loadSettingsProxyConfiguration_();
}

void MCServerMainWindow::saveSettings_() {
  ILogger::Debug() << "Save settings.";
  MCApp->settings()->setValue(SettingCurrentForm, m_pActionCurrentForm->objectName());
  MCApp->settings()->saveLayout(this, "MCServerMainWindow"); // Window layout
}

void MCServerMainWindow::cleanAll_() {
  if (!m_pProgressDialogCloseClients.isNull()) { delete m_pProgressDialogCloseClients; }
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
    setupServerLogForm_();
    setupClientsForm_();
    setupComponents_();
    loadSettings_();

    // Auto connect ?
    if (MCSettings->value("ServerConnectionConfiguration/AutoConnect", false).toBool() == true) {
      on_doMainToolBarConnectDisconnect_triggered();
    }
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

void MCServerMainWindow::on_doFilePreferences_triggered() {
  MCDialogPreferences dlgPreferences(this);
  if (dlgPreferences.exec() == QDialog::Accepted) {
    loadSettings_();
  }
}

void MCServerMainWindow::on_mainToolBar_actionTriggered(QAction* action) {
  QVariant formIndex = action->property("FormIndex");
  if (formIndex.isValid()) {
    // New action clicked
    if (action != m_pActionCurrentForm) {
      tabWidgetForms->setCurrentIndex(formIndex.toInt());

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
    connectServer_();
  }
  // Disconnect
  else {
    int nClients = MCServer::instance()->countClients();
    if (nClients > 0) {
      int button = QMessageBox::warning(
        this, QApplication::applicationName(),
        QString("%1 client(s) are currently connected.<br />" \
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
    MCServerLogWidget::ErrorIcon,
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
                .arg(MCServer::instance()->listenAddress().toString())
                .arg(MCServer::instance()->listenPort());
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
      int nClients = MCServer::instance()->countClients();
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

    default:;
  }

  treeWidgetServerLog->write(MCServerLogWidget::InformationIcon, message, color, QFont::Bold);
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
    MCServerLogWidget::ErrorIcon,
    QString("Error on the client %1 : %2 (%3)%4.")
      .arg(client->peerAddressAndPort())
      .arg(client->errorString())
      .arg(error)
      .arg(remainder),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::slotClientConnectionStateChanged(MCClientThread* client, MCClientThread::ConnectionState state) {
  AssertCheckPtr(client);

  QColor color = Qt::black;
  if (state == MCClientThread::ConnectedState)           { color = Qt::darkGreen; }
  else if (state == MCClientThread::AuthenticatingState) { color = Qt::blue; }
  else if (state == MCClientThread::UnconnectedState)    { color = Qt::red; }

  treeWidgetServerLog->write(
    MCServerLogWidget::InformationIcon,
    QString("Client %1 : %2 (%3)")
      .arg(client->peerAddressAndPort())
      .arg(MCClientThread::connectionStateToString(state))
      .arg(state),
    color, QFont::Bold
  );
}

void MCServerMainWindow::slotClientTimeout(MCClientThread* client, MCClientPeer::TimeoutNotify notifiedWhen) {
  treeWidgetServerLog->write(
    MCServerLogWidget::ErrorIcon,
    QString("The client %1 not responding (%2).")
      .arg(client->peerAddressAndPort())
      .arg(MCClientPeer::timeoutNotifyToString(notifiedWhen)),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::slotClientErrorProcessingPacket(
  MCClientThread* client, MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size,
  bool aborted
)
{
  treeWidgetServerLog->write(
    MCServerLogWidget::ErrorIcon,
    QString("Error processing a packet of the client %1.\n" \
            "(Type = %2, Size = %3) %4 (%5)\n" \
            "%6")
      .arg(client->peerAddressAndPort())
      .arg(type)
      .arg(size)
      .arg(MCClientPeer::packetErrorToString(error))
      .arg(error)
      .arg(
      (aborted == true)?
      "To prevent of a DoS attack, the connection with the client was aborted.":
      "Trying recover the packet."),
    Qt::red, QFont::Bold
  );
}

void MCServerMainWindow::closeEvent(QCloseEvent* event) {
  disconnectServer_(); // Forces to disconnect all clients
  event->accept();
}

bool MCServerMainWindow::connectServer_() {
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
      MCServerLogWidget::ErrorIcon,
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
