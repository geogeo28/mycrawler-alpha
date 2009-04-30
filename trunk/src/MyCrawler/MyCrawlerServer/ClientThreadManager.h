/****************************************************************************
 * @(#) Multi-threads client' threads manager.
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

#ifndef MCCLIENTTHREADMANAGER_H
#define MCCLIENTTHREADMANAGER_H

#include <QObject>
#include <QSet>

#include "ClientThread.h"

class MCClientThreadManager : public QObject
{
    Q_OBJECT

public:
    typedef enum {
      NoError,
      UnknownError,
      ManagerFull
    } Error;

public:
    static MCClientThreadManager* instance();

    MCClientThreadManager();
    virtual ~MCClientThreadManager();

    Error error() const;
    QString errorString() const;
    bool canCreateThread() const;

    int maxThreads() const;
    MCClientThread* createThread(int socketDescriptor);

signals:
    void error(Error error);
    void errorThread(MCClientThread* clientThread, MCClientThread::Error);
    void finishedThread(MCClientThread* clientThread);

private slots:
    void errorThread_(MCClientThread::Error error);
    void finishedThread_();

private:
    void setError_(Error error, bool signal = true);

private:
    Error m_enumError;
    QString m_sError;

    QSet<MCClientThread*> m_lstClientThreads;
};

#endif // MCCLIENTTHREADMANAGER_H
