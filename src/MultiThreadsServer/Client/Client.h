/****************************************************************************
 * @(#) Multi-threads client component.
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
 * RCSID $Id: template.h 18 2009-03-28 15:31:25Z geoffrey.anneheim $
 ****************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>

class CClient : public QTcpSocket
{
public:
    CClient(QObject* parent = NULL);

private slots:
    void establishServerCommunication_();

};

#endif // CLIENT_H
