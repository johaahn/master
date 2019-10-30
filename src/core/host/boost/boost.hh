/***********************************************************************
 ** boost.hh
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
 ***********************************************************************/

/* define against mutual inclusion */
#ifndef HOST_BOOST_HH_
#define HOST_BOOST_HH_

/**
 * @file boost.hh
 * HOST definition using BOOST Library.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include "boost/smart_ptr/detail/spinlock.hpp"
#undef D
#include <host.hh>

/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/

class CT_BOOST_TIMEOUT: public CT_HOST_TIMEOUT {
public:
	CT_BOOST_TIMEOUT();
	/* virtual methods */
	virtual ~CT_BOOST_TIMEOUT();

	/* timeout methods */
	void f_start(int in_i_ms, int in_i_interval);
	void f_stop();
};

class CT_BOOST_MUTEX: public CT_HOST_MUTEX {
	boost::mutex _c_lock;
	boost::condition_variable _c_cond;
public:
	void f_lock();
	bool f_try_lock();
	void f_unlock();

	void f_notify_wait();
	void f_notify_all();
	void f_notify_one();
};

class CT_BOOST_SPINLOCK: public CT_HOST_SPINLOCK  {
	boost::detail::spinlock _c_lock = BOOST_DETAIL_SPINLOCK_INIT;
public:
	CT_BOOST_SPINLOCK();

	void f_lock();
	bool f_try_lock();
	void f_unlock();
};

class CT_BOOST_CONTEXT: public CT_HOST_CONTEXT {
	boost::thread *_pc_thread;
public:
	CT_BOOST_CONTEXT();
	virtual ~CT_BOOST_CONTEXT();

	int f_init(void);
	int f_join(void);
};

class CT_BOOST_HOST: public CT_HOST {

	/* Timeout lock */
	boost::mutex _c_timeout_lock;

	/* Timeout map */
	std::list<CT_BOOST_TIMEOUT*> _l_timeouts;

	/* running state */
	volatile bool _b_running;

public:
	CT_BOOST_HOST(struct ST_HOST_ARGS * in_ps_args);
	virtual ~CT_BOOST_HOST();

	/* Virtual redefined function */
	int f_main_init(void);
	int f_main_loop(void);
	int f_plugin_load(void);

	/* timeout specifics */
	int f_timeout_start(CT_BOOST_TIMEOUT * in_pc_timeout);
	int f_timeout_stop(CT_BOOST_TIMEOUT * in_pc_timeout);
	int f_timeout_process(void);

	/* file methods */
	std::list<CT_HOST_FILE> f_file_list(CT_HOST_FILE const in_str_path,
			std::string const & in_str_prefix,
			std::string const & in_str_suffix);

	/* allocators */
	CT_HOST_LOCK * f_new_mutex(void);
	CT_HOST_CONTEXT * f_new_context(void);
	CT_HOST_LOCK * f_new_spinlock(void);
	CT_HOST_TIMEOUT * f_new_timeout(void);

	/* context wait */
	int f_join(void);

	/* plugin function */
	int f_plugin_reload(struct ST_PLUGIN_INFO & inout_s_info);
	int f_plugin_close(struct ST_PLUGIN_INFO & inout_s_info);

private:
	int f_plugin_load(char const * in_str_module);
};

CT_HOST * f_host_boost(struct ST_HOST_ARGS * in_ps_args);

#endif /* HOST_BOOST_HH_ */
