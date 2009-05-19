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

void MCDialogPreferences::on_buttonBox_clicked(QAbstractButton* button) {
  AssertCheckPtr(button);

  QDialogButtonBox::StandardButton buttonType = buttonBox->standardButton(button);
  if (buttonType == QDialogButtonBox::RestoreDefaults) {
    int buttonSelected = QMessageBox::warning(
      NULL, QApplication::applicationName(),
      "The default values of the current tab will be reset.\n" \
      "Are you sure to continue ?",
      QMessageBox::Yes | QMessageBox::No
    );

    if (buttonSelected == QMessageBox::No) { return; }

    // Set default values for the current tab
    switch (tabWidget->currentIndex()) {
      case 0: defaultValuesTabConnection_(); break;
      case 1: defaultValuesTabProxy_(); break;
      case 2: defaultValuesTabAdvancedOptions_(); break;

      default:;
    }
  }
}

void MCDialogPreferences::readServerConnectionConfiguration_() {  
  textServerAddress->setText(MCServer::instance()->listenAddress().toString());
  spinBoxServerPort->setValue(MCServer::instance()->listenPort());
  spinBoxServerMaxConnections->setValue(MCServer::instance()->maxConnections());
}

void MCDialogPreferences::writeServerConnectionConfiguration_() {
  MCApp->saveSettingsServerConnection(
    textServerAddress->text(), spinBoxServerPort->value(),
    spinBoxServerMaxConnections->value()
  );
}

void MCDialogPreferences::readProxyConfiguration_() {
  const QNetworkProxy& proxy = MCServerApplication::proxy();

  checkBoxProxyUse->setChecked(proxy.type() != QNetworkProxy::NoProxy);
  textProxyHostName->setText(proxy.hostName());
  spinBoxProxyPort->setValue(proxy.port());
  textProxyUserName->setText(proxy.user());
  textProxyUserPassword->setText(proxy.password());
}

void MCDialogPreferences::writeProxyConfiguration_() {
  MCApp->saveSettingsProxyConfiguration(
    checkBoxProxyUse->isChecked(),
    textProxyHostName->text(), spinBoxProxyPort->value(),
    textProxyUserName->text(), textProxyUserPassword->text()
  );
}

void MCDialogPreferences::readAdvancedOptions_() {
  MCSettings->beginGroup("AdvancedOptions");
    checkBoxServerAutoConnectAtStartup->setChecked(
      MCSettings->value("ServerAutoConnectAtStartup", MCSettingsApplication::DefaultServerAutoConnectAtStartup).toBool()
    );
    checkBoxServerSaveHistory->setChecked(
      MCSettings->value("ServerSaveHistory", MCSettingsApplication::DefaultServerSaveHistory).toBool()
    );
  MCSettings->endGroup();
}

void MCDialogPreferences::writeAdvancedOptions_() {
  MCSettings->beginGroup("AdvancedOptions");
    MCSettings->setValue("ServerAutoConnectAtStartup", checkBoxServerAutoConnectAtStartup->isChecked());
    MCSettings->setValue("ServerSaveHistory", checkBoxServerSaveHistory->isChecked());
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

void MCDialogPreferences::defaultValuesTabConnection_() {
  textServerAddress->setText(MCSettingsApplication::DefaultServerAddress);
  spinBoxServerPort->setValue(MCSettingsApplication::DefaultServerPort);
  spinBoxServerMaxConnections->setValue(MCSettingsApplication::DefaultServerMaxConnections);
}

void MCDialogPreferences::defaultValuesTabProxy_() {
  checkBoxProxyUse->setChecked(MCSettingsApplication::DefaultProxyUse);
  textProxyHostName->setText(MCSettingsApplication::DefaultProxyHostName);
  spinBoxProxyPort->setValue(MCSettingsApplication::DefaultProxyPort);
  textProxyUserName->setText(MCSettingsApplication::DefaultProxyUserName);
  textProxyUserPassword->setText(MCSettingsApplication::DefaultProxyUserPassword);
}

void MCDialogPreferences::defaultValuesTabAdvancedOptions_() {
  checkBoxServerAutoConnectAtStartup->setChecked(MCSettingsApplication::DefaultServerAutoConnectAtStartup);
  checkBoxServerSaveHistory->setChecked(MCSettingsApplication::DefaultServerSaveHistory);
}
