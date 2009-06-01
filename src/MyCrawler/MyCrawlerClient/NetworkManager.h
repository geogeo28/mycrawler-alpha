/****************************************************************************
 * @(#) Network requests manager.
 * Source code of the crawler of IC05
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

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QtNetwork>
#include <QQueue>
#include <qsslconfiguration.h>

#include "UrlInfo.h"
#include "UrlsCollection.h"

#include "ClientApplication.h"

class CTransferRate;

class MCUrlsCollection;

class NetworkManagerThread {
  public:
    NetworkManagerThread(int id = -1, QNetworkReply* reply = NULL, MCUrlInfo urlInfo = MCUrlInfo());

    int id() const {return m_nId;}
    void setId(int id) {m_nId = id;}
    QNetworkReply* reply() const {return m_pReply;}
    void setReply(QNetworkReply* reply) {m_pReply = reply;} // Utile seulement lors d'une redirection
    int count() const {return m_nCount;}
    bool inProcess() const {return m_bInProcess;}

    void start(QNetworkReply* reply, MCUrlInfo urlInfo);
    void end();

  private:
    bool m_bInProcess;
    int m_nId;
    int m_nCount;
    QNetworkReply* m_pReply;

    MCUrlInfo m_urlInfo;
};

Q_DECLARE_METATYPE(NetworkManagerThread*)

// Gérer Timeout
class CNetworkManager : public QObject
{
  Q_OBJECT

  public:
    typedef QList<QNetworkReply*> RepliesList;

  // Wrapper
  public:
    QNetworkProxy proxy() const {return m_pNetworkManager->proxy();}
    void setProxy(const QNetworkProxy& proxy) {m_pNetworkManager->setProxy(proxy);}

  public:
    CNetworkManager(MCUrlsCollection& queueOfPendingRequest, int threads = 5, QObject* parent = NULL);
    virtual ~CNetworkManager();

    QNetworkRequest& baseRequest() {return m_baseRequest;}
    void setBaseRequest(const QNetworkRequest& baseRequest) {m_baseRequest = baseRequest;}
    const RepliesList& replies() const {return m_lstReplies;}
    //QNetworkReply* doRequest(const QUrl& url);

    static CTransferRate* transferRateManager(QNetworkReply* reply);

    int numberOfThreads() const {return m_nThreads;}
    //bool addPendingRequest(const QUrl& url);
    bool processingPendingRequests() const {return m_bProcessingPendingRequests;}
    //void clearPendingRequests() {m_lstPendingRequests.clear();}
    //const QList<QUrl>& pendingRequests() const {return m_lstPendingRequests;}
    bool hasPendingRequests() const {return !m_lstPendingRequests.isEmpty();}
    const NetworkManagerThread* thread(int thread) const;
    const NetworkManagerThread* thread(const QNetworkReply* reply) const;
    static int threadId(const QNetworkReply* reply) {QVariant data = reply->property("ThreadId"); return (data.isValid()?data.toInt():-1);}

  public slots:
    void setProcessingPendingRequests(bool processing);
    void abortAll();

  signals:
    void authenticationRequired(QAuthenticator* authenticator, const NetworkManagerThread* thread);
    void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator* authenticator);
    void sslErrors(const QList<QSslError>& errors, const NetworkManagerThread* thread);
    void started(const NetworkManagerThread* thread);
    void responseHeaderReceived(int statusCode, const NetworkManagerThread* thread);
    void redirection(QUrl urlRedirection, const NetworkManagerThread* thread);
    void downloadStarted(const NetworkManagerThread* thread);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, const NetworkManagerThread* thread);
    void requestError(QNetworkReply::NetworkError error, const NetworkManagerThread* thread);
    void finished(const NetworkManagerThread* thread);
    void success(const NetworkManagerThread* thread);
    void allDone();
    void transferRateUpdated(const CTransferRate* transferRateManager, const NetworkManagerThread* thread);


  private slots:
    void slotAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
    void slotSslErrors(QNetworkReply* reply, const QList<QSslError>& errors);
    void slotNetworkReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotNetworkReplyError(QNetworkReply::NetworkError error);
    void slotNetworkReplyFinished(QNetworkReply* reply);
    void slotTransferRateUpdated();

    void slotAddUrl_(MCUrlInfo urlInfo);

  private:
    NetworkManagerThread* thread_(const QNetworkReply* reply) {return qVariantValue<NetworkManagerThread*>(reply->property("Thread"));}
    NetworkManagerThread* thread_(int thread) {Q_ASSERT((thread>=0) && (thread < numberOfThreads())); return &m_lstThreads[thread];}
    QNetworkReply* doRequest_(const QUrl& url);
    int threadInWaiting_() const;
    void nextPendingRequest_(int thread = -1);


  private:
    QPointer<QNetworkAccessManager> m_pNetworkManager;
    QNetworkRequest m_baseRequest;
    RepliesList m_lstReplies;

    int m_nThreads;
    //QQueue<QUrl> m_lstPendingRequests;
    NetworkManagerThread* m_lstThreads;
    bool m_bProcessingPendingRequests;

    MCUrlsCollection& m_lstPendingRequests;

  private:
    CNetworkManager(const CNetworkManager& other);
    CNetworkManager& operator=(const CNetworkManager& other);
};

#endif
