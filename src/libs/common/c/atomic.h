/***********************************************************************
 ** atomic.h
 ***********************************************************************
 ** Copyright (c) SEAGNAL SAS
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2.1 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 ** MA 02110-1301 USA
 **
 ** -------------------------------------------------------------------
 **
 ** As a special exception to the GNU Lesser General Public License, you
 ** may link, statically or dynamically, a "work that uses this Library"
 ** with a publicly distributed version of this Library to produce an
 ** executable file containing portions of this Library, and distribute
 ** that executable file under terms of your choice, without any of the
 ** additional requirements listed in clause 6 of the GNU Lesser General
 ** Public License.  By "a publicly distributed version of this Library",
 ** we mean either the unmodified Library as distributed by the copyright
 ** holder, or a modified version of this Library that is distributed under
 ** the conditions defined in clause 3 of the GNU Lesser General Public
 ** License.  This exception does not however invalidate any other reasons
 ** why the executable file might be covered by the GNU Lesser General
 ** Public License.
 ***********************************************************************/

#ifndef ATOMIC_H_
#define ATOMIC_H_


/**
 * @file mutex.h
 * Atomic methods.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/


/***********************************************************************
 * Defines
 ***********************************************************************/


/***********************************************************************
 * Types
 ***********************************************************************/

/***********************************************************************
 * Functions
 ***********************************************************************/

#if 0
#elif defined __GNUC__

typedef unsigned int atomic_t;
# define M_ATOMIC_SET(var, val)  var=val
# define M_ATOMIC_INC(var) __sync_fetch_and_add(&(var), 1)
# define M_ATOMIC_DEC(var) __sync_fetch_and_sub(&(var), 1)
# define M_ATOMIC_OR(var, val) __sync_or_and_fetch(&(var), (val))
# define M_ATOMIC_EXCHANGE(var, val) __sync_lock_test_and_set(&(var), (val))
# define M_ATOMIC_ADD(var, val) __sync_fetch_and_add(&(var), (val))
# define M_ATOMIC_SUB(var, val) __sync_fetch_and_sub(&(var), (val))


#elif defined _WIN32
#endif


#endif /* ATOMIC_H_ */
