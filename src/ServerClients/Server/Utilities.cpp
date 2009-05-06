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

#include "Utilities.h"

namespace Utilities {
  QString socketStateToString(QAbstractSocket::SocketState state) {
    static QString s_strSocketStates[] = {
      QObject::tr("Unconnected"),
      QObject::tr("Host Lookup"),
      QObject::tr("Connecting ..."),
      QObject::tr("Connected"),
      QObject::tr("Bound"),
      QObject::tr("Listening ..."),
      QObject::tr("Closing ...")
    };

    Q_ASSERT((state >= QAbstractSocket::UnconnectedState) && (state <= QAbstractSocket::ClosingState));

    return s_strSocketStates[state];
  }
}
