/***********************************************************************
 ** function.cc
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
 * @file function.cc
 * Plugin function transport.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <function.hh>

using namespace std;


CT_FUNCTION::CT_FUNCTION(void) :
		CT_TRANSPORT() {

}

CT_FUNCTION::~CT_FUNCTION() {

}

int CT_FUNCTION::f_apply_config(std::string & in_str_url) {
	int ec;

	/* Name url */
	_str_name = in_str_url;

	ec = EC_SUCCESS;
	//out_err:
	return ec;
}

int CT_FUNCTION::f_send(CT_GUARD<CT_PORT_NODE> const & in_c_node) {
	//D("Function send %s of node %x", _str_name.c_str(), in_c_node->get_id());
	//_DBG << *in_c_node;
	f_push_cb(in_c_node);
	return EC_SUCCESS;
}

M_PLUGIN_TRANSPORT(CT_FUNCTION, "FUNC", M_STR_VERSION);
