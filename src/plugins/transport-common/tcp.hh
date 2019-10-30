/***********************************************************************
 ** tcp.hh
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
#ifndef TCP_HH_
#define TCP_HH_
/**
 * @file tcp.hh
 * TCP plugin TRANSPORT definition.
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
#include <cpp/lock.hh>
#include <c/common.h>
#include <plugin.hh>
#include <host.hh>
#include <list>

/***********************************************************************
 * namespace
 ***********************************************************************/
namespace master {
namespace plugins {
namespace transport {
namespace tcp {

/***********************************************************************
 * Types
 ***********************************************************************/

using boost::asio::ip::tcp;

class CT_TCP;
#if 0
class CT_TCP_PARSER: public  {
protected:
	tcp::socket & _c_socket;

	char * _pc_buffer;
	size_t _sz_buffer;
	uint64_t _i_offset;
public:
	CT_TCP_PARSER(char * in_pc_buffer, size_t in_sz_buffer);
	~CT_TCP_PARSER();

	int parse_data(char* in_pc_buffer, size_t in_sz_buffer);

	uint64_t get_offset(void) {
		return _i_offset;
	}
	uint64_t get_size(void) {
		return _sz_buffer;
	}
};
#endif

class CT_TCP_SESSION : bml::node_parser<CT_GUARD> {
private:
	size_t _i_offset_read;
	//size_t _i_offset_read_tmp;

	volatile size_t _sz_available;
	tcp::socket _c_socket;
	std::vector<char> _v_buffer_recv;
	std::vector<char> _v_buffer_send;
	CT_TCP & _c_server;

	/* Remote info */
	boost::asio::ip::tcp::endpoint _c_remote;
	std::string _str_service;
	std::string _str_address;
	std::string _str_address_full;
	ushort _i_port;

	std::list<CT_GUARD<CT_PORT_NODE> > _l_msg;
	CT_SPINLOCK _c_msg_lock;

	/* Connection status */
	bool _b_connected;
	bool _b_connect_in_progress;

	/* Parser offset */
	size_t _i_offset_parser;

	/*Sync read error */
	boost::system::error_code _ec_sync_boost;

public:
	CT_TCP_SESSION(CT_TCP& in_c_server);
	~CT_TCP_SESSION();

	/* Set endpoint for connect */
	void f_set_endpoint(std::string const & in_str_address,
			std::string const & in_str_service);

	/* Notify command */
	void f_notify_link_failure();
	void f_notify_link_established(void);

	/* Asio commands */
	void f_start();
	void f_async_read(void);
	int f_sync_read(void);
	void f_send();
	void f_send(CT_GUARD<CT_PORT_NODE> const & in_pc_node);
	void f_disconnect();

	/* Asio event */
	void on_read(const boost::system::error_code& error,
			size_t bytes_transferred);
	void on_read_sync(const boost::system::error_code& error,
			size_t bytes_transferred);

	void on_write(const boost::system::error_code& error);

	void on_timeout(boost::optional<boost::system::error_code>* a,
			boost::system::error_code b);
	void on_send(const boost::system::error_code& error);

	/* Parser methods */
	int parse_data(char* in_pc_buffer, size_t in_sz_buffer);


	tcp::socket& f_get_socket() {
		return _c_socket;
	}
	std::string f_get_address_full() {
		return _str_address_full;
	}
};

class CT_TCP: public CT_TRANSPORT {
	/* Queue name */
	std::string _str_name;

	/* IO service */
	boost::asio::io_service _c_io_service;

	/* tcp socket */
	boost::asio::ip::tcp::socket _c_socket;

	/* tmp session socket */
	CT_TCP_SESSION* _pc_tmp_session;

	/* tcp session acceptor */
	boost::asio::ip::tcp::acceptor _c_acceptor;

	/* Context of msqqueue. Msgqueue has its own context */
	CT_HOST_CONTEXT * _pc_context;

	/* Session list */
	std::list<CT_TCP_SESSION*> _l_sessions;

	/* Map session / address */
	std::map<std::string, CT_TCP_SESSION*> _m_clients;


	/* Destination */
	std::string _str_dst;
	std::string _str_service;

	/* Server port */
	short _i_port;

	/* Server mode */
	bool _b_server_mode;

	/* Session Lock */
	CT_SPINLOCK _c_lock;

public:
	CT_TCP();
	~CT_TCP();

	/* Asio event */
	void on_accept(CT_TCP_SESSION* in_pc_session,
			const boost::system::error_code& in_c_error);
	void on_connect(CT_TCP_SESSION* in_pc_session,
			const boost::system::error_code& in_c_error);
	void on_timeout(boost::optional<boost::system::error_code>* a,
			const boost::system::error_code & in_c_error);
	void on_post_send(CT_GUARD<CT_PORT_NODE> const in_pc_msg);

	/* Asio accept command */
	void f_accept(void);
	void f_connect(void);
	void f_bind(void);

	/* Applying config */
	int f_apply_config(std::string & in_str_url);

	/* transport method */
	int f_send(CT_GUARD<CT_PORT_NODE> const & in_c_node);

	/* context method */
	int f_run(CT_HOST_CONTEXT&);

	/* session handling */
	void f_session_remove(CT_TCP_SESSION * in_pc_session);
	void f_session_add(CT_TCP_SESSION * in_pc_session);
	void f_session_notify(CT_GUARD<CT_PORT_NODE> const & in_pc_msg);


	boost::asio::io_service& f_get_service() {
		return _c_io_service;
	}

};

}
}
}
}
#endif /* TCP_HH_ */
