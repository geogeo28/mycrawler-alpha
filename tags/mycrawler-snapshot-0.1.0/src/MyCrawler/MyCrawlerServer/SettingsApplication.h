/****************************************************************************
 * @(#) Application default parameters.
 *
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

#ifndef SETTINGSAPPLICATION_H
#define SETTINGSAPPLICATION_H

struct MCSettingsApplication
{
  static const char* SettingTagCurrentForm;
  static const char* DefaultMainWindowForm;

  static const char* ServerHistoryFileName;

  static const char* DefaultServerAddress;
  static const quint16 DefaultServerPort;
  static const char* DefaultServerName;
  static const int DefaultServerMaxConnections;

  static const bool DefaultProxyUse;
  static const char* DefaultProxyHostName;
  static const quint16 DefaultProxyPort;
  static const char* DefaultProxyUserName;
  static const char* DefaultProxyUserPassword;

  static const bool DefaultServerAutoConnectAtStartup;
  static const bool DefaultServerSaveHistory;
};

#endif // SETTINGSAPPLICATION_H
