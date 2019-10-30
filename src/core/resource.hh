/***********************************************************************
 ** Resource
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
#ifndef RESOURCE_HH_
#define RESOURCE_HH_

/**
 * @file resource.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Jan 27, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <c/common.h>
#include <node.hh>
#include <port.hh>
#include <plugin.hh>

#include <boost/pool/pool_alloc.hpp>

/***********************************************************************
 * Defines
 ***********************************************************************/
//#define FF_RESOURCE_TRACKING 1

//TODO move into dedictaed file
enum ET_RESOURCE {
	E_RESOURCE_DEFAULT,
	E_RESOURCE_GPU,
};

/***********************************************************************
 * Types
 ***********************************************************************/
class CT_RESOURCE;

enum ET_RESOURCE_NODE_STATUS {
	E_RESOURCE_NODE_BUSY,
	E_RESOURCE_NODE_FREE,
};

typedef bml::node_resource_segment<CT_GUARD> CT_NODE_RESOURCE_SEGMENT;

/******************************
 * MMAP BUFFER RESOURCE
 */
class CT_FILE_RESOURCE: public bml::node_buffer_resource<CT_GUARD> {
public:
	CT_FILE_RESOURCE(std::string const & in_str_buffer);
	virtual ~CT_FILE_RESOURCE();
};

class CT_NODE_RESOURCE: public bml::node_resource<CT_GUARD> {
protected:
	/* Resource pointer */
	CT_RESOURCE * _pc_resource;

	/* Status of resource */
	enum ET_RESOURCE_NODE_STATUS _e_status;


protected:
	void f_set_resource(CT_RESOURCE * in_pc_resource);


public:
	/* Memtrack info */
	ST_MEMTRACK_INFO s_memtrack_info;
#ifdef FF_RESOURCE_TRACKING
	/* backtrace allocation */
	ST_DEBUG_BACKTRACE s_backtrace;
#endif

public:
	CT_NODE_RESOURCE();
	CT_NODE_RESOURCE(CT_RESOURCE * in_pc_resource);
	virtual ~CT_NODE_RESOURCE();

	/* Resource identification */
	virtual int f_get_id(void) {
		return E_RESOURCE_DEFAULT;
	}

	inline CT_RESOURCE * f_get_resource(void) {
		return _pc_resource;
	}

	/* Quick set name of resource */
	void f_set_status(enum ET_RESOURCE_NODE_STATUS in_e_status) {
		_e_status = in_e_status;
	}
};




class CT_RESOURCE: public CT_PLUGIN {
	/* Name of resource */
	std::string _str_name;

#ifdef FF_RESOURCE_TRACKING
	/* List of resource */
		typedef boost::pool_allocator<std::pair<uintptr_t, CT_NODE_RESOURCE *>, boost::default_user_allocator_new_delete, CT_SPINLOCK,
				1024*1024, 0> node_resource_pool_t;
	//typedef boost::singleton_pool<boost::fast_pool_allocator_tag, sizeof(CT_NODE_RESOURCE *)> node_resource_pool_t;
	typedef std::map<uintptr_t, CT_NODE_RESOURCE *>::iterator _l_resources_iterator_t;
	std::map<uintptr_t, CT_NODE_RESOURCE *, std::less<uintptr_t>, node_resource_pool_t > _l_resources;

#endif

protected:
	/* list lock */
	CT_SPINLOCK _c_lock;


protected:
	void selfdestruct(void);

public:
	CT_RESOURCE();
	CT_RESOURCE(std::string const & in_str_name);
	virtual ~CT_RESOURCE();

	/* common methods */
	virtual CT_NODE_RESOURCE_SEGMENT f_alloc(size_t in_i_size);

	/* common methods */
	virtual CT_GUARD<bml::node_resource<CT_GUARD> > f_alloc_resource(size_t in_i_size);

	/* Registering node resource */
	virtual int f_register(CT_NODE_RESOURCE * in_pc_node);

	/* Unregistering node resource */
	virtual int f_unregister(CT_NODE_RESOURCE * in_pc_node);

	/* Quick get */
	std::string const &f_get_name() { return _str_name; };

	/* Statistics display */
	//void f_display_stats(void);
};


/******************************
 * CPU RESOURCE
 */
class CT_CPU_RESOURCE: public CT_RESOURCE {
	//boost::pool _c_resource_poll;
public:
	CT_CPU_RESOURCE();
	virtual ~CT_CPU_RESOURCE();

	/* common methods */
	CT_GUARD<bml::node_resource<CT_GUARD> >  f_alloc_resource(size_t in_i_size);



};

/******************************
 * MALLOC RESOURCE
 */

class CT_CPU_NODE_RESOURCE: public CT_NODE_RESOURCE {
	size_t _sz_allocated_data;
	void * _pv_data;

public:
	ST_GUARD_USAGE s_usage;
public:
	CT_CPU_NODE_RESOURCE(size_t in_sz_data, CT_CPU_RESOURCE * in_pc_resource);
	virtual ~CT_CPU_NODE_RESOURCE();

	void resize(size_t in_sz_data);

	/* mmap methods */
	char * mmap(off_t in_off_data = 0, size_t in_sz_data = 0) const;
	//void unmmap(off_t in_off_data = 0, size_t in_sz_data = 0) const;

	/* Redefined allocators */
	void * operator new(size_t in_i_size);
	void operator delete(void * in_pv) throw ();
};

#endif /* RESOURCE_HH_ */
