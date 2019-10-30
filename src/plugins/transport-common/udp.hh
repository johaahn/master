/***********************************************************************
 ** udp.hh
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

/* define against mutual inclusion */
#ifndef UDP_HH_
#define UDP_HH_
/**
 * @file udp.hh
 * UDP Plugin transport definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include <c/common.h>
#include <plugin.hh>
#include <host.hh>
#include <list>

/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/

using boost::asio::ip::udp;


class CT_UDP: public CT_TRANSPORT, public  bml::node_parser<CT_GUARD> {
	/* Queue name */
	std::string _str_name;

	/* IO service */
	boost::asio::io_service _c_io_service;

	/* Context of msqqueue. Msgqueue has its own context */
	CT_HOST_CONTEXT * _pc_context;

	/* UDP socket */
	udp::socket _c_socket;

	/* Session list */
	//std::list<CT_UDP_SESSION*> _l_sessions;
	//CT_UDP_SESSION* _pc_remote;

	/* Send list */
	std::list< CT_GUARD<CT_PORT_NODE> > _l_msg;
	CT_SPINLOCK _c_msg_lock;

	/* UDP Endpoint */
	udp::endpoint _c_remote_endpoint;

	/* Destination */
	std::string _str_dst;
	std::string _str_service;

	/* Server port */
	short _i_port;

	/* Server mode */
	bool _b_server_mode;

	/* Receive buffer */
	size_t _i_offset_read;
	size_t _sz_buffer;
	size_t _sz_available;
	char *_pc_buffer_rcv;

	/* Send buffer */
	char *_pc_buffer_snd;

	/* Parser variable */
	size_t _i_offset_parser;

public:
	CT_UDP();
	~CT_UDP();

	/* Asio event */
	void on_timeout(boost::optional<boost::system::error_code>* a,
			const boost::system::error_code & in_c_error);
	void on_send(const boost::system::error_code& error,
			size_t in_i_bytes_transferred);
	void on_post_async_send(CT_GUARD<CT_PORT_NODE> const in_pc_msg);
	void on_read(const boost::system::error_code& error,
			size_t in_i_bytes_transferred);
	void on_received(const boost::system::error_code& error, unsigned int);

	/* Asio accept command */
	void f_bind();

	void f_sync_send(CT_GUARD<CT_PORT_NODE> const & in_c_node);
	void f_async_send();

	/* Applying config */
	int f_apply_config(std::string & in_str_url);

	/* transport method */
	int f_send(CT_GUARD<CT_PORT_NODE> const & in_c_node);

	/* context method */
	int f_run(CT_HOST_CONTEXT&);

	/* Parser methods */
	int parse_data(char* in_pc_buffer, size_t in_sz_buffer);
	void f_async_read();
	int f_sync_read(void);


	/* Notify command */
	void f_notify_link_failure(void);
	void f_notify_link_established(void);

	/* session handling */
	//void f_session_remove(CT_UDP_SESSION * in_pc_session);
	//void f_session_add(CT_UDP_SESSION * in_pc_session);

	boost::asio::io_service& f_get_service() {
		return _c_io_service;
	}

};

#endif /* TCP_HH_ */
