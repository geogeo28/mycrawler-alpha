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

#include "SettingsApplication.h"

const char* MCSettingsApplication::SettingTagCurrentForm = "CurrentForm";
const char* MCSettingsApplication::DefaultMainWindowForm = "doMainToolBarServers";

const char* MCSettingsApplication::ServersListFileName = "servers.lst";

const bool MCSettingsApplication::DefaultProxyUse = false;
const char* MCSettingsApplication::DefaultProxyHostName = "";
const quint16 MCSettingsApplication::DefaultProxyPort = 3128;
const char* MCSettingsApplication::DefaultProxyUserName = "";
const char* MCSettingsApplication::DefaultProxyUserPassword = "";

const bool MCSettingsApplication::DefaultServersConnectAtStartup = false;
const bool MCSettingsApplication::DefaultServersAutoReconnect = false;
