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
class QHeaderView;

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

    template <class T> void saveLayout(const T* obj, const QString& keyName = QString());
    template <class T> bool loadLayout(T* obj, const QString& keyName = QString()) const;

  private:
    static bool readXmlFile_(QIODevice& device, SettingsMap& map);
    static bool writeXmlFile_(QIODevice& device, const SettingsMap& map);

  private:
    static MethodWriteValue m_enumMethodWriteValue;
};

template <class T>
void CSettings::saveLayout(const T* obj, const QString& keyName) {
  AssertCheckPtr(obj);

  QString key = keyName;
  if (key.isEmpty()) { key = obj->objectName(); }

  // State
  this->setValue(key + "/State", obj->saveState().toBase64());

  // Geometry
  const QWidget* widget = qobject_cast<const QWidget*>(obj);
  if (widget != NULL) {
    this->setValue(key + "/Geometry", widget->saveGeometry().toBase64());
  }
}

template <class T>
bool CSettings::loadLayout(T* obj, const QString& keyName) const {
  AssertCheckPtr(obj);

  QString key = keyName;
  if (key.isEmpty()) { key = obj->objectName(); }

  // State
  QByteArray data = QByteArray::fromBase64(this->value(key + "/State").toByteArray());
  if (data.isEmpty()) {
    ILogger::Notice() << "The setting 'State' of the object layout '" << keyName << "' doesn't exist.";
    return false;
  }

  obj->restoreState(data);

  // Geometry
  QWidget* widget = qobject_cast<QWidget*>(obj);
  if (widget != NULL) {
    data = QByteArray::fromBase64(this->value(key + "/Geometry").toByteArray());
    if (data.isEmpty()) {
      ILogger::Notice() <<  "The setting 'Geometry' of the widget layout '" << keyName << "' doesn't exist.";
      return false;
    }

    widget->restoreGeometry(data);
  }

  return true;
}

#endif // SETTINGS_H
