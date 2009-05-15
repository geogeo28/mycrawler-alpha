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

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QMainWindow>
#include <QWidget>
#include <QMessageBox>

#include "Config/Settings.h"
#include "Debug/Exception.h"
#include "Debug/Logger.h"

CSettings::MethodWriteValue CSettings::m_enumMethodWriteValue = CSettings::AllValues;

const CSettings::Format CSettings::XmlFormat = CSettings::registerFormat("xml", CSettings::readXmlFile_, CSettings::writeXmlFile_);

bool CSettings::readXmlFile_(QIODevice& device, SettingsMap& map) {
  QXmlStreamReader xmlReader(&device);
  QStringList currentElementName;

  // Until the end of file
  while (!xmlReader.atEnd()) {
    xmlReader.readNext();

    // A section "<mysection>" exists
    while (xmlReader.isStartElement()) {
      const QString element = xmlReader.name().toString();

      // A section SettingsMap was found
      if (element == "SettingsMap") {
         xmlReader.readNext();
         continue;
      }

      int iSeparator = element.indexOf('.');

      if (iSeparator == -1) {
        currentElementName.append(element);
      }
      // Manage arrays
      else {
        currentElementName.append(element.right(element.length() - iSeparator - 1));
        currentElementName.append(element.left(iSeparator));
      }
      xmlReader.readNext();
    }

    // End of a section "</mysection>"
    if (xmlReader.isEndElement()) {
      const QString element = xmlReader.name().toString();
      if (element != "SettingsMap") {
        currentElementName.removeLast();

        // It's an array. Remove /n/key
        if (element.indexOf('.')!=-1) {
          currentElementName.removeLast();
        }
      }
      continue;
    }

    // If it's a content of a valid section
    if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
      // Pair <key, QVariant>
      QString key = currentElementName.join("/");
      QString value = xmlReader.text().toString();
      map[key] = value;
    }
  }

  // Syntax error in the XML file
  if (xmlReader.hasError()) {
    ILogger::Error() << QString("Cannot open an XML file of settings.\n%1\nLine %2. Column %3.")
                        .arg(xmlReader.errorString())
                        .arg(xmlReader.lineNumber())
                        .arg(xmlReader.columnNumber());
    return false;
  }

  return true;
}

bool CSettings::writeXmlFile_(QIODevice& device, const SettingsMap& map) {
  QXmlStreamWriter xmlWriter(&device);
  xmlWriter.setAutoFormatting(true); // Format content of XML file (automatic indentation)
  xmlWriter.writeStartDocument(); // XML header
  xmlWriter.writeStartElement("SettingsMap"); // Root section

  QStringList treeGroups;

  // For each parameter
  for (
    SettingsMap::const_iterator mi = map.constBegin();
    mi != map.constEnd();
    ++mi
  )
  {
    QStringList groups(mi.key().split("/")); // Extract groups

    // Manage arrays
    for (
      QStringList::iterator it = groups.begin();
      it!=groups.end();
      ++it
    )
    {
      bool bIntOk;
      QVariant value;
      value.setValue(it->toInt(&bIntOk));
      if (bIntOk) {
        it[1].append('.').append(value.toString());
        groups.erase(it);
      }
    }

    int i;
    int depth = 0;
    for (; (depth < treeGroups.count()) && (depth < (groups.count() - 1)); ++depth) {
      if (treeGroups[depth]!=groups[depth])
        break;
    }

    // Close n sections
    for (i = treeGroups.count() - 1; i>=depth; --i) {
      xmlWriter.writeEndElement();
      treeGroups.pop_back(); // Go up the hierarchy
    }

    // Open n sections of groups
    for (i = depth; i < (groups.count() - 1); ++i) {
      xmlWriter.writeStartElement(groups[i]);
      treeGroups.append(groups[i]); // Go down the hierarchy
    }

    // Write an element
    const QString sValue = mi.value().toString();
    if (
      (CSettings::methodWriteValue() == AllValues) ||
      ((CSettings::methodWriteValue() == NotEmptyValues) && (!sValue.isEmpty()))
    )
    {
      xmlWriter.writeStartElement(groups.last());
      xmlWriter.writeCharacters(sValue);
      xmlWriter.writeEndElement();
    }
  }

  // Close the hierarchy
  for (int i = 0; i < treeGroups.count(); ++i)
    xmlWriter.writeEndElement();

  xmlWriter.writeEndElement(); // Close the root section
  xmlWriter.writeEndDocument(); // Document "footer"

  return true;
}

CSettings::CSettings(
  const QString& fileName, const QString& folderName,
  Scope scope,
  QObject* parent
)
  : QSettings(XmlFormat, scope, folderName, fileName, parent)
{}

CSettings::~CSettings()
{}

/*void CSettings::saveLayout(const QWidget* widget) {
  AssertCheckPtr(widget);

  this->setValue(widget->objectName() + "/Geometry", widget->saveGeometry().toBase64());
}

bool CSettings::loadLayout(QWidget* widget) const {
  AssertCheckPtr(widget);

  QByteArray tmpArray;
  tmpArray = QByteArray::fromBase64(this->value(widget->objectName() + "/Geometry").toByteArray());
  if (tmpArray.isEmpty()) {
    ILogger::Notice() <<  "The setting 'Geometry' of the widget layout '" << widget->objectName() << "' doesn't exist.";
    return false;
  }

  widget->restoreGeometry(tmpArray);
  return true;
}

void CSettings::saveLayout(const QMainWindow* window) {
  AssertCheckPtr(window);

  this->saveLayout(qobject_cast<const QWidget*>(window));
  this->setValue(window->objectName() + "/State", window->saveState().toBase64());
}

bool CSettings::loadLayout(QMainWindow* window) const {
  AssertCheckPtr(window);

  if (!this->loadLayout(qobject_cast<QWidget*>(window))) {
    return false;
  }

  QByteArray tmpArray;
  tmpArray = QByteArray::fromBase64(this->value(window->objectName() + "/State").toByteArray());
  if (tmpArray.isEmpty()) {
    ILogger::Notice() << "The setting 'State' of the main window layout'" << window->objectName() << "' doesn't exist.";
    return false;
  }

  window->restoreState(tmpArray);
  return true;
}*/
