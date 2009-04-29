/****************************************************************************
 * @(#) Persistent platform-independent application settings.
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QIODevice>

class QMainWindow;
class QWidget;

class CSettings : public QSettings
{
  public:
    enum MethodWriteValue {NotEmptyValues, AllValues};

  public:
    static const Format XmlFormat;

  public:
    CSettings(
      const QString& fileName, const QString& folderName = QString("\\"),
      Scope scope = CSettings::UserScope,
      QObject* parent = NULL
    );

    ~CSettings();

    static void setMethodWriteValue(MethodWriteValue methodWriteValue) {m_enumMethodWriteValue = methodWriteValue;}
    static MethodWriteValue methodWriteValue() {return m_enumMethodWriteValue;}

    void saveLayout(const QWidget* widget);
    void saveLayout(const QMainWindow* window);

    bool loadLayout(QWidget* pwidget) const;
    bool loadLayout(QMainWindow* window) const;

  private:
    static bool readXmlFile_(QIODevice& device, SettingsMap& map);
    static bool writeXmlFile_(QIODevice& device, const SettingsMap& map);

  private:
    static MethodWriteValue m_enumMethodWriteValue;
};

#endif // SETTINGS_H
