/***********************************************************************
 ** i2c.h
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
 * @file i2c.h
 * i2C related types & methods.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#ifndef I2C_H_
#define I2C_H_

/***********************************************************************
 * Types
 ***********************************************************************/

struct ST_IIC {
	int fd;

	uint8_t i_address;
};

/***********************************************************************
 * Methods
 ***********************************************************************/


#ifdef __cplusplus
extern "C" {
#endif
int f_iic_change_slave(struct ST_IIC * ps_this, uint8_t in_i_address);

int f_iic_init(struct ST_IIC * ps_this, const char *in_str_device,
		uint8_t in_i_address);

int f_iic_read(struct ST_IIC * ps_this, uint8_t in_reg, char * in_pc_buffer,
		size_t in_sz_transfer);

int f_iic_write(struct ST_IIC * ps_this, uint8_t in_reg, char * in_pc_buffer,
		size_t in_sz_transfer);

int f_iic_write_byte(struct ST_IIC * ps_this, uint8_t in_reg, char in_c_value);

int f_iic_close(struct ST_IIC * ps_this);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
