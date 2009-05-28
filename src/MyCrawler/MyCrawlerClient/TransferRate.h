/****************************************************************************
 * @(#) Transfer rate manager.
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

#ifndef TRANSFERRATE_H
#define TRANSFERRATE_H

#include <QObject>
#include <QTime>

class QIODevice;
class QTimer;

/**
 * @brief Classe de gestion d'un taux de transfert dans un périphérique d'entrée-sortie.
 */
class CTransferRate : public QObject {
  Q_OBJECT

  // ---------------------
  // Eumérations publiques
  // ---------------------
  public:
    /** @brief Délai en millisecondes de mise à jour du taux de transfert */
    enum {DEFAULT_TIME_UPDATE = 1000};


  // ------------------
  // Méthodes publiques
  // ------------------
  public:
    /**
     * @brief Constructeur
     *
     * @param[in] device : Périphérique d'entrée-sortie à analyser.
     * @param[in] timeUpdate : Délai en millisecondes avant chaque mise à jour du taux de transfert.
     * @param[in] parent : Objet parent (chargé de la suppression de this).
     */
    CTransferRate(const QIODevice* device, int timeUpdate = DEFAULT_TIME_UPDATE, QObject* parent = NULL);

    /**
     * @brief Destructeur virtuel
     */
    ~CTransferRate();

    const QIODevice* device() const {return m_pDevice;}
    void setDevice(const QIODevice* device) {m_pDevice = device;}

    /**
     * @brief Démarre le calcul du taux de transfert
     */
    void start();

    /**
     * @brief Relance le calcul du taux de transfert
     *
     * @note Agit de la même façon que start().
     */
    void restart() {start();}

    /**
     * @brief Arrête le gestionnaire
     */
    void stop();

    /**
     * @brief Retourne le temps en millisecondes depuis le lancement du gestionnaire par start() ou restart()
     *
     * @return le temps en millisecondes depuis le lancement du gestionnaire.
     */
    int elapsed() const;

    /**
     * @brief Retourne le taux instantané
     *
     * @return le taux instantané.
     */
    qint64 rate() const {return m_nTransferRate;}

    /**
     * @brief Retourne le taux moyen
     *
     * @return le taux moyen.
     */
    qint64 averageRate() const;


  // ------------------
  // Signaux
  // ------------------
  signals:
    /**
     * @brief Signal de mise à jour du taux de transfert
     *
     * @note Ce signal est délenché lorsque le taux de transfert à été mis à jour.
     */
    void updated();


  // ------------------
  // Slots publics
  // ------------------
  public slots:
    /**
     * @brief Slot de calcul du taux de transfert
     *
     * @note Recalcul le taux de tranfert.
     */
    void calculateRate();


  // ------------------
  // Membres privés
  // ------------------
  private:
    bool m_bFirstStart;         /// Le gestionnaire a été démarré pour la première fois
    const QIODevice* m_pDevice; /// Périphérique d'entrée-sortie
    QTimer* m_pTimer;           /// Timer utile pour la mise à jour du taux de transfert
    QTime m_elapsedTime;        /// Horloge calculant le temps écoulé depuis le lancement du gestionnaire

    qint64 m_nBeginSize;        /// Taille en octets du buffer avant lancement du gestionnaire (non géré)
    qint64 m_nBytesTransfered;  /// Nombre d'octets déjà transférées
    qint64 m_nTransferRate;     /// Taux de transfert actuel
};

#endif
