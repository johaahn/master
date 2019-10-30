/***********************************************************************
 ** task
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
#ifndef TASK_HH_
#define TASK_HH_

/**
 * @file task.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Aug 21, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <c/common.h>
#include <c/atomic.h>
#include <cpp/guard.hh>
#include <cpp/lock.hh>
#include <node.hh>
#include <port.hh>
#include <host.hh>
#include <vector>
#include <map>
#include <event_base.hh>
/***********************************************************************
 * namespaces
 ***********************************************************************/

class CT_HOST_TIMEOUT;
class CT_HOST_SPINLOCK;

namespace master {
namespace core {

/***********************************************************************
 * Includes
 ***********************************************************************/
enum ET_ID_TASK {
	E_ID_TASK_STATUS = (E_ID_TASK_BASE << C_ID_OFFSET_BASE),
	E_ID_TASK_STATUS_MSG
};

enum ET_TASK_STATUS {
	E_TASK_STATUS_PROGRESS = 0,
	E_TASK_STATUS_PART_COMPLETE,
	E_TASK_STATUS_STEP_COMPLETE,
	E_TASK_STATUS_TASK_COMPLETE,
	E_TASK_STATUS_KILLED,
	E_TASK_STATUS_ABORTED,
	E_TASK_STATUS_TIMEOUT,
	E_TASK_STATUS_ERROR,
};

enum ET_TASK_STATE {
	E_TASK_STATE_INIT,
	E_TASK_STATE_RUNNING,
	E_TASK_STATE_COMPLETE,
	E_TASK_STATE_ERROR,
	E_TASK_STATE_ABORTING,
};

#define C_TASK_MSG_SIZE 1024
#define C_TASK_ID_SHARED_BASE ((uint64_t)ULONG_MAX)

/***********************************************************************
 * Types
 ***********************************************************************/

struct ST_TASK_STATUS {
	uint64_t i_muid;
	uint64_t i_luid;
	uint32_t i_step;
	uint32_t __dummy;
}M_PACKED;

struct ST_TASK_NOTIFICATION {

	ST_TASK_STATUS s_status;

	/* event task */
	uint32_t e_status;

	/* Progress info */
	float f_progress;
};

class CT_TASK_STEP {
	/* complete flag */
	bool _b_complete;

	/* progress */
	uint8_t _i_progress;
public:
	void f_set_progress(uint8_t in_i_progress) {
		_i_progress = in_i_progress;
		if (_i_progress >= 100) {
			_b_complete = true;
		}
	}

	uint8_t f_get_progress(void) {
		return _i_progress;
	}

	bool f_is_complete(void) {
		return _b_complete;
	}

	CT_TASK_STEP() {
		_b_complete = false;
	}

};

class CT_TASK {
public:
	typedef std::pair<uint64_t, uint64_t> task_key_t;

public:
	/* USAGE ELEMENT */
	ST_GUARD_USAGE s_usage;

private:
	/* unique id key */
	task_key_t _c_uid;

	/* type id */
	uint32_t _i_type;

	/* Notification id */
	uint32_t _i_notification_id;

	/* Internal port */
	CT_PORT * _pc_notification_internal_port;

	/* External port */
	CT_PORT * _pc_notification_external_port;

	/* Callback notification */
	CT_PORT_SLOT _c_notification_cb;

	/* Steps list */
	std::vector<CT_TASK_STEP> _v_steps;

	/* Timeout */
	CT_HOST_TIMEOUT * _pc_timeout;

	/* Reply status */
	ST_TASK_STATUS _s_reply_status;
	bool _b_reply_status;

	/* Reply node */
	CT_PORT_NODE_GUARD _c_reply_node;

	/* Spin lock of content */
	CT_SPINLOCK _c_lock;

	/* Name */
	std::string _str_name;

	/* State */
	bool _b_enable;

	/* Last notification */
	ET_TASK_STATE _e_last_status;
	int _i_last_progress;
	std::string _str_last_msg;

	/* Data */
	CT_GUARD<CT_OBJECT_REF> _c_data;

public:
	CT_TASK(task_key_t const & in_c_uuid);
	virtual ~CT_TASK();

	/* Name */
	void f_set_name(std::string const & in_str_name);
	void f_set_type(uint32_t const & in_i_type);

	/* Notification config methods */
	void f_notification_set_id(uint32_t);
	void f_notification_set_external_id(uint32_t in_i_notification_id);
	void f_notification_set_internal_port(CT_PORT &);
	void f_notification_set_external_port(CT_PORT &);
	void f_notification_set_cb(CT_PORT_BIND_CORE const & in_c_cb);
	void f_notification_set_cb(std::nullptr_t);

	/* Describe task */
	void f_step_set_nb(uint32_t in_nb_step);
	uint32_t f_step_get_nb(void);
	CT_TASK_STEP & f_step_get(uint32_t in_i_step);

	/* Timeout method */
	void f_timeout_set(uint32_t in_i_ms);

	/* Status method */
	void f_status_init(CT_PORT_NODE& in_c_node, uint32_t in_i_step);
	void f_status_init(ST_TASK_STATUS& in_s_status, uint32_t in_i_step);
	ST_TASK_STATUS f_status_init(uint32_t in_i_step);

	/* Status complete method */
	bool f_status_is_complete(void);
	float f_status_get_progress(void);

	/* Reply methods - CRADO ...*/
	void f_reply_set_parent_task(CT_PORT_NODE & in_c_node);
	static void f_reply_copy_status(CT_PORT_NODE & in_c_to,
			CT_PORT_NODE & in_c_from);
	static ST_TASK_STATUS & f_reply_get_status(CT_PORT_NODE & in_c_from);
	static bool f_reply_has_status(CT_PORT_NODE & in_c_from);
	/* Reply methods - END ...*/

	/* Reply facility - port node */
	CT_PORT_NODE_GUARD & f_reply_get_node(void);

	/* Reply facility - parent status */
	void f_reply_set_parent_status(ST_TASK_STATUS & in_s_status);
	void f_reply_set_parent_status(CT_PORT_NODE & in_c_node);
	ST_TASK_STATUS & f_reply_get_parent_status(void);
	bool f_reply_has_parent_status(void);
	inline static task_key_t f_reply_get_key(
			ST_TASK_STATUS const & in_s_status);

	/* Processing methods */
	void f_enable();
	void f_enable(ST_TASK_STATUS const & in_s_status);

	void f_error();
	void f_error(std::string const & in_str_msg);
	void f_error(ST_TASK_STATUS const & in_s_status,
			std::string const & in_str_msg);

	void f_abort();
	void f_abort(ST_TASK_STATUS const & in_s_status);

	void f_kill();
	void f_kill(ST_TASK_STATUS const & in_s_status);

	void f_complete();
	void f_complete(uint32_t in_i_step, uint32_t in_i_part = 0);
	void f_complete(ST_TASK_STATUS const & in_s_status);

	void f_progress(float const in_f_progress, std::string const & in_str_msg);
	void f_progress(uint32_t in_i_step, uint32_t in_i_part,
			float const in_f_progress, std::string const & in_str_msg);
	void f_progress(ST_TASK_STATUS const & in_s_status,
			float const in_f_progress, std::string const & in_str_msg);

	void f_notify(ST_TASK_NOTIFICATION const & in_s_notif,
			std::string const & in_str_msg);
	void f_notify(uint32_t const in_i_id, ST_TASK_STATUS const & in_s_status,
			float in_f_percent = 0.0, std::string const & in_str_msg = "");

	/* data get */
	template<typename T> T & f_get_data(void);
	template<typename T> T & f_set_data(void);

	/* quick get */
	inline task_key_t f_get_uid() {
		return _c_uid;
	}
	inline uint32_t f_get_type() {
		return _i_type;
	}
	inline std::string f_get_name() {
		return _str_name;
	}
	inline ET_TASK_STATE f_get_status() {
		return _e_last_status;
	}
	inline std::string f_get_msg() {
		return _str_last_msg;
	}
	inline int f_get_progress() {
		return _i_last_progress;
	}

	/* State function */
	bool f_is_running(void);
	bool f_is_error(void);

	/* virtual methods */
	virtual void on_update(void);

	/* virtual methods */
	virtual void on_delete(void);
};

inline CT_TASK::task_key_t CT_TASK::f_reply_get_key(
		ST_TASK_STATUS const & in_s_status) {
	return CT_TASK::task_key_t(in_s_status.i_luid, in_s_status.i_muid);
}

template<typename T> T & CT_TASK::f_set_data(void) {
	_c_data = CT_GUARD<CT_OBJECT_REF>(
			static_cast<CT_OBJECT_REF*>(new CT_OBJECT<T>()));
	return *(static_cast<CT_OBJECT<T> *>(_c_data.get()));
}

template<typename T> T & CT_TASK::f_get_data(void) {
	M_ASSERT(_c_data.get());
	CT_OBJECT<T> * pc_tmp = dynamic_cast<CT_OBJECT<T>*>(_c_data.get());
	M_ASSERT(pc_tmp);
	return *(pc_tmp);
}

class CT_HOST_TASK_MANAGER {
	CT_SPINLOCK _c_spinlock;

	typedef std::map<CT_TASK::task_key_t const, CT_GUARD<CT_TASK> >::iterator _m_tasks_iterator_t;
	std::map<CT_TASK::task_key_t const, CT_GUARD<CT_TASK> > _m_tasks;
	atomic_t _i_uid;
	uint64_t _i_manager_uid;
public:
	CT_HOST_TASK_MANAGER();
	virtual ~CT_HOST_TASK_MANAGER();

	template<typename T>
	CT_GUARD<CT_TASK> f_new(std::string const& in_str_name,
			ST_TASK_STATUS const & in_s_status);
	template<typename T>
	CT_GUARD<CT_TASK> f_new(std::string const& in_str_name,
			CT_TASK::task_key_t const & in_c_uuid);
	template<typename T>
	CT_GUARD<CT_TASK> f_new(std::string const& in_str_name);
	template<typename T>
	CT_GUARD<CT_TASK> f_new(std::string const& in_str_name,
			uint32_t in_i_notif_id, CT_PORT & in_c_notif_int_port,
			uint32_t in_i_nb_step);

	virtual CT_GUARD<CT_TASK> f_new_task(CT_TASK::task_key_t const & in_c_uuid);
	virtual void on_new(CT_GUARD<CT_TASK> & in_pc_task);
	virtual void on_release(CT_GUARD<CT_TASK> & in_pc_task);

	void f_clear(void);

	bool f_has(CT_TASK::task_key_t const & in_c_uuid);
	bool f_has(ST_TASK_STATUS const & in_s_status);
	bool f_has(std::string const& in_str_name);

	CT_GUARD<CT_TASK> f_get(ST_TASK_STATUS const & in_s_status);
	CT_GUARD<CT_TASK> f_get(CT_TASK::task_key_t const & in_c_uuid);

	void f_release(CT_GUARD<CT_TASK> in_c_task);
	void f_set_manager_uid(uint64_t in_i_muid);
	uint64_t f_get_manager_uid(void);

	bool f_has_error(void);

};

template<typename T>
CT_GUARD<CT_TASK> CT_HOST_TASK_MANAGER::f_new(std::string const & in_str_name,
		CT_TASK::task_key_t const & in_c_uuid) {

	CT_GUARD_LOCK c_guard(_c_spinlock);

	//_DBG << pc_tmp;
	CT_GUARD<CT_TASK> c_tmp = f_new_task(in_c_uuid);
	c_tmp->f_set_data<T>();
	c_tmp->f_set_name(in_str_name);
	//_DBG << c_tmp.get();
	M_ASSERT(_m_tasks.find(in_c_uuid) == _m_tasks.end());
	//_DBG << "ADD TO LIST - START - pointer : " << pc_tmp;
	_m_tasks[in_c_uuid] = c_tmp;

	on_new(c_tmp);

	//_DBG << "ADD TO LIST - END :" << &_m_tasks[in_c_uuid];

	//_WARN << "New task:" << _V(in_c_uuid.first) << _V(in_c_uuid.second);
	//_DBG << c_tmp.get();
	return c_tmp;
}
template<typename T>
CT_GUARD<CT_TASK> CT_HOST_TASK_MANAGER::f_new(std::string const & in_str_name,
		ST_TASK_STATUS const & in_s_status) {
	return f_new<T>(in_str_name,
			CT_TASK::task_key_t(in_s_status.i_luid, in_s_status.i_muid));
}

template<typename T>
CT_GUARD<CT_TASK> CT_HOST_TASK_MANAGER::f_new(std::string const & in_str_name) {
	uint64_t i_tmp = M_ATOMIC_INC(_i_uid);
	CT_TASK::task_key_t c_key(i_tmp, _i_manager_uid);
	return f_new<T>(in_str_name, c_key);
}

/* Code friendly template */
template<typename T>
CT_GUARD<CT_TASK> CT_HOST_TASK_MANAGER::f_new(std::string const& in_str_name,
		uint32_t in_i_notif_id, CT_PORT & in_c_notif_int_port,
		uint32_t in_i_nb_step) {

	CT_GUARD<CT_TASK> c_tmp = f_new<T>(in_str_name);
	c_tmp->f_notification_set_id(in_i_notif_id);
	c_tmp->f_notification_set_internal_port(in_c_notif_int_port);
	c_tmp->f_step_set_nb(in_i_nb_step);
	return c_tmp;
}


}
}

template<>
CT_GUARD<master::core::CT_TASK>::CT_GUARD(master::core::CT_TASK * in_pointer);
template<>
void CT_GUARD<master::core::CT_TASK>::delete_pointer(
		void);

/* Print friendly key function */
std::ostream& operator<<(std::ostream& stream,
		master::core::CT_TASK::task_key_t const & key);
std::ostream& operator<<(std::ostream& stream,
		master::core::ST_TASK_STATUS const & status);

#endif /* TASK_HH_ */
