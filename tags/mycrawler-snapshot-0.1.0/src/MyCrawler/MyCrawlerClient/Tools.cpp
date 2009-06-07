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

#include <QLocale>
#include <QUrl>

#include "Tools.h"

namespace Tools {
  double convertSize(qint64 size, FormatSize format, FormatSize* formatOut) {
    double result = size;

    switch (format) {
      case Bytes:     return result;
      case Kilobytes: return (result / 1024.);
      case Megabytes: return (result / 1048576.);
      case Gigabytes: return (result / 1073741824.);

      default:
        FormatSize tmpFormat;
        if (size < 1023)                  tmpFormat = Bytes;
        else if (size < (1024 * 1023))    tmpFormat = Kilobytes;
        else if (size < (1048576 * 1023)) tmpFormat = Megabytes;
        else                              tmpFormat = Gigabytes;

        if (formatOut) *formatOut = tmpFormat;
        return convertSize(size, tmpFormat);
    }
  }

  QString sizePrefixString(FormatSize format) {
    switch (format) {
      case Bytes:     return QString("octets");
      case Kilobytes: return QString("Ko");
      case Megabytes: return QString("Mo");
      case Gigabytes: return QString("Go");
      default:
        assert(false);
    }
  }

  QString sizeToString(qint64 size, FormatSize format, int precision) {
    double sizeConverted = convertSize(size, format, &format);
    if (precision==-1)
      precision = (format>=Megabytes)?2:0;

    return QLocale::c().toString(sizeConverted, 'f', precision) + " " + sizePrefixString(format);
  }

  QUrl decodeUrl(const QUrl& url) {
    QUrl tmpUrl = QUrl::fromEncoded(url.toString().toAscii());
    return QUrl(QUrl::fromPercentEncoding(tmpUrl.toString().toAscii()));
  }

  QUrl absoluteUrl(const QString& base, const QString& relative) {
    return QUrl(base).resolved(QUrl(relative));
  }
}
