/****************************************************************************
 * @(#) Tools.
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

#ifndef TOOLS_H
#define TOOLS_H

#include <QtGlobal>
#include <QString>

/**
 * @brief Boîte à outils de fonctions
 */
namespace Tools {
  /** @brief Formats de conversion d'une taille de données en octets */
  enum FormatSize {
    Bytes,
    Kilobytes,
    Megabytes,
    Gigabytes,
    AdaptedSize
  };

  /**
   * @brief Convertie une taille en octets dans el format de destination
   *
   * @param[in] size : Taille à convertir.
   * @param[in] format : Vers le format de destination.
   * @param[out] formatOut = Format de destination utilisé (si format = AdaptedSize).
   *
   * @return la taille dans le format voulu.
   */
  double convertSize(qint64 size, FormatSize format = AdaptedSize, FormatSize* formatOut = NULL);

  /**
   * @brief Retourne le préfixe d'un format de conversion sous forme de chaîne de caractères
   *
   * @param[in] format : Format de conversion.
   *
   * @return le préfixe du format de conversion.
   *
   * @pre format!=AdaptedSize
   */
  QString sizePrefixString(FormatSize format);

  /**
   * @brief Retourne une taille formattée
   *
   * @param[in] size : Taille à formatter.
   * @param[in] format : Format de conversion.
   * @param[in] precision : Précision.
   *
   * @return une chaîne de caractères avec la taille correctement formattée.
   *
   * @note Si precision==-1, par défaut une précision de 0 est prise pour les formats <=Kilobytes, sinon elle vaut 2 pour les autres.
   */
  QString sizeToString(qint64 size, FormatSize format = AdaptedSize, int precision = -1);

  QUrl decodeUrl(const QUrl& url);

  QUrl absoluteUrl(const QString& base, const QString& relative);
}


#endif
