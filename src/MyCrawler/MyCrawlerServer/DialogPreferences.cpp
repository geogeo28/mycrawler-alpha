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

#include "Debug/Logger.h"
#include "DialogPreferences.h"
#include "ServerApplication.h"
#include "Server.h"

MCDialogPreferences::MCDialogPreferences(QWidget* parent)
  : QDialog(parent)
{
  setupUi(this);
}

int MCDialogPreferences::exec() {
  readSettings_();
  return QDialog::exec();
}

void MCDialogPreferences::accept() {
  writeSettings_();
  ILogger::Warning() << "Some parameters require restarting the application.";
  QDialog::accept();
}

void MCDialogPreferences::readServerConnectionConfiguration_() {  
  textServerAddress->setText(MCServer::instance()->listenAddress().toString());
  spinBoxServerPort->setValue(MCServer::instance()->listenPort());
  spinBoxServerNumberOfConnections->setValue(MCServer::instance()->maxConnections());
}

void MCDialogPreferences::writeServerConnectionConfiguration_() {
  MCApp->saveSettingsServerConnection(
    textServerAddress->text(), spinBoxServerPort->value(),
    spinBoxServerNumberOfConnections->value()
  );
}

void MCDialogPreferences::readProxyConfiguration_() {
  const QNetworkProxy& proxy = MCServerApplication::proxy();

  checkBoxProxyUse->setChecked(proxy.type() != QNetworkProxy::NoProxy);
  textProxyHostName->setText(proxy.hostName());
  spinBoxProxyHostPort->setValue(proxy.port());
  textProxyUserName->setText(proxy.user());
  textProxyUserPassword->setText(proxy.password());
}

void MCDialogPreferences::writeProxyConfiguration_() {
  MCApp->saveSettingsProxyConfiguration(
    checkBoxProxyUse->isChecked(),
    textProxyHostName->text(), spinBoxProxyHostPort->value(),
    textProxyUserName->text(), textProxyUserPassword->text()
  );
}

void MCDialogPreferences::readAdvancedOptions_() {
  MCSettings->beginGroup("AdvancedOptions");
    checkBoxAutoConnectServerAtStartup->setChecked(MCSettings->value("AutoConnectServerAtStartup", false).toBool());
    checkBoxSaveServerHistory->setChecked(MCSettings->value("SaveServerHistory", true).toBool());
  MCSettings->endGroup();
}

void MCDialogPreferences::writeAdvancedOptions_() {
  MCSettings->beginGroup("AdvancedOptions");
    MCSettings->setValue("AutoConnectServerAtStartup", checkBoxAutoConnectServerAtStartup->isChecked());
    MCSettings->setValue("SaveServerHistory", checkBoxSaveServerHistory->isChecked());
  MCSettings->endGroup();
}

void MCDialogPreferences::readSettings_() {
  readServerConnectionConfiguration_();
  readProxyConfiguration_();
  readAdvancedOptions_();
}

void MCDialogPreferences::writeSettings_() {
  writeServerConnectionConfiguration_();
  writeProxyConfiguration_();
  writeAdvancedOptions_();
}
