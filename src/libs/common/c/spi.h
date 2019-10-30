/***********************************************************************
 ** spi.h
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

/**
 * @file spi.h
 * SPI related types & methods.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#ifndef SPI_H_
#define SPI_H_

#include <c/common.h>
#include <c/misc.h>

/* sPI */
#include <linux/spi/spidev.h>

/***********************************************************************
 * Defines
 ***********************************************************************/
#define C_SPI_MAX_TR 20
#define C_SPI_MAX_TR_SIZE 2048

/***********************************************************************
 * Types
 ***********************************************************************/

struct ST_SPI {
	int fd;

	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	uint16_t delay;

	struct spi_ioc_transfer as_tr[C_SPI_MAX_TR];
	int i_tr_index;

};

struct ST_SPI_COMMAND {
	char b_read;
	int i_add;
	uint8_t * pc_buffer;
	size_t sz_transfer;

	uint8_t ac_tx[C_SPI_MAX_TR_SIZE];
	uint8_t ac_rx[C_SPI_MAX_TR_SIZE];
};

/***********************************************************************
 * Methods
 ***********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

int f_spi_init(struct ST_SPI * ps_this, const char *in_str_device,
		uint8_t in_i_mode, uint in_i_speed, uint in_i_nb_bits);
int f_spi_transfer(struct ST_SPI * ps_this, uint8_t * in_rx_buffer,
		uint8_t * in_tx_buffer, size_t in_sz_transfer);
int f_spi_sync(struct ST_SPI * ps_this);
int f_spi_close(struct ST_SPI * ps_this);

#ifdef __cplusplus
}
;
#endif
#endif /* SPI_H_ */
