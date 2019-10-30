/***********************************************************************
 ** bind.h
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

#include <c/common.h>
#include <c/misc.h>
#include <cpp/guard.hh>
#include <cpp/lock.hh>
#include <list>

#ifndef BIND_HH_
#define BIND_HH_

//template<typename RET, typename A>
//class CT_BIND_CALL {
//}
/******************************
 * BIND CORE
 */

template<typename RET, typename A>
class CT_BIND_CORE {
public:
	virtual ~CT_BIND_CORE() {
	}
	virtual RET operator()(A) const = 0;

	virtual CT_BIND_CORE<RET, A> const * clone() const {
		return NULL;
	}
private:
	void operator=(CT_BIND_CORE<RET, A> const & other);
};
#if 0
template<typename A>
class CT_BIND_CORE<void, A> {
public:
	virtual ~CT_BIND_CORE() {
	}
	virtual void operator()(A) const = 0;

	virtual CT_BIND_CORE<RET, A> const * clone() const {
		return NULL;
	}
private:
	void operator=(CT_BIND_CORE<void, A> const & other);
};
#endif
#if 0
template<typename RET>
class CT_BIND_CORE<RET,void> {
public:
	virtual ~CT_BIND_CORE() {
	}
	virtual RET operator()() const = 0;

	virtual CT_BIND_CORE<RET,void>* clone() const = 0;

private:
	void operator=(CT_BIND_CORE<RET,void> const & other);
};
#endif

template<typename RET, typename A>
class CT_BIND_NULL: public CT_BIND_CORE<RET, A> {
	RET ret;
public:
	CT_BIND_NULL(RET in_ret) :
			ret(in_ret) {
	}

	RET operator()(A) const {
		return ret;
	}

	CT_BIND_CORE<RET, A> const * clone() const {
		return NULL;
	}
};

/******************************
 * BIND
 */
template<typename RET, typename T, typename A, typename TF = RET (T::*)(A)>
class CT_BIND: public CT_BIND_CORE<RET, A> {
	T * _pc_object;
	TF _pc_method;
public:
	CT_BIND(TF in_pc_method, T*in_pc_object) :
			_pc_object(in_pc_object), _pc_method(in_pc_method) {
	}
	~CT_BIND() {
	}
	RET operator()(A in_pc_core) const {
		//uint64_t i_time_start = f_get_time_ns64();

		int ec = (_pc_object->*_pc_method)(in_pc_core);

		//uint64_t i_time_end = f_get_time_ns64();
		//if (float(i_time_end - i_time_start) / 1e6 > 1) {
		//	_DBG << "Time bind() :" << float(i_time_end - i_time_start) / 1e6;
		//}
		return ec;
	}
	CT_BIND_CORE<RET, A> const * clone() const {
		return new CT_BIND(_pc_method, _pc_object);
	}
private:
	void operator=(CT_BIND<RET, T, A, TF> const & other);
};

template<typename T, typename A, typename TF>
class CT_BIND<void, T, A, TF> : public CT_BIND_CORE<void, A> {
	T * _pc_object;
	TF _pc_method;

public:
	CT_BIND(TF in_pc_method, T*in_pc_object) :
			_pc_object(in_pc_object), _pc_method(in_pc_method) {
	}
	~CT_BIND() {
	}
	void operator()(A in_pc_core) const {
		(_pc_object->*_pc_method)(in_pc_core);
	}
	CT_BIND_CORE<void, A> const * clone() const {
		return new CT_BIND(_pc_method, _pc_object);
	}
private:
	void operator=(CT_BIND<void, T, A, TF> const & other);
};
#if 0
template<typename RET, typename T,
typename TF = RET (T::*)()>
class CT_BIND<RET,T,void>: public CT_BIND_CORE<RET, void> {
	T * _pc_object;
	TF _pc_method;
public:
	CT_BIND(TF in_pc_method, T*in_pc_object) :
	_pc_object(in_pc_object), _pc_method(in_pc_method) {
	}
	~CT_BIND() {
	}
	RET operator()() const {
		return (_pc_object->*_pc_method)();
	}
	CT_BIND_CORE<RET, void>* clone() const {
		return new CT_BIND(_pc_method, _pc_object);
	}
private:
	void operator=(CT_BIND<RET, T, void, TF> const & other);
};
#endif

/******************************
 * SLOT
 */
template<typename RET, typename A>
class CT_SLOT {
	CT_GUARD<CT_BIND_CORE<RET, A> const> _pc_bind;
public:
	CT_SLOT() {
		_pc_bind = NULL;
		//_DBG << "INIT" << _V(this);
	}
#if 1
	CT_SLOT(CT_SLOT const & other) :
			_pc_bind(other._pc_bind) {
		//_DBG << "CLONE 2.1 " << _pc_bind << _V(this);
	}
#endif

	CT_SLOT(CT_BIND_CORE<RET, A> const & other) {

		CT_BIND_CORE<RET, A> const * pc_tmp = other.clone();
		if (pc_tmp) {
			_pc_bind = pc_tmp; //CT_GUARD<CT_BIND_CORE<RET, A> const>(pc_tmp);
		}
		//_DBG << "CLONE 2.2 " << _pc_tmp << _V(this);
	}

	virtual ~CT_SLOT() {
		_pc_bind = NULL;
	}

	virtual RET operator()(A in_pc_node) const {
		//uint64_t i_time_start = f_get_time_ns64();
		//_DBG << "Executing callback " << _pc_bind.get() << _V(this);
		M_ASSERT(_pc_bind);
		int ec = _pc_bind->operator ()(in_pc_node);

		//uint64_t i_time_end = f_get_time_ns64();
		//if (float(i_time_end - i_time_start) / 1e6 > 1) {
		//	_DBG << "Time slot() :" << float(i_time_end - i_time_start) / 1e6;
		//}

		return ec;
	}

	operator bool() const {
		return _pc_bind;
	}

	/* Private assignment */
	void operator=(CT_SLOT<RET, A> const & other) {
		//_DBG << "=" << _V(this);
		_pc_bind = other._pc_bind;
	}

	CT_SLOT & operator=(std::nullptr_t) {
		_pc_bind = NULL;
		return *this;
	}

	operator CT_BIND_CORE<RET,A> const *() {
		return _pc_bind.get();
	}
};

template<typename A>
class CT_SLOT<void, A> {
	CT_GUARD<CT_BIND_CORE<void, A> const> _pc_bind;
public:
	CT_SLOT() {
		_pc_bind = NULL;
	}
	CT_SLOT(CT_SLOT const & other) :
			_pc_bind(other._pc_bind) {
	}
	CT_SLOT(CT_BIND_CORE<void, A> const & other) {

		CT_BIND_CORE<void, A> const * pc_tmp = other.clone();
		if (pc_tmp) {
			_pc_bind = pc_tmp;
		}
	}
	virtual ~CT_SLOT() {
		_pc_bind = NULL;
	}
	virtual void operator()(A in_pc_node) const {
		M_ASSERT(_pc_bind);
		_pc_bind->operator ()(in_pc_node);
	}
	operator bool() const {
		return _pc_bind;
	}
	void operator=(CT_SLOT<void, A> const & other) {
		_pc_bind = other._pc_bind;
	}
	CT_SLOT & operator=(std::nullptr_t) {
		_pc_bind = NULL;
		return *this;
	}
	operator CT_BIND_CORE<void,A> const *() {
		return _pc_bind.get();
	}
};

#if 0
template<typename A>
class CT_SIGNAL_SLOT {
	CT_GUARD<CT_BIND_CORE<void, A> const> _pc_bind;
public:
	CT_SIGNAL_SLOT() {
		_pc_bind = NULL;
		//_DBG << "INIT" << _V(this);
	}
#if 1
	CT_SIGNAL_SLOT(CT_SLOT const & other) :
	_pc_bind(other._pc_bind) {
		//_DBG << "CLONE 2.1 " << _pc_bind << _V(this);
	}
#endif

	CT_SIGNAL_SLOT(CT_BIND_CORE<void, A> const & other) {

		CT_BIND_CORE<void, A> const * pc_tmp = other.clone();
		if (pc_tmp) {
			_pc_bind = pc_tmp; //CT_GUARD<CT_BIND_CORE<RET, A> const>(pc_tmp);
		}
		//_DBG << "CLONE 2.2 " << _pc_tmp << _V(this);
	}
#if 0
	CT_SLOT(CT_BIND_CORE<RET, A> * in_pc_other) :
	_pc_bind(in_pc_other) {
		_DBG << "SET " << _pc_bind << _V(this);
	}
#endif
	virtual ~CT_SIGNAL_SLOT() {
		_pc_bind = NULL;
	}

	virtual void operator()(A in_pc_node) const {
		//_DBG << "Executing callback " << _pc_bind.get() << _V(this);
		M_ASSERT(_pc_bind);
		_pc_bind->operator ()(in_pc_node);
	}

	operator bool() const {
		return _pc_bind;
	}

	/* Private assignment */
	void operator=(CT_SIGNAL_SLOT<A> const & other) {
		//_DBG << "=" << _V(this);
		_pc_bind = other._pc_bind;
	}

	CT_SIGNAL_SLOT & operator=(std::nullptr_t) {
		_pc_bind = NULL;
		return *this;
	}

	operator CT_BIND_CORE<void,A> const *() {
		return _pc_bind.get();
	}
};
#endif
template<typename A>
class CT_SIGNAL {
	mutable CT_SPINLOCK _c_lock;

	typedef typename std::list<CT_SLOT<int, A> const *>::const_iterator _l_bind_slot_iterator;
	std::list<CT_SLOT<int, A> const *> _l_slots;

	typedef typename std::list<CT_SIGNAL<A> const *>::const_iterator _l_bind_signal_iterator;
	std::list<CT_SIGNAL<A> const *> _l_signals;

public:

	void operator()(A in) const {
		CT_GUARD_LOCK c_guard(_c_lock);

		for (_l_bind_slot_iterator pc_it = _l_slots.begin();
				pc_it != _l_slots.end(); pc_it++) {
			(*pc_it)->operator()(in);
		}

		for (_l_bind_signal_iterator pc_it = _l_signals.begin();
				pc_it != _l_signals.end(); pc_it++) {
			(*pc_it)->operator()(in);
		}
	}

	void connect(CT_SLOT<int, A> const & in_c_slot) {
		CT_GUARD_LOCK c_guard(_c_lock);
		_l_slots.push_back(&in_c_slot);
	}
	void connect(CT_SIGNAL<A> const & in_c_signal) {
		CT_GUARD_LOCK c_guard(_c_lock);
		_l_signals.push_back(&in_c_signal);
	}

	void disconnect(CT_SLOT<int, A> const & in_c_slot) {
		CT_GUARD_LOCK c_guard(_c_lock);
		_l_slots.remove(&in_c_slot);
	}
	void disconnect(CT_SIGNAL<A> const & in_c_signal) {
		CT_GUARD_LOCK c_guard(_c_lock);
		_l_signals.remove(&in_c_signal);
	}

	void disconnect() {
		CT_GUARD_LOCK c_guard(_c_lock);
		_l_slots.clear();
		_l_signals.clear();
	}

	operator bool() const {
		return size_t(_l_slots.size() + _l_signals.size());
	}
};

#endif /* BIND_HH_ */
