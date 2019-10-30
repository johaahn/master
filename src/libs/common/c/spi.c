/***********************************************************************
 ** spi.c
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
 * @file spi.c
 * SPI related methods.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <c/common.h>
#include <c/spi.h>
/* errno */
#include <errno.h>
/* ioctl */
#include <sys/ioctl.h>
/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* close */
#include <unistd.h>
/* sPI */
#include <linux/spi/spidev.h>

/***********************************************************************
 * Methods
 ***********************************************************************/

int f_spi_close(struct ST_SPI * ps_this) {
	close(ps_this->fd);
	return EC_SUCCESS;
}

int f_spi_init(struct ST_SPI * ps_this, const char *in_str_device,
		uint8_t in_i_mode, uint in_i_speed, uint in_i_nb_bits) {
	int ret;
	ps_this->mode = in_i_mode;
	ps_this->bits = in_i_nb_bits;
	ps_this->speed = in_i_speed;
	ps_this->delay = 0;

	ps_this->fd = open(in_str_device, O_RDWR);
	if (ps_this->fd < 0) {
		CRIT("can't open device");
		ret = EC_FAILURE;
		goto out_err;
	}

	/*
	 * spi mode
	 */
	ret = ioctl(ps_this->fd, SPI_IOC_WR_MODE, &ps_this->mode);
	if (ret == -1) {
		CRIT("can't set spi mode");
	}
	ret = ioctl(ps_this->fd, SPI_IOC_RD_MODE, &ps_this->mode);
	if (ret == -1) {
		CRIT("can't get spi mode");
	}
	/*
	 * bits per word
	 */
	ret = ioctl(ps_this->fd, SPI_IOC_WR_BITS_PER_WORD, &ps_this->bits);
	if (ret == -1)
		CRIT("can't set bits per word");

	ret = ioctl(ps_this->fd, SPI_IOC_RD_BITS_PER_WORD, &ps_this->bits);
	if (ret == -1)
		CRIT("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(ps_this->fd, SPI_IOC_WR_MAX_SPEED_HZ, &ps_this->speed);
	if (ret == -1)
		CRIT("can't set max speed hz");

	ret = ioctl(ps_this->fd, SPI_IOC_RD_MAX_SPEED_HZ, &ps_this->speed);
	if (ret == -1)
		CRIT("can't get max speed hz");

	printf("spi mode: %d\n", ps_this->mode);
	printf("bits per word: %d\n", ps_this->bits);
	printf("max speed: %d Hz (%d KHz)\n", ps_this->speed,
			ps_this->speed / 1000);

	ps_this->i_tr_index = 0;

	out_err: ret = EC_SUCCESS;
	return ret;
}

int f_spi_transfer(struct ST_SPI * ps_this, uint8_t * in_rx_buffer,
		uint8_t * in_tx_buffer, size_t in_sz_transfer) {

	int i_tr_index = ps_this->i_tr_index++;
	M_ASSERT(i_tr_index < C_SPI_MAX_TR);

	ps_this->as_tr[i_tr_index].tx_buf = (unsigned long) in_tx_buffer;
	ps_this->as_tr[i_tr_index].rx_buf = (unsigned long) in_rx_buffer;
	ps_this->as_tr[i_tr_index].delay_usecs = 0;
	ps_this->as_tr[i_tr_index].len = in_sz_transfer;
	ps_this->as_tr[i_tr_index].speed_hz = ps_this->speed;
	ps_this->as_tr[i_tr_index].bits_per_word = ps_this->bits;

	return EC_SUCCESS;

}

int f_spi_sync(struct ST_SPI * ps_this) {
	int ret;

	ret = ioctl(ps_this->fd, SPI_IOC_MESSAGE(ps_this->i_tr_index),
			&ps_this->as_tr);
	ps_this->i_tr_index = 0;
	return ret < 1 ? EC_FAILURE : EC_SUCCESS;

}
