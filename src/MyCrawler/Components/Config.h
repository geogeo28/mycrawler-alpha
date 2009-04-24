/****************************************************************************
 * @(#) Configuration header. Contains all macros.
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

#ifndef CONFIG_H
#define CONFIG_H

#define xstr(s) str(s)
#define str(s) #s

#define _MYCRAWLER_APPNAME_    "MyCrawler"
#define _MYCRAWLER_DATE_       "2009-04-17"
#define _MYCRAWLER_AUTHOR_     "ANNEHEIM Geoffrey"
#define _MYCRAWLER_COMPILER_   __COMPILER__

#define _MYCRAWLER_ORGANIZATION_NAME_      "UTC"
#define _MYCRAWLER_ORGANIZATION_DOMAIN_    ""

#define _MYCRAWLER_VERSION_MAJOR_         0
#define _MYCRAWLER_VERSION_MINOR_         1
#define _MYCRAWLER_VERSION_REVISION_      0

#define _COMPILER_VERSION_      __VERSION__
#define _COMPILER_DESCRIPTION_  "MinGW g++ version "_COMPILER_VERSION_
#define _COMPILER_BUILD_DATE_  __DATE__" "__TIME__

#define _MYCRAWLER_VERSION_    xstr(_MYCRAWLER_VERSION_MAJOR_)"."xstr(_MYCRAWLER_VERSION_MINOR_)"."xstr(_MYCRAWLER_VERSION_REVISION_)

#endif // CONFIG_H
