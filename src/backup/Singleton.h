/****************************************************************************
 * @(#) Singleton.
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

#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class ISingleton
{
public:
    /**
     * @brief Returns the single instance of the class
     *
     * @return the single instance of the class
     */
    static T* instance()
    {
      if (!s_instance) {
        s_instance = new T;
      }

      return static_cast<T*>(s_instance);
    }

    /**
     * @brief Kill the single instance of the class
     */
    static void destroy()
    {
      if (s_instance) {
        delete s_instance;
        s_instance = NULL;
      }
    }

protected:
    /**
     * @brief Constructor
     */
    ISingleton() {}

    /**
     * @brief Destructor
     */
    ~ISingleton() {}

private:
    /**
     * @brief Copy forbidden
     */
    ISingleton(ISingleton&);
    void operator=(ISingleton&);

private:
    static T* s_instance; // Base instance of the class
};

//template <class T>
//T* ISingleton<T>::s_instance = NULL;

#define SINGLETON_DECLARATION(C) \
  public: \
    friend class ISingleton<C>; \
    static C* instance();

#define SINGLETON_IMPLEMENTATION(C) \
  template <> C* ISingleton<C>::s_instance = NULL; \
  C* C::instance() \
  { \
    return ISingleton<C>::instance(); \
  }

#endif // SINGLETON_H
