#include <QMainWindow>
#include <QWidget>
#include <QTreeWidget>
#include <QHeaderView>

#include "Debug/Exception.h"
#include "Debug/Logger.h"

template <class T>
void CSettings::saveLayout(const T* obj, const QString& key, bool useLayoutPrefixKey) {
  AssertCheckPtr(obj);

  QString sLongKey = key;
  if ((useLayoutPrefixKey == true) && !m_sLayoutPrefixKey.isEmpty()) {
    sLongKey = m_sLayoutPrefixKey + "/" + sLongKey;
  }

  // State
  this->setValue(sLongKey + "/State", obj->saveState().toBase64());

  // Geometry
  const QWidget* widget = qobject_cast<const QWidget*>(obj);
  if (widget != NULL) {
    this->setValue(sLongKey + "/Geometry", widget->saveGeometry().toBase64());
  }
}

template <class T>
bool CSettings::loadLayout(T* obj, const QString& key, bool useLayoutPrefixKey) {
  AssertCheckPtr(obj);

  QString sLongKey = key;
  if ((useLayoutPrefixKey == true) && !m_sLayoutPrefixKey.isEmpty()) {
    sLongKey = m_sLayoutPrefixKey + "/" + sLongKey;
  }

  // State
  QByteArray data = QByteArray::fromBase64(this->value(sLongKey + "/State").toByteArray());
  if (data.isEmpty()) {
    ILogger::Notice() << "The setting 'State' of the object layout '" << key << "' doesn't exist.";
    return false;
  }

  obj->restoreState(data);

  // Geometry
  QWidget* widget = qobject_cast<QWidget*>(obj);
  if (widget != NULL) {
    data = QByteArray::fromBase64(this->value(sLongKey + "/Geometry").toByteArray());
    if (data.isEmpty()) {
      ILogger::Notice() <<  "The setting 'Geometry' of the widget layout '" << key << "' doesn't exist.";
      return false;
    }

    widget->restoreGeometry(data);
  }

  return true;
}

// Specialization
template <>
inline void CSettings::saveLayout(const QTreeWidget* tree, const QString& key, bool useLayoutPrefixKey) {
  this->saveLayout(tree->header(), key, useLayoutPrefixKey);
}

template <>
inline bool CSettings::loadLayout(QTreeWidget* tree, const QString& key, bool useLayoutPrefixKey) {
  return this->loadLayout(tree->header(), key, true);
}
