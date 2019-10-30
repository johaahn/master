/***********************************************************************
 ** randn.h
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
 **
 ***********************************************************************/

#ifndef RANDN_H_
#define RANDN_H_


/**
 * @file randn.h
 * Misc methods.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <math.h>
#include <unistd.h>
#include <stdlib.h>


/***********************************************************************
 * Defines
 ***********************************************************************/


/***********************************************************************
 * Types
 ***********************************************************************/
struct ST_RANDN {
  struct drand48_data s_drand_buf;
};

/***********************************************************************
 * Functions
 ***********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* Génération d'un bruit gaussien (moyenne 0, écart-type 1) */
void f_get_randn2(float *pf_out1, float *pf_out2);

/* Initilisation du PRN */
void f_randn_seed(struct ST_RANDN * in_ps_this, long int in_i_seed);

/* Génération d'un bruit gaussien (moyenne 0, écart-type 1) */
float f_randn(struct ST_RANDN * in_ps_this);

/* Génération d'une variable aléatoire suivant loi uniforme (entre 0 et 1) */
float f_rand(struct ST_RANDN * in_ps_this);

#ifdef __cplusplus
}
;
#endif


#endif /* RANDN_H_ */
