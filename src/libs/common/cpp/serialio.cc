/***********************************************************************
 ** serialio
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
 * @file serialio.cc
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2017
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
 #include "serialio.hh"

 #include <cpp/debug.hh>
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>

CT_SERIALIO::CT_SERIALIO() {
  _i_fd = -1;
}

CT_SERIALIO::CT_SERIALIO(const char * in_str_device_name) {
 int ec;

 _i_fd = -1;

 ec = f_init(in_str_device_name);
 if(ec != 1) {
    _CRIT << "Unable to initialize SERIALIO" << _V(in_str_device_name);
 }
}

int CT_SERIALIO::f_init(const char * in_str_device_name) {
 struct termios attr;
 if(_i_fd != -1) {
   f_destroy();
 }

 if ((_i_fd = open(in_str_device_name, O_RDWR)) == -1) {
     perror("openserial(): open()");
     return EC_FAILURE;
 }
 if (tcgetattr(_i_fd, &_s_oldterminfo) == -1) {
     perror("openserial(): tcgetattr()");
     return EC_FAILURE;
 }
 attr = _s_oldterminfo;
 attr.c_cflag |= CLOCAL;
 attr.c_oflag = 0;

 if (tcflush(_i_fd, TCIOFLUSH) == -1) {
     perror("openserial(): tcflush()");
     return EC_FAILURE;
 }
 if (tcsetattr(_i_fd, TCSANOW, &attr) == -1) {
     perror("initserial(): tcsetattr()");
     return EC_FAILURE;
 }
 return EC_SUCCESS;
}

void  CT_SERIALIO::f_destroy(void) {
 tcsetattr(_i_fd, TCSANOW, &_s_oldterminfo);
 if (close(_i_fd) < 0)
     perror("closeserial()");
}
CT_SERIALIO::~CT_SERIALIO() {
   f_destroy();
}
void CT_SERIALIO::f_setRTS(bool in_b_value) {
 int status;

 if (ioctl(_i_fd, TIOCMGET, &status) == -1) {
     perror("setRTS(): TIOCMGET");
    _CRIT << "Unable to get modem bits";
    return;
 }
 if (in_b_value)
     status |= TIOCM_RTS;
 else
     status &= ~TIOCM_RTS;
 if (ioctl(_i_fd, TIOCMSET, &status) == -1) {
     perror("setRTS(): TIOCMSET");
     _CRIT << "Unable to set modem bits";
     return;
 }
 _DBG << "SET RTS "<< _V(in_b_value);
}

int CT_SERIALIO::f_getCTS(void) {
  int status;

 if (ioctl(_i_fd, TIOCMGET, &status) == -1) {
     perror("setRTS(): TIOCMGET");
     throw _CRIT << "Unable to get modem bits";
 }

 if(TIOCM_CTS&status) {
   return 1;
 } else {
   return 0;
 }
}
