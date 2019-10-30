/***********************************************************************
 ** lock
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
#ifndef LOCK_HH_
#define LOCK_HH_

/**
 * @file lock.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 03 Jan, 2014
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#ifdef D
#undef D
#endif

#ifndef FF_NOBOOST
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>
#else

#endif
#define D(in_str_format,...) DBG (in_str_format, ##__VA_ARGS__)

#include <c/common.h>
/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/

class CT_LOCK {
public:
	/* virtual methods */
	virtual ~CT_LOCK() {

	}

	/* mutex methods */
	virtual void lock() = 0;
	virtual bool try_lock() = 0;
	virtual void unlock() = 0;

};

class CT_MUTEX: public CT_LOCK {
	boost::mutex _c_lock;
public:
  CT_MUTEX() {
	}
  CT_MUTEX(const CT_MUTEX & other) {
    /* COPY OF MUTEX IS NOT ALLOWED*/
	}
	void lock() {
		_c_lock.lock();

	}
	bool try_lock() {
		return _c_lock.try_lock();
	}
	void unlock() {
		_c_lock.unlock();
	}
};

class CT_SPINLOCK: public CT_LOCK {
	boost::detail::spinlock _c_lock = BOOST_DETAIL_SPINLOCK_INIT;
public:
	CT_SPINLOCK() {
	}
  CT_SPINLOCK(const CT_SPINLOCK & other) {
    /* COPY OF SPINLOCK IS NOT ALLOWED*/
	}
	void force_init() {
    M_BUG();
	}

	void lock() {
		_c_lock.lock();

	}
	bool try_lock() {
		return _c_lock.try_lock();
	}
	void unlock() {
		_c_lock.unlock();
	}
};

class CT_GUARD_LOCK {
	CT_LOCK & _c_lock;

public:
	CT_GUARD_LOCK(CT_LOCK & in_c_lock) : _c_lock(in_c_lock) {
		_c_lock.lock();
	}
	~CT_GUARD_LOCK() {
		_c_lock.unlock();
	}

	CT_LOCK & get_lock() {
		return _c_lock;
	}
};

class CT_GUARD_UNLOCK {
	CT_LOCK & _c_lock;

public:
	CT_GUARD_UNLOCK(CT_GUARD_LOCK & in_c_guard_lock) : _c_lock(in_c_guard_lock.get_lock()) {
		_c_lock.unlock();
	}
	explicit CT_GUARD_UNLOCK(CT_LOCK & in_c_lock) : _c_lock(in_c_lock) {
		_c_lock.unlock();
	}


	~CT_GUARD_UNLOCK() {
		_c_lock.lock();
	}

	CT_LOCK & get_lock() {
		return _c_lock;
	}
};



union UT_RWTICKET
{
	uint32_t i_value;

	__extension__ struct
	{
		union {
			struct {
				uint8_t i_w;
				uint8_t i_r;
			} s_rw;
			uint16_t i_rw;
		} u_rw;
		uint8_t i_users;
		uint8_t i_dummy;
	} s;

	UT_RWTICKET() {
		M_ASSERT(sizeof(s) == sizeof(i_value));
		i_value = 0;
	}
	UT_RWTICKET(uint8_t const & i_users, uint8_t const & i_read, uint8_t const & i_write) {
		s.u_rw.s_rw.i_w = i_write;
		s.u_rw.s_rw.i_r = i_read;
		s.i_users = i_users;
		s.i_dummy = 0;
	}

	UT_RWTICKET(UT_RWTICKET const & other) {
		i_value = other.i_value;
	}

	uint8_t f_inc_user(void) {
		uint32_t me = __sync_fetch_and_add((&i_value), (1<<16));
		return me >> 16;
	}
} M_PACKED;

class CT_W_SPINLOCK : public CT_LOCK {
	UT_RWTICKET & _u_ticket;

public:
	CT_W_SPINLOCK(UT_RWTICKET & in_u_ticket) : _u_ticket(in_u_ticket){

	}
	void lock()
	{
		/* Push +1 in users */
		uint8_t val = _u_ticket.f_inc_user();

		/* While write != old value of user, yield */
		for (int k=0; val != _u_ticket.s.u_rw.s_rw.i_w; k++){
			 boost::detail::yield( k );
		}
	}

	void unlock()
	{
		/* Copy ticket */
		UT_RWTICKET t = _u_ticket;

		/* Memory update */
		__sync_synchronize();

		/* Add +1 in write, allowing next write to start */
		t.s.u_rw.s_rw.i_w++;

		/* Add +1 in read, allowing next write to start */
		t.s.u_rw.s_rw.i_r++;

		/* Update read and write fields */
		_u_ticket.s.u_rw.i_rw = t.s.u_rw.i_rw;
	}

	bool try_lock()
	{
		uint8_t i_me = _u_ticket.s.i_users;
		uint8_t i_menew = i_me + 1;
		uint8_t i_read = _u_ticket.s.u_rw.s_rw.i_r;

		UT_RWTICKET u_cmp(i_me, i_read, i_me);
		UT_RWTICKET u_cmpnew(i_menew, i_read, i_me);

		if (__sync_val_compare_and_swap((&_u_ticket.i_value), u_cmp.i_value, u_cmpnew.i_value) == u_cmp.i_value) return true;
		return false;
	}

};


class CT_R_SPINLOCK : public CT_LOCK {
	UT_RWTICKET & _u_ticket;

public:
	CT_R_SPINLOCK(UT_RWTICKET & in_u_ticket) : _u_ticket(in_u_ticket){

	}

	 void lock()
	{
		 /* Add new ticket, +1 in user */
		uint8_t val = _u_ticket.f_inc_user();

		/* While write != old value of user, yield */
		for (int k=0; val != _u_ticket.s.u_rw.s_rw.i_r; k++){
			 boost::detail::yield( k );
		}
		/* Inc read, authorize next read */
		_u_ticket.s.u_rw.s_rw.i_r++;
	}

	void unlock()
	{
		/* Inc write, atomic due to multiple read */
		__sync_add_and_fetch(&(_u_ticket.s.u_rw.s_rw.i_w), 1);
	}

	bool try_lock()
	{
		uint8_t i_me = _u_ticket.s.i_users;
		uint8_t i_write = _u_ticket.s.u_rw.s_rw.i_w;
		uint8_t i_menew = i_me + 1;

		UT_RWTICKET u_cmp(i_me, i_me, i_write);
		UT_RWTICKET u_cmpnew(i_menew, i_menew, i_write);

		if (__sync_val_compare_and_swap((&_u_ticket.i_value), u_cmp.i_value, u_cmpnew.i_value) == u_cmp.i_value) return true;
		return false;
	}
};

class CT_RW_SPINLOCK {

	UT_RWTICKET _u_ticket;

	CT_R_SPINLOCK _c_rd_lock;
	CT_W_SPINLOCK _c_wr_lock;

public:
	CT_RW_SPINLOCK() : _c_rd_lock(_u_ticket), _c_wr_lock(_u_ticket) {
	}

	CT_R_SPINLOCK & read() {
		return _c_rd_lock;
	}

	CT_W_SPINLOCK & write() {
		return _c_wr_lock;
	}
};

/***********************************************************************
 * Functions
 ***********************************************************************/

#endif /* LOCK_HH_ */
