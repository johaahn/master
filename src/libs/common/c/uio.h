/***********************************************************************
 ** uio.h
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

#ifndef UIO_H_
#define UIO_H_


/**
 * @file uio.h
 * User IO API.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2014
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <c/common.h>
#include <c/mutex.h>


/***********************************************************************
 * Defines
 ***********************************************************************/
#define C_UIO_NB_MMAP_MAX 10

/***********************************************************************
 * Types
 ***********************************************************************/


struct ST_UIO_MMAP {
	  /** Mmap name **/
	  char * str_name;
	 /** virtual address **/
	  void * pv_virtual;
	  /** physical address **/
	  void * pv_physical;
	  /** size **/
	  int i_length;
};

struct ST_UIO
{

  /** UIO device name */
  char * str_uio_name;
  /** SYSFS path**/
  char * str_sysfs_path;
  /** Device path **/
  char * str_dev_path;

  /* File descriptor */
  int i_fd;


  /* Memory mapping */
  struct ST_UIO_MMAP as_mmap[C_UIO_NB_MMAP_MAX];

  /* Select variable */
  fd_set rd_fds;
};

/***********************************************************************
 * Prototypes
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
int f_uio_init(struct ST_UIO * in_ps_uio);
int f_uio_open(struct ST_UIO * in_ps_uio, char const * in_str_dname);
int f_uio_close(struct ST_UIO * in_ps_uio);

void f_uio_display(struct ST_UIO * in_ps_uio);
int f_uio_wait_ist(struct ST_UIO * in_ps_uio, int in_i_timeout_ms);
int f_uio_irqcontrol(struct ST_UIO * in_ps_uio, int in_i_value);

#ifdef __cplusplus
}
#endif


#endif /* UIO_H_ */
