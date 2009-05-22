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
#include "ClientApplication.h"

MCDialogPreferences::MCDialogPreferences(QWidget* parent)
  : QDialog(parent)
{
  setupUi(this);
  tabWidget->setCurrentIndex(0);
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
      case 0: defaultValuesTabProxy_(); break;
      case 1: defaultValuesTabAdvancedOptions_(); break;

      default:;
    }
  }
}

void MCDialogPreferences::readProxyConfiguration_() {
  const QNetworkProxy& proxy = MCClientApplication::proxy();

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
    checkBoxServersConnectAtStartup->setChecked(
      MCSettings->value("ServersConnectAtStartup", MCSettingsApplication::DefaultServersConnectAtStartup).toBool()
    );
    checkBoxServersAutoReconnect->setChecked(
      MCSettings->value("ServersAutoReconnect", MCSettingsApplication::DefaultServersAutoReconnect).toBool()
    );
  MCSettings->endGroup();
}

void MCDialogPreferences::writeAdvancedOptions_() {
  MCSettings->beginGroup("AdvancedOptions");
    MCSettings->setValue("ServersConnectAtStartup", checkBoxServersConnectAtStartup->isChecked());
    MCSettings->setValue("ServersAutoReconnect", checkBoxServersAutoReconnect->isChecked());
  MCSettings->endGroup();
}

void MCDialogPreferences::readSettings_() {
  readProxyConfiguration_();
  readAdvancedOptions_();
}

void MCDialogPreferences::writeSettings_() {
  writeProxyConfiguration_();
  writeAdvancedOptions_();
}

void MCDialogPreferences::defaultValuesTabProxy_() {
  checkBoxProxyUse->setChecked(MCSettingsApplication::DefaultProxyUse);
  textProxyHostName->setText(MCSettingsApplication::DefaultProxyHostName);
  spinBoxProxyPort->setValue(MCSettingsApplication::DefaultProxyPort);
  textProxyUserName->setText(MCSettingsApplication::DefaultProxyUserName);
  textProxyUserPassword->setText(MCSettingsApplication::DefaultProxyUserPassword);
}

void MCDialogPreferences::defaultValuesTabAdvancedOptions_() {
  checkBoxServersConnectAtStartup->setChecked(MCSettingsApplication::DefaultServersConnectAtStartup);
  checkBoxServersAutoReconnect->setChecked(MCSettingsApplication::DefaultServersAutoReconnect);
}
