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

#include "ServerHistory.h"

MCServerHistory* MCServerHistory::s_instance = NULL;

MCServerHistory* MCServerHistory::instance() {
  if (s_instance == NULL) {
    s_instance = new MCServerHistory();
  }

  return s_instance;
}

void MCServerHistory::destroy() {
  if (s_instance != NULL) {
    delete s_instance;
    s_instance = NULL;
  }
}

void MCServerHistory::init_() {

}

void MCServerHistory::cleanAll_() {

}

MCServerHistory::MCServerHistory()
{
  init_();
}

MCServerHistory::~MCServerHistory() {
  cleanAll_();
}
