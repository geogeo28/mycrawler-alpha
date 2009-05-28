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
 * @brief Classe de gestion d'un taux de transfert dans un p�riph�rique d'entr�e-sortie.
 */
class CTransferRate : public QObject {
  Q_OBJECT

  // ---------------------
  // Eum�rations publiques
  // ---------------------
  public:
    /** @brief D�lai en millisecondes de mise � jour du taux de transfert */
    enum {DEFAULT_TIME_UPDATE = 1000};


  // ------------------
  // M�thodes publiques
  // ------------------
  public:
    /**
     * @brief Constructeur
     *
     * @param[in] device : P�riph�rique d'entr�e-sortie � analyser.
     * @param[in] timeUpdate : D�lai en millisecondes avant chaque mise � jour du taux de transfert.
     * @param[in] parent : Objet parent (charg� de la suppression de this).
     */
    CTransferRate(const QIODevice* device, int timeUpdate = DEFAULT_TIME_UPDATE, QObject* parent = NULL);

    /**
     * @brief Destructeur virtuel
     */
    ~CTransferRate();

    const QIODevice* device() const {return m_pDevice;}
    void setDevice(const QIODevice* device) {m_pDevice = device;}

    /**
     * @brief D�marre le calcul du taux de transfert
     */
    void start();

    /**
     * @brief Relance le calcul du taux de transfert
     *
     * @note Agit de la m�me fa�on que start().
     */
    void restart() {start();}

    /**
     * @brief Arr�te le gestionnaire
     */
    void stop();

    /**
     * @brief Retourne le temps en millisecondes depuis le lancement du gestionnaire par start() ou restart()
     *
     * @return le temps en millisecondes depuis le lancement du gestionnaire.
     */
    int elapsed() const;

    /**
     * @brief Retourne le taux instantan�
     *
     * @return le taux instantan�.
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
     * @brief Signal de mise � jour du taux de transfert
     *
     * @note Ce signal est d�lench� lorsque le taux de transfert � �t� mis � jour.
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
  // Membres priv�s
  // ------------------
  private:
    bool m_bFirstStart;         /// Le gestionnaire a �t� d�marr� pour la premi�re fois
    const QIODevice* m_pDevice; /// P�riph�rique d'entr�e-sortie
    QTimer* m_pTimer;           /// Timer utile pour la mise � jour du taux de transfert
    QTime m_elapsedTime;        /// Horloge calculant le temps �coul� depuis le lancement du gestionnaire

    qint64 m_nBeginSize;        /// Taille en octets du buffer avant lancement du gestionnaire (non g�r�)
    qint64 m_nBytesTransfered;  /// Nombre d'octets d�j� transf�r�es
    qint64 m_nTransferRate;     /// Taux de transfert actuel
};

#endif
