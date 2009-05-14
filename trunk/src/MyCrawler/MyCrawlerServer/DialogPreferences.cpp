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
  MCSettings->beginGroup("ServerConnectionConfiguration");
    textServerAddress->setText(MCSettings->value("Address", "").toString());
    spinBoxServerPort->setValue(MCSettings->value("Port", 8080).toUInt());
    checkBoxServerAutoConnect->setChecked(MCSettings->value("AutoConnect", false).toBool());

    spinBoxServerNumberOfConnections->setValue(MCSettings->value("MaxConnections", MCServer::defaultMaxConnections()).toInt());
  MCSettings->endGroup();
}

void MCDialogPreferences::readProxyConfiguration_() {
  MCSettings->beginGroup("ProxyConfiguration");
    checkBoxProxyUse->setChecked(MCSettings->value("Use", false).toBool());
    textProxyHostName->setText(MCSettings->value("HostName").toString());
    spinBoxProxyHostPort->setValue(MCSettings->value("Port", "3128").toInt());
    textProxyUserName->setText(MCSettings->value("UserName").toString());
    textProxyUserPassword->setText(QByteArray::fromBase64(MCSettings->value("UserPassword").toByteArray()));
  MCApp->settings()->endGroup();
}

void MCDialogPreferences::writeServerConnectionConfiguration_() {
  MCSettings->beginGroup("ServerConnectionConfiguration");
    MCSettings->setValue("Address", textServerAddress->text());
    MCSettings->setValue("Port", spinBoxServerPort->value());
    MCSettings->setValue("AutoConnect", checkBoxServerAutoConnect->isChecked());

    MCSettings->setValue("MaxConnections", spinBoxServerNumberOfConnections->value());
  MCSettings->endGroup();
}

void MCDialogPreferences::writeProxyConfiguration_() {
  MCSettings->beginGroup("ProxyConfiguration");
    MCSettings->setValue("Use", checkBoxProxyUse->isChecked());
    MCSettings->setValue("HostName", textProxyHostName->text());
    MCSettings->setValue("Port", spinBoxProxyHostPort->value());
    MCSettings->setValue("UserName", textProxyUserName->text());
    MCSettings->setValue("UserPassword", textProxyUserPassword->text().toUtf8().toBase64());
  MCSettings->endGroup();
}

void MCDialogPreferences::readSettings_() {
  readServerConnectionConfiguration_();
  readProxyConfiguration_();
}

void MCDialogPreferences::writeSettings_() {
  writeServerConnectionConfiguration_();
  writeProxyConfiguration_();
}
