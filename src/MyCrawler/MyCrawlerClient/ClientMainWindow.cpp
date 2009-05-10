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
#include "Client.h"
#include "ClientPeer.h"

void MCClientMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  //setAttribute(Qt::WA_DeleteOnClose, true);

  // Set title of the window
  setWindowTitle(MCClientApplication::applicationName() + " v" + _MYCRAWLER_CLIENT_VERSION_);

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

void MCClientMainWindow::setupComponents_() {
  QObject::connect(MCClient::instance(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotClientError(QAbstractSocket::SocketError)));
  QObject::connect(MCClient::instance(), SIGNAL(connectionStateChanged(QAbstractSocket::SocketState)), this, SLOT(slotClientConnectionStateChanged(QAbstractSocket::SocketState)));
  QObject::connect(MCClient::instance(), SIGNAL(timeout(MCClientPeer::TimeoutNotify)), this, SLOT(slotClientTimeout(MCClientPeer::TimeoutNotify)));
  QObject::connect(MCClient::instance(), SIGNAL(errorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)), this, SLOT(slotClientErrorProcessingPacket(MCClientPeer::PacketError,MCClientPeer::PacketType,quint32,bool)));
}

void MCClientMainWindow::cleanAll_() {

}

void MCClientMainWindow::closeWindow_() {
  ILogger::Debug() << "Save the setting window layout.";
  MCApp->settings()->saveLayout(this);
}

MCClientMainWindow::MCClientMainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_bClientConnected(false)
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

void MCClientMainWindow::on_buttonClientConnect_clicked() {
  // Connect the client
  if (m_bClientConnected == false) {
    buttonClientConnect->setText("Connecting...");

    QString address = textServerAddress->text();
    quint16 port = textServerPort->text().toUShort();

    if (MCClient::instance()->state() != MCClientPeer::UnconnectedState) {
      int button = QMessageBox::warning(
        this, QApplication::applicationName(),
        "Trying to connect to server.\n" \
        "Do you want interrupt this connection ?",
        QMessageBox::Yes | QMessageBox::No
      );

      if (button == QMessageBox::Yes) {
        buttonClientConnect->setText("Disconnecting...");
        if (MCClient::instance()->state() != MCClientPeer::ClosingState) {
          MCClient::instance()->disconnect(-1);
        }
      }

      return;
    }
    MCClient::instance()->connectToHost(address, port);
  }
  // Disconnect the client
  else {
    buttonClientConnect->setText("Disconnecting...");
    if (MCClient::instance()->state() != MCClientPeer::ClosingState) {
      MCClient::instance()->disconnect(-1);
    }
  }
}

void MCClientMainWindow::slotClientError(QAbstractSocket::SocketError error) {
  ILogger::Error() << MCClient::instance()->errorString();
}

void MCClientMainWindow::slotClientConnectionStateChanged(QAbstractSocket::SocketState state) {
  ILogger::Trace() << QString("Connection state changed : %1 (%2)")
                      .arg(MCClientPeer::stateToString(state))
                      .arg(state);

  // Client connected
  if (state == QAbstractSocket::ConnectedState) {
    buttonClientConnect->setText("Disconnect");
    m_bClientConnected = true;
  }
  // Client disconnected
  else if (state == QAbstractSocket::UnconnectedState) {
    buttonClientConnect->setText("Connect");
    m_bClientConnected = false;
  }
}

void MCClientMainWindow::slotClientTimeout(MCClientPeer::TimeoutNotify notifiedWhen) {
  ILogger::Error() << QString(
      "The server not responding (%1).\n" \
      "Check your Internet connection.")
      .arg(MCClientPeer::timeoutNotifyToString(notifiedWhen));
}

void MCClientMainWindow::slotClientErrorProcessingPacket(MCClientPeer::PacketError error, MCClientPeer::PacketType type, quint32 size, bool aborted) {
  ILogger::Error() << QString("Error processing a packet of the server.\n" \
                              "(Type = %1, Size = %2) %3 (%4) \n" \
                              "%5")
                      .arg(type)
                      .arg(size)
                      .arg(MCClientPeer::packetErrorToString(error))
                      .arg(error)
                      .arg(
                        (aborted == true)?
                        "To prevent of a DoS attack, the connection was aborted.":
                        "Trying recover the packet.");
}
