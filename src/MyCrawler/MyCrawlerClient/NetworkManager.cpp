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

#include "NetworkManager.h"
#include "TransferRate.h"

#include "Debug/Exception.h"

#include "UrlsCollection.h"

NetworkManagerThread::NetworkManagerThread(int id, QNetworkReply* reply, MCUrlInfo urlInfo)
  : m_bInProcess(false), m_nId(id), m_nCount(0), m_pReply(NULL), m_urlInfo(urlInfo)
{
  if (reply) {
    Assert(urlInfo.isValid() == true);
    start(reply, urlInfo);
  }
}

void NetworkManagerThread::start(QNetworkReply* reply, MCUrlInfo urlInfo) {
  Q_CHECK_PTR(reply);
  Assert(urlInfo.isValid() == true);
  Q_ASSERT(m_bInProcess==false);

  m_pReply = reply;
  m_urlInfo = urlInfo;
  m_bInProcess = true;
  ++m_nCount;
}

void NetworkManagerThread::end() {
  Q_ASSERT(m_bInProcess==true);
  m_bInProcess = false;
}

CNetworkManager::CNetworkManager(MCUrlsCollection& queueOfPendingRequest, int threads, QObject* parent)
  : QObject(parent),
    m_nThreads(threads), m_lstThreads(NULL),
    m_bProcessingPendingRequests(false),
    m_lstPendingRequests(queueOfPendingRequest)
{
  Q_ASSERT(threads>=0);

  // Liste des threads
  if (threads) {
    m_lstThreads = new NetworkManagerThread[threads];
    for (int i = numberOfThreads() - 1; i>=0; --i)
      m_lstThreads[i].setId(i);
  }

  m_pNetworkManager = new QNetworkAccessManager(parent);

  #ifndef QT_NO_OPENSSL
    m_baseRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  #endif
  m_baseRequest.setRawHeader("Accept", "text/html");
  m_baseRequest.setRawHeader("Connection", "Keep-Alive");
  m_baseRequest.setRawHeader("User-agent", "unknown");

  QObject::connect(
    m_pNetworkManager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
    this, SLOT(slotAuthenticationRequired(QNetworkReply*, QAuthenticator*))
  );
  QObject::connect(
    m_pNetworkManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
    this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*))
  );
  QObject::connect(
    m_pNetworkManager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)),
    this, SLOT(slotSslErrors(QNetworkReply*, const QList<QSslError>&))
  );
  QObject::connect(
    m_pNetworkManager, SIGNAL(finished(QNetworkReply*)),
    this, SLOT(slotNetworkReplyFinished(QNetworkReply*))
  );
}

CNetworkManager::~CNetworkManager() {
  if (m_lstThreads) delete[] m_lstThreads;
}


/*QNetworkReply* CNetworkManager::doRequest(const QUrl& url) {
  QNetworkReply* reply = doRequest_(url);
  reply->setProperty("Thread", qVariantFromValue(new NetworkManagerThread(-1, reply)));
  reply->setProperty("ThreadId", -1);
  reply->setProperty("BaseUrl", url);

  emit started(thread_(reply));
  return reply;
}*/

void CNetworkManager::abortAll() {
  foreach (QNetworkReply* reply, m_lstReplies)
    reply->abort();
}

CTransferRate* CNetworkManager::transferRateManager(QNetworkReply* reply) {
  return reply->findChild<CTransferRate*>();
}

/*bool CNetworkManager::addPendingRequest(const QUrl& url) {
  Q_ASSERT(numberOfThreads());

  if (m_lstPendingRequests.contains(url))
    return false;

  m_lstPendingRequests.enqueue(url);

  return true;
}*/

void CNetworkManager::setProcessingPendingRequests(bool processing) {
  //Q_ASSERT(m_bProcessingPendingRequests!=processing);
  m_bProcessingPendingRequests = processing;

  if (processing==true) {
    Q_ASSERT(!m_lstPendingRequests.isEmpty());

    // Distribue les requêtes de la file dans les threads en attente
    for (int i = 0; (i < numberOfThreads()) && !m_lstPendingRequests.isEmpty(); ++i) {
      if (m_lstThreads[i].inProcess()==false)
        nextPendingRequest_(i);
    }
  }
}

const NetworkManagerThread* CNetworkManager::thread(int thread) const {return const_cast<CNetworkManager*>(this)->thread_(thread);}
const NetworkManagerThread* CNetworkManager::thread(const QNetworkReply* reply) const {return const_cast<CNetworkManager*>(this)->thread_(reply);}

void CNetworkManager::slotAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator) {
  qDebug() << "NetworkManager authentication required";
  emit authenticationRequired(authenticator, thread_(reply));
}

void CNetworkManager::slotSslErrors(QNetworkReply* reply, const QList<QSslError>& errors) {
  qDebug() << "NetworkManager ssl errors";
  emit sslErrors(errors, thread_(reply));
}

void CNetworkManager::slotNetworkReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
  //qDebug() << "NetworkManager download progress";
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(this->sender());
  //transferRateManager(reply)->start();

  // Une erreur s'est produite, annuler emission signaux
  if (reply->error())
    return;

  if (!reply->property("Downloading").isValid()) {
    reply->setProperty("Downloading", -1);

    // Header response
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    emit responseHeaderReceived(statusCode, thread_(reply));
    if ((reply->error()==QNetworkReply::OperationCanceledError) || (statusCode>=400))
      return;

    // Gestion redirection
    QUrl urlRedirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!urlRedirection.isEmpty()) {
      urlRedirection = reply->url().resolved(urlRedirection);     

      // Si redirection différente
      if (urlRedirection!=reply->url()) {        
        // Only the HTTP protocol is supported
        if (urlRedirection.scheme().toLower() == "http") {
          emit redirection(urlRedirection, thread_(reply));
          m_lstReplies.removeOne(reply);
          reply->abort(); // On annule la requête en cours

          return;
        }
      }
    }

    emit downloadStarted(thread_(reply));
  }

  emit downloadProgress(bytesReceived, bytesTotal, thread_(reply));
}

void CNetworkManager::slotNetworkReplyError(QNetworkReply::NetworkError error) {
  qDebug() << "NetworkManager error" << error;

  const QNetworkReply* reply = qobject_cast<QNetworkReply*>(this->sender());
  emit requestError(error, thread_(reply));
}

void CNetworkManager::slotNetworkReplyFinished(QNetworkReply* reply) {
  qDebug() << "NetworkManager finished" << reply->url();

  // TODO : Bug Qt 4.4.0
  if (!reply->property("Thread").isValid()) {
    reply->readAll();
    reply->close();
    reply->deleteLater();
    return;
  }

  CTransferRate* transferRate = CNetworkManager::transferRateManager(reply);
  transferRate->stop();

  NetworkManagerThread* pThread = thread_(reply);

  // Correspond à une requête de redirection
  QUrl urlRedirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  m_lstReplies.removeOne(reply);
  if (!m_lstReplies.removeOne(reply) && urlRedirection.isValid()) {
    urlRedirection = reply->url().resolved(urlRedirection);

    // Only the HTTP protocol is supported
    if (urlRedirection.scheme().toLower() == "http") {
      QNetworkReply* newReply = this->doRequest_(urlRedirection);

      // Copie les propriétés
      foreach (const QByteArray& propertyName, reply->dynamicPropertyNames())
        newReply->setProperty(propertyName, reply->property(propertyName));
      pThread->setReply(newReply);

      reply->setProperty("Thread", QVariant());
      reply->setProperty("ThreadId", QVariant());

      reply->close();
      reply->deleteLater();
      reply = NULL;

      emit started(pThread);
      return;
    }
  }

  if (!reply->error())
    emit success(pThread);

  emit finished(pThread);

  if (pThread->id()==-1) delete pThread;
  // TODO : Bug Qt 4.4.0
  else pThread->end();
  /*else if (!reply->property("Finished").isValid()) {
    pThread->end();
    reply->setProperty("Finished", -1);
  }*/

  reply->setProperty("Thread", QVariant());
  reply->setProperty("ThreadId", QVariant());

  reply->readAll();

  reply->close();
  reply->deleteLater();
  reply = NULL;

  if (m_lstReplies.isEmpty())
    emit allDone();

  // Exécute une autre requête en attente dans la file
  if ((pThread->id()!=-1) && (processingPendingRequests()==true) && !m_lstPendingRequests.isEmpty())
    nextPendingRequest_(pThread->id());
}

void CNetworkManager::slotTransferRateUpdated() {
  const CTransferRate* transferRateManager = qobject_cast<const CTransferRate*>(this->sender());
  const QNetworkReply* reply = qobject_cast<QNetworkReply*>(transferRateManager->parent());

  emit transferRateUpdated(transferRateManager, thread_(reply));
}

QNetworkReply* CNetworkManager::doRequest_(const QUrl& url) {
  m_baseRequest.setUrl(url);

  // Check proxy !
  //m_pNetworkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "proxyweb.utc.fr", 3128));
  QNetworkReply* reply = m_pNetworkManager->get(m_baseRequest);
  Q_CHECK_PTR(reply); // Normalement cela ne doit jamais se produire !

  // Gestionnaire de calcul du taux de transfert
  CTransferRate* transferRate = new CTransferRate(reply, CTransferRate::DEFAULT_TIME_UPDATE, reply);
  transferRate->start();

  QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)),   this, SLOT(slotNetworkReplyDownloadProgress(qint64, qint64)));
  QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkReplyError(QNetworkReply::NetworkError)));
  QObject::connect(transferRate, SIGNAL(updated()), this, SLOT(slotTransferRateUpdated()));

  m_lstReplies.append(reply);

  return reply;
}

int CNetworkManager::threadInWaiting_() const {
  for (int i = 0; i < numberOfThreads(); ++i) {
    if (m_lstThreads[i].inProcess()==false)
      return i;
  }

  return -1;
}

void CNetworkManager::nextPendingRequest_(int thread) {
  int thread_free = (thread==-1)?threadInWaiting_():thread;
  Q_ASSERT((processingPendingRequests()==true) && !m_lstPendingRequests.isEmpty() && (thread_free!=-1));

  MCUrlInfo urlInfo = m_lstPendingRequests.takeOne();
  Assert(urlInfo.isValid() == true);

  if (m_lstPendingRequests.isEmpty())
    setProcessingPendingRequests(false);

  QNetworkReply* reply = doRequest_(urlInfo.url());
  m_lstThreads[thread_free].start(reply, urlInfo);

  reply->setProperty("Thread", qVariantFromValue(&m_lstThreads[thread_free]));
  reply->setProperty("ThreadId", thread_free);
  reply->setProperty("BaseUrl", urlInfo.url());

  emit started(&m_lstThreads[thread_free]);
}
