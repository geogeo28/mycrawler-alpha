/****************************************************************************
 * @(#) Multi-threads client thread.
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

#ifndef MCCLIENTTHREAD_H
#define MCCLIENTTHREAD_H

#include <QThread>

class MCClientPeer;

class MCClientThread : public QThread
{
    Q_OBJECT

public:
    typedef enum {
      NoError,
      UnknownError,
      ClientPeerError
    } Error;

public:
    MCClientThread(int socketDescriptor, QObject* parent = NULL);

    Error error() const;
    QString errorString() const;

    MCClientPeer* clientPeer();

signals:
    void error(Error error);

protected:
    void run();

private:
    void setError_(Error error, bool signal = true);

private:
    Error m_enumError;
    QString m_sError;

    int m_nSocketDescriptor;
    MCClientPeer* m_pClientPeer;

};

#endif // MCCLIENTTHREAD_H
