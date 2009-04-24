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

#include "ServerMainWindow.h"
#include "ui_ServerMainWindow.h"

#include "MCException.h"

#include "Utilities.h"

void ServerMainWindow::setupWindow_() {
  // Destroy window in memory when the user clicks on the close button
  // setAttribute(Qt::WA_DeleteOnClose, true);
}

void ServerMainWindow::setupComponents_() {
  m_pServer = new CServer(this);

  QObject::connect(
    m_pServer, SIGNAL(newClientConnection(QAbstractSocket*)),
    this, SLOT(slotClientNewConnection(QAbstractSocket*))
  );
}

void ServerMainWindow::cleanAll_() {
  if (m_pServer!=NULL) {stopServerConnection_();}
}

void ServerMainWindow::closeWindow_() {
  deleteLater();
}

ServerMainWindow::ServerMainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_pServer(NULL)
{
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

ServerMainWindow::~ServerMainWindow()
{
  closeWindow_();
  cleanAll_();
}

void ServerMainWindow::on_buttonServerListen_clicked() {
  // The server is off
  if (m_pServer->isListening() == false) {
    // Server started to listening connections
    if (startServerConnection_() == true) {
      QMessageBox::information(
        this, QApplication::applicationName(),
        tr("Server started to listening connection on %1:%2")
          .arg(m_pServer->serverAddress().toString())
          .arg(m_pServer->serverPort())
      );
    }
    // Could not start the server
    else {
      QMessageBox::critical(
        this, QApplication::applicationName(),
        tr("Unable to start the server: %1.").arg(m_pServer->errorString())
      );

      m_pServer->close();
      return;
    }

    buttonServerListen->setText(tr("Disconnect"));
  }
  // Else, the server listens connections
  else {
    stopServerConnection_();
    buttonServerListen->setText(tr("Connect"));
  }
}

void ServerMainWindow::slotClientNewConnection(QAbstractSocket* clientSocket) {
  Q_CHECK_PTR(clientSocket);

  QTreeWidgetItem* item = new QTreeWidgetItem(treeWidgetClientsConnected);

  item->setText(0, clientSocket->peerName());
  item->setText(1, clientSocket->peerAddress().toString());
  item->setText(2, QString::number(clientSocket->peerPort()));
  item->setText(3, Utilities::socketStateToString(clientSocket->state()));

  treeWidgetClientsConnected->insertTopLevelItem(0, item);

  // Stores the line of information into the client's socket
  clientSocket->setProperty("TreeWidgetItemInfo", qVariantFromValue(item));

  QObject::connect(
    clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
    this, SLOT(slotClientStateChanged(QAbstractSocket::SocketState))
  );
}

void ServerMainWindow::slotClientStateChanged(QAbstractSocket::SocketState state) {
  // Gets line of information stored into the client's socket
  QTreeWidgetItem* item = qVariantValue<QTreeWidgetItem*>(QObject::sender()->property("TreeWidgetItemInfo"));
  Q_CHECK_PTR(item);

  // Client disconnected (remove line of information)
  if (state == QAbstractSocket::UnconnectedState) {
    delete item;
  }
  // Else, update the state
  else {
    item->setText(3, Utilities::socketStateToString(state));
  }
}

bool ServerMainWindow::startServerConnection_() {
  if (m_pServer->isListening()==false) {
    return m_pServer->listen(QHostAddress(textServerAddress->text()), textServerPort->text().toUShort());
  }

  return false;
}

void ServerMainWindow::stopServerConnection_() {
  m_pServer->close();
}
