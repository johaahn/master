/***********************************************************************
 ** i2c.c
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
#include <c/i2c.h>
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

/*str error */
#include <string.h>
/* IIC */
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int f_iic_change_slave(struct ST_IIC * ps_this, uint8_t in_i_address) {
	int ec;

	if (ps_this->i_address == in_i_address) {
		ec = EC_SUCCESS;
		goto out_err;
	}

	if (ioctl(ps_this->fd, I2C_SLAVE, in_i_address) < 0) { // Set the port options and set the address of the device we wish to speak to
		CRIT("Unable to get bus access to talk to slave\n");
		goto out_err;
	}
	ps_this->i_address = in_i_address;
	ec = EC_SUCCESS;
	out_err: return ec;
}

int f_iic_init(struct ST_IIC * ps_this, const char *in_str_device,
		uint8_t in_i_address) {
	int ec;

	ps_this->i_address = 0;

	ps_this->fd = open(in_str_device, O_RDWR);
	if (ps_this->fd < 0) {
		CRIT("can't open device");
		ec = EC_FAILURE;
		goto out_err;
	}

	ec = f_iic_change_slave(ps_this, in_i_address);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to change slave address to 0x%x\n", in_i_address);
		ec = EC_FAILURE;
		goto out_err;
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}

static inline __s32 f_iic_smbus_access(struct ST_IIC * ps_this,
		char in_c_read_write, uint8_t in_i_command, int in_i_size,
		union i2c_smbus_data * in_ps_data) {
	int ec;
	struct i2c_smbus_ioctl_data args;

	args.read_write = in_c_read_write;
	args.command = in_i_command;
	args.size = in_i_size;
	args.data = in_ps_data;

	ec = ioctl(ps_this->fd, I2C_SMBUS, &args);
	//D("%d %x %d %d", in_c_read_write, in_i_command, in_ps_data->block[0]);
	//D("ec: %d", ec);
	return ec;
}

int f_iic_read(struct ST_IIC * ps_this, uint8_t in_reg, char * in_pc_buffer,
		size_t in_sz_transfer) {
	int ec;
	size_t sz_max = in_sz_transfer;
	union i2c_smbus_data s_data;
	int i;

	if (sz_max > I2C_SMBUS_BLOCK_MAX) {
		sz_max = I2C_SMBUS_BLOCK_MAX;
	}
	s_data.block[0] = sz_max;

	/* Read IIC */
	ec = f_iic_smbus_access(ps_this, I2C_SMBUS_READ, in_reg,
			I2C_SMBUS_I2C_BLOCK_DATA, &s_data);
	if (ec) {
		CRIT("Unable to read from slave %d %d : %s\n",
				ec, sz_max, strerror(errno));
		ec = EC_FAILURE;
		goto out_err;
	}
	M_ASSERT(s_data.block[0] == sz_max);
	/* Copy data */
	for (i = 1; i <= s_data.block[0]; i++) {

		in_pc_buffer[i - 1] = s_data.block[i];
		//printf("%x ", in_pc_buffer[i - 1]);
	}
	//puts("");
	//*in_psz_transfer = s_data.block[0];

	ec = EC_SUCCESS;
	out_err:

	return ec;
}

int f_iic_write(struct ST_IIC * ps_this, uint8_t in_reg, char * in_pc_buffer,
		size_t in_sz_transfer) {
	int ec;
	union i2c_smbus_data s_data;
	int i;

	/* Copy data */
	s_data.block[0] = in_sz_transfer;
	for (i = 1; i <= s_data.block[0]; i++) {
		s_data.block[i] = in_pc_buffer[i - 1];
	}

	/* Write IIC */
	ec = f_iic_smbus_access(ps_this, I2C_SMBUS_WRITE, in_reg,
			I2C_SMBUS_I2C_BLOCK_DATA, &s_data);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to write from slave %d %d : %s\n",
				ec, in_sz_transfer, strerror(errno));
		ec = EC_FAILURE;
		goto out_err;
	}
	ec = EC_SUCCESS;
	out_err:

	return ec;
}

int f_iic_write_byte(struct ST_IIC * ps_this, uint8_t in_reg, char in_c_value) {
	return f_iic_write(ps_this, in_reg, &in_c_value, 1);
}

int f_iic_close(struct ST_IIC * ps_this) {
	close(ps_this->fd);
	return EC_SUCCESS;
}
