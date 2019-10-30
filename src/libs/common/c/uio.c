/***********************************************************************
 ** uio.c
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
 * @file uio.c
 * User interface IO methods.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2014
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "uio.h"
#include <c/misc.h>

/***********************************************************************
 * Functions
 ***********************************************************************/

int f_uio_init(struct ST_UIO * in_ps_uio) {
	long i;
	in_ps_uio->i_fd = -1;
	in_ps_uio->str_uio_name = NULL;
	in_ps_uio->str_sysfs_path = NULL;
	in_ps_uio->str_dev_path = NULL;

	for (i = 0; i < C_UIO_NB_MMAP_MAX; i++) {
		in_ps_uio->as_mmap[i].str_name = NULL;
		in_ps_uio->as_mmap[i].pv_virtual = NULL;
		in_ps_uio->as_mmap[i].pv_physical = NULL;
		in_ps_uio->as_mmap[i].i_length = 0;
	}

	FD_ZERO(&in_ps_uio->rd_fds);

	return EC_SUCCESS;
}

int f_uio_close(struct ST_UIO * in_ps_uio) {
	long i = 0;

	if (in_ps_uio->str_uio_name)
		free(in_ps_uio->str_uio_name);
	if (in_ps_uio->str_sysfs_path)
		free(in_ps_uio->str_sysfs_path);
	if (in_ps_uio->str_dev_path)
		free(in_ps_uio->str_dev_path);
	if (in_ps_uio->i_fd != -1) {
		close(in_ps_uio->i_fd);
	}
	for (i = 0; i < C_UIO_NB_MMAP_MAX; i++) {
		struct ST_UIO_MMAP * ps_mmap = &in_ps_uio->as_mmap[i];
		if (ps_mmap->pv_virtual) {
			munmap(ps_mmap->pv_virtual, ps_mmap->i_length);
		}
		if (ps_mmap->str_name) {
			free(ps_mmap->str_name);
		}
	}
	return EC_SUCCESS;
}

int f_uio_open(struct ST_UIO * in_ps_uio, char const * in_str_name) {
	int ec, num;
	struct dirent **namelist;
	int i, j;
	f_uio_close(in_ps_uio);

	DBG("Looking for UIO: %s", in_str_name);

	num = scandir("/sys/class/uio", &namelist, NULL, alphasort);
	if (num < 0) {
		/* No device found */
		CRIT("No UIO sys class");
	} else {

		/* Loop on result */
		for (i = num - 1; i > 0; i--) {
			/* remove parsing files */
			if (namelist[i]->d_name[0] != '.') {

				char str_file[300];
				char str_file_content[300];

				snprintf(str_file, sizeof(str_file), "/sys/class/uio/%s/name",
						namelist[i]->d_name);

				ec = f_file_dump(str_file, str_file_content,
						sizeof(str_file_content));

				DBG("%s %s %s", str_file, str_file_content, in_str_name);
				if (ec >= 0) {
					ec = strncmp(str_file_content, in_str_name,
							strlen(str_file_content));
					if (ec == 0) {
						/* generate path of device */
						sprintf(str_file_content, "/dev/%s",
								namelist[i]->d_name);
						in_ps_uio->str_dev_path = strdup(str_file_content);
						sprintf(str_file_content, "/sys/class/uio/%s",
								namelist[i]->d_name);
						in_ps_uio->str_sysfs_path = strdup(str_file_content);
						in_ps_uio->str_uio_name = strdup(in_str_name);

						/* open device */
						in_ps_uio->i_fd = open(in_ps_uio->str_dev_path, O_RDWR);
						if (in_ps_uio->i_fd < 0) {
							CRIT("Unable to open UIO device %s",
									in_ps_uio->str_dev_path);
							return EC_FAILURE;
						}

						FD_SET(in_ps_uio->i_fd, &in_ps_uio->rd_fds);

						for (j = 0; j < C_UIO_NB_MMAP_MAX; j++) {

							struct ST_UIO_MMAP * ps_mmap =
									&in_ps_uio->as_mmap[j];

							/* get name */
							sprintf(str_file,
									"/sys/class/uio/%s/maps/map%d/name",
									namelist[i]->d_name, j);
							ec = f_file_dump(str_file, str_file_content,
									sizeof str_file_content);
							if (ec == EC_FAILURE)
								break;

							ps_mmap->str_name = strdup(str_file_content);

							/* get physical address */
							sprintf(str_file,
									"/sys/class/uio/%s/maps/map%d/addr",
									namelist[i]->d_name, j);
							ec = f_file_dump(str_file, str_file_content,
									sizeof str_file_content);
							if (ec == EC_FAILURE)
								break;

							ps_mmap->pv_physical = (void *) strtoul(
									str_file_content, NULL, 16);
							DBG(" phys: %s %p", str_file_content,
									ps_mmap->pv_physical);
							/* get size */
							sprintf(str_file,
									"/sys/class/uio/%s/maps/map%d/size",
									namelist[i]->d_name, j);
							ec = f_file_dump(str_file, str_file_content,
									sizeof str_file_content);
							if (ec == EC_FAILURE)
								break;
							ps_mmap->i_length = strtol(str_file_content, NULL,
									16);

							/* mmap if needed */
							if (ps_mmap->i_length) {
								ps_mmap->pv_virtual = mmap(0, ps_mmap->i_length,
								PROT_READ | PROT_WRITE, MAP_SHARED,
										in_ps_uio->i_fd,
										(off_t) j * sysconf(_SC_PAGE_SIZE));

								if (ps_mmap->pv_virtual == (void*) -1) {
									perror("mmap");
									return EC_FAILURE;
								}
							}
						}

						f_uio_display(in_ps_uio);

						return EC_SUCCESS;
					}
				}
			}
		}
	}

	CRIT("Unable to locate UIO device: %s", in_str_name);
	ec = EC_FAILURE;
	return ec;
}

int f_uio_irqcontrol(struct ST_UIO * in_ps_uio, int in_i_value) {
	int32_t i_tmp = in_i_value;
	int ec;
	ec = write(in_ps_uio->i_fd, (char*)&i_tmp, sizeof(i_tmp));
	if(ec != 4) {
		perror("write");
		CRIT("Unable to send irq control of UIO: %s", in_ps_uio->str_uio_name);
		return EC_FAILURE;
	} else {
		return EC_SUCCESS;
	}
}

int f_uio_wait_ist(struct ST_UIO * in_ps_uio, int in_i_timeout_ms) {
#if 0
	int ec;
	unsigned long i_dummy;
	ec = read(in_ps_uio->i_fd, &i_dummy, sizeof(i_dummy));
	if (ec == -1) {
		return EC_FAILURE;
	} else {
		return EC_SUCCESS;
	}
#else
	fd_set tmp_fds = in_ps_uio->rd_fds;
	struct timeval timeout;
	timeout.tv_sec = in_i_timeout_ms / 1000;
	timeout.tv_usec = in_i_timeout_ms - timeout.tv_sec * 1000;
	int ret = select(in_ps_uio->i_fd + 1, &tmp_fds, NULL, NULL, &timeout);
	if (ret > 0) {
		//irq_count++;
		//gettimeofday(&cur_tv,NULL);
		//cur_tim = cur_tv.tv_sec
		//	+ (double)cur_tv.tv_usec/1000000.0;
		//if (last_tim > 0.0) dt = (cur_tim - last_tim);
		//last_tim = cur_tim;
		unsigned long dummy;
		if (read(in_ps_uio->i_fd, &dummy, sizeof(dummy)) != sizeof(dummy)) {
			perror("read");
			return EC_FAILURE;
		}
		return EC_SUCCESS;
	} else if (ret == 0) {
		return EC_BYPASS;
	} else {
		perror("select");
		return EC_FAILURE;
	}
#endif
}

void f_uio_display(struct ST_UIO * in_ps_uio) {
	long i = 0;
	DBG("Device UIO: %s", in_ps_uio->str_uio_name);
	DBG("- dev: %s", in_ps_uio->str_dev_path);
	DBG("- sys: %s", in_ps_uio->str_sysfs_path);
	DBG("- fd: %d", in_ps_uio->i_fd);

	for (i = 0; i < C_UIO_NB_MMAP_MAX; i++) {

		if (in_ps_uio->as_mmap[i].pv_virtual != 0) {
			DBG("* %d: %s", i, in_ps_uio->as_mmap[i].str_name);
			DBG("  - virt: %p", in_ps_uio->as_mmap[i].pv_virtual);
			DBG("  - phys: %p", in_ps_uio->as_mmap[i].pv_physical);
			DBG("  - len: %d bytes", in_ps_uio->as_mmap[i].i_length);
		}
	}
}
