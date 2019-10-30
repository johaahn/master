/***********************************************************************
 ** resource
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

/**
 * @file resource.cc
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Jan 27, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <host.hh>
#include <resource.hh>
#include <cpp/guard.hh>
//TODO Move to define
#ifndef FF_LOW_MEMORY
#define C_PORT_RESOURCE_POOL_LARGE_NB_ELEMENT (32)
#define C_PORT_RESOURCE_POOL_SMALL_NB_ELEMENT (1024*8)
#define C_PORT_RESOURCE_POOL_RESOURCE_NB_ELEMENT (1024*1024)
#else
#define C_PORT_RESOURCE_POOL_LARGE_NB_ELEMENT (16)
#define C_PORT_RESOURCE_POOL_SMALL_NB_ELEMENT (256)
#define C_PORT_RESOURCE_POOL_RESOURCE_NB_ELEMENT (1024)
#endif

#define C_PORT_RESOURCE_POOL_SMALL_SIZE 1024 //4 Ko
#define C_PORT_RESOURCE_POOL_LARGE_SIZE (32*1024*1024) //393216 Ko
#define C_PORT_RESOURCE_POOL_RESOURCE_SIZE (sizeof(class CT_CPU_NODE_RESOURCE))


typedef boost::singleton_pool<boost::fast_pool_allocator_tag,
		C_PORT_RESOURCE_POOL_LARGE_SIZE,
		boost::default_user_allocator_new_delete, CT_SPINLOCK,
		C_PORT_RESOURCE_POOL_LARGE_NB_ELEMENT, C_PORT_RESOURCE_POOL_LARGE_NB_ELEMENT> C_PORT_RESOURCE_POOL_LARGE;

typedef boost::singleton_pool<boost::fast_pool_allocator_tag,
		C_PORT_RESOURCE_POOL_SMALL_SIZE, boost::default_user_allocator_new_delete,
		CT_SPINLOCK, C_PORT_RESOURCE_POOL_SMALL_NB_ELEMENT, C_PORT_RESOURCE_POOL_SMALL_NB_ELEMENT> C_PORT_RESOURCE_POOL_SMALL;

typedef boost::singleton_pool<boost::fast_pool_allocator_tag,
		C_PORT_RESOURCE_POOL_RESOURCE_SIZE,
		boost::default_user_allocator_new_delete, CT_SPINLOCK,
		C_PORT_RESOURCE_POOL_RESOURCE_NB_ELEMENT, C_PORT_RESOURCE_POOL_RESOURCE_NB_ELEMENT> C_PORT_RESOURCE_POOL;

using namespace bml;
/***********************************************************************
 * CT_CPU_NODE_RESOURCE CLASS
 ***********************************************************************/

/* Customize GUARD init - speed improvement due to usage allocation */
template<>
CT_GUARD<CT_CPU_NODE_RESOURCE>::CT_GUARD(CT_CPU_NODE_RESOURCE * in_pointer) {
	_WARN << this;
	init(&(in_pointer->s_usage));
}

CT_CPU_NODE_RESOURCE::CT_CPU_NODE_RESOURCE(size_t in_sz_data,
		CT_CPU_RESOURCE * in_pc_resource) :
		CT_NODE_RESOURCE(in_pc_resource), s_usage(false, this) {

	_sz_data = 0;
	_sz_allocated_data = 0;
	_pv_data = NULL;

	if (in_sz_data) {
		resize(in_sz_data);
	}

}
//static int gi_cnt = 0;
CT_CPU_NODE_RESOURCE::~CT_CPU_NODE_RESOURCE() {
	if (_sz_data) {
		CT_MEMTRACK::f_register_free(s_memtrack_info);
		//_DBG << _V(gi_cnt--);
		if (_sz_data > C_PORT_RESOURCE_POOL_LARGE_SIZE) {
			__libc_free(_pv_data);
		} else if (_sz_data > C_PORT_RESOURCE_POOL_SMALL_SIZE) {
			C_PORT_RESOURCE_POOL_LARGE::free(_pv_data);
		} else {
			C_PORT_RESOURCE_POOL_SMALL::free(_pv_data);
			//D("Delete POOL resource: %p", _pv_data);
		}
	}
	//D("Delete CUST resource: %p", this);
}

void CT_CPU_NODE_RESOURCE::resize(size_t in_sz_data) {
	//D("Allocating %d bytes", (int)in_sz_data);
	if (in_sz_data > (size_t) _sz_allocated_data) {
		if (_pv_data) {
			CT_MEMTRACK::f_register_free(s_memtrack_info);
			//_DBG << _V(gi_cnt--);
			if (_sz_allocated_data > C_PORT_RESOURCE_POOL_LARGE_SIZE) {
				__libc_free(_pv_data);
			} else if (_sz_allocated_data > C_PORT_RESOURCE_POOL_SMALL_SIZE) {
				C_PORT_RESOURCE_POOL_LARGE::free(_pv_data);
			} else {
				C_PORT_RESOURCE_POOL_SMALL::free(_pv_data);
			}

		}
		/* Allocate memory or pool */
		if (!in_sz_data) {
			_pv_data = NULL;
			_sz_allocated_data = 0;
		} else if (in_sz_data > C_PORT_RESOURCE_POOL_LARGE_SIZE) {
			_pv_data = __libc_malloc(in_sz_data);
			_sz_allocated_data = in_sz_data;
		} else if (in_sz_data > C_PORT_RESOURCE_POOL_SMALL_SIZE) {
			//_DBG << "Allocate large";
			_pv_data = C_PORT_RESOURCE_POOL_LARGE::malloc();
			_sz_allocated_data = C_PORT_RESOURCE_POOL_LARGE_SIZE;
		} else {
			_pv_data = C_PORT_RESOURCE_POOL_SMALL::malloc();
			_sz_allocated_data = C_PORT_RESOURCE_POOL_SMALL_SIZE;
		}
		if(_sz_allocated_data) {
			CT_MEMTRACK::f_register_malloc(s_memtrack_info, _pv_data, _sz_allocated_data, "CT_CPU_NODE_RESOURCE_CONTENT");
			//CT_MEMTRACK::f_set_name(_pv_data, "CT_CPU_NODE_RESOURCE_CONTENT");
			//_DBG << _V(gi_cnt++);
		}
	}
	_sz_data = in_sz_data;
	if(_sz_data) {
		if(!_pv_data) {
			throw std::bad_alloc();
		}
	}
	//_DBG << "ALLOC" << _V((uintptr_t)_pv_data) << _V(in_sz_data);
}

char * CT_CPU_NODE_RESOURCE::mmap(off_t in_off_data, size_t in_sz_data) const {
	//DBG_LINE();
	char * pc_tmp = &(((char*) _pv_data)[in_off_data]);
	//_DBG << _V((uintptr_t)pc_tmp) << _V(_pv_data)<< _V(in_off_data);
	return pc_tmp;
}

void * CT_CPU_NODE_RESOURCE::operator new(size_t in_i_size) {
	//_DBG << "Alloc";
	/* Allocate node from pool */
	CT_CPU_NODE_RESOURCE* pc_tmp =
			(CT_CPU_NODE_RESOURCE*) C_PORT_RESOURCE_POOL::malloc();
	if (!pc_tmp) {
		throw std::bad_alloc();
	}
	//CT_MEMTRACK::f_register_malloc(pc_tmp, in_i_size);
	//CT_MEMTRACK::f_set_name(pc_tmp, "CT_CPU_NODE_RESOURCE");

	return pc_tmp;
}
void CT_CPU_NODE_RESOURCE::operator delete(void * in_pv) throw () {
	if (in_pv == 0)
		return;
	//CT_MEMTRACK::f_register_free(in_pv);
	C_PORT_RESOURCE_POOL::free(in_pv);
	//D("Delete node %p (%d)", in_pv, gi_port_node_cnt);
}

#if 0
void CT_CPU_NODE_RESOURCE::unmmap(off_t in_off_data, size_t in_sz_data) const {
	return;
}
#endif

/***********************************************************************
 * CT_CPU_RESOURCE CLASS
 ***********************************************************************/

CT_CPU_RESOURCE::CT_CPU_RESOURCE() :
		CT_RESOURCE("cpu") /*,  _c_resource_poll()*/{
}
CT_CPU_RESOURCE::~CT_CPU_RESOURCE() {
}

/* common methods */
CT_GUARD<node_resource<CT_GUARD> > CT_CPU_RESOURCE::f_alloc_resource(
		size_t in_i_size) {
	CT_CPU_NODE_RESOURCE * pc_tmp = new CT_CPU_NODE_RESOURCE(0, this);
	if (!pc_tmp) {
		CRIT("Unable to allocate gpu resource");
		throw std::bad_alloc();
	}
	pc_tmp->resize(in_i_size);
	pc_tmp->f_set_status(E_RESOURCE_NODE_BUSY);

	CT_GUARD<node_resource<CT_GUARD> > c_tmp(&pc_tmp->s_usage);

	/* Decrement usage */
	pc_tmp->s_usage.unuse();

	return c_tmp;
}
;

/***********************************************************************
 * CT_NODE_RESSOURCE CLASS
 ***********************************************************************/
CT_NODE_RESOURCE::CT_NODE_RESOURCE() :
		node_resource() {
	_e_status = E_RESOURCE_NODE_FREE;
	_pc_resource = NULL;
}

CT_NODE_RESOURCE::CT_NODE_RESOURCE(CT_RESOURCE * in_pc_resource) :
		node_resource() {
	_pc_resource = NULL;
	f_set_resource(in_pc_resource);
}

CT_NODE_RESOURCE::~CT_NODE_RESOURCE() {
	M_ASSERT(_pc_resource);
	if (f_node_dbg_in(this)) {
		_DBG << "DELETE RESOURCE " << this;
	}
	if (_pc_resource) {
		_pc_resource->f_unregister(this);
	}
}

void CT_NODE_RESOURCE::f_set_resource(CT_RESOURCE * in_pc_resource) {
	M_ASSERT(_pc_resource == NULL);
	_pc_resource = in_pc_resource;
	_pc_resource->f_register(this);
#ifdef FF_RESOURCE_TRACKING
	CT_DEBUG::f_get_backtrace(s_backtrace);
#endif
}

/***********************************************************************
 * CT_RESSOURCE CLASS
 ***********************************************************************/
CT_FILE_RESOURCE::CT_FILE_RESOURCE(std::string const & in_str_buffer) :
		node_buffer_resource<CT_GUARD>(NULL, 0) {
	int fd;
	struct stat sb;

	fd = open(in_str_buffer.c_str(), O_RDONLY);
	if (fd == -1) {
		throw std::runtime_error(strerror(errno));
	}

	if (fstat(fd, &sb) == -1) {
		throw std::runtime_error(strerror(errno));
	}

	if (!S_ISREG(sb.st_mode)) {
		throw std::runtime_error("not a file");
	}

	_pv_data = (void *) ::mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if ((char*) _pv_data == MAP_FAILED) {
		throw std::runtime_error(strerror(errno));
	}
	_sz_data = sb.st_size;

	if (close(fd) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

CT_FILE_RESOURCE::~CT_FILE_RESOURCE() {
	if (munmap((char*) _pv_data, _sz_data) == -1) {
		//throw std::runtime_error(strerror(errno));
    M_BUG();
	}
	_pv_data = NULL;
	_sz_data = 0;
}

/***********************************************************************
 * CT_RESSOURCE CLASS
 ***********************************************************************/

CT_RESOURCE::CT_RESOURCE() {
	_str_name = "";

}

CT_RESOURCE::CT_RESOURCE(std::string const & in_str_name) {
	_str_name = in_str_name;

}

CT_RESOURCE::~CT_RESOURCE() {
#ifdef FF_RESOURCE_TRACKING
	_WARN << _l_resources.size() << " resources still used in " << _str_name;
	for (_l_resources_iterator_t pc_it = _l_resources.begin();
			pc_it != _l_resources.end(); pc_it++) {
		_WARN << "Resource still used : " << (pc_it->second);
		CT_DEBUG::f_print_backtrace_cxx(pc_it->second->s_backtrace);
	}
#endif
	//M_ASSERT(!_l_resources.size());
}

CT_GUARD<node_resource<CT_GUARD> > CT_RESOURCE::f_alloc_resource(
		size_t in_i_size) {
	M_BUG();
	return CT_GUARD<node_resource<CT_GUARD> >();
}

node_resource_segment<CT_GUARD> CT_RESOURCE::f_alloc(size_t in_i_size) {
	return node_resource_segment<CT_GUARD>(f_alloc_resource(in_i_size));
}

int CT_RESOURCE::f_register(CT_NODE_RESOURCE * in_pc_node) {
#ifdef FF_RESOURCE_TRACKING
	CT_GUARD_LOCK c_guard(_c_lock);
	_str_name = "1";
	_l_resources[(uintptr_t)in_pc_node] = in_pc_node;
#endif
	//use();
	return EC_SUCCESS;
}

int CT_RESOURCE::f_unregister(CT_NODE_RESOURCE * in_pc_node) {
#ifdef FF_RESOURCE_TRACKING
	CT_GUARD_LOCK c_guard(_c_lock);
	_l_resources.erase((uintptr_t)in_pc_node);
#endif
	//unuse();
	return EC_SUCCESS;
}
#if 0
struct ST_RESOURCE_SIZE_STAT {
	int i_nb;
	int i_size;
	std::list<CT_NODE_RESOURCE *> l_elem;

	ST_RESOURCE_SIZE_STAT() {
		i_nb = 0;
		i_size = 0;
	}
};

void CT_RESOURCE::f_display_stats(void) {
	CT_GUARD_LOCK c_guard(_c_lock);
	std::map<int, struct ST_RESOURCE_SIZE_STAT> m_size;
	typedef std::map<int, struct ST_RESOURCE_SIZE_STAT>::iterator m_size_iterator_t;
	for (_l_resources_iterator_t pc_it = _l_resources.begin();
				pc_it != _l_resources.end(); pc_it++) {
		m_size[pc_it->second->size()].i_nb += 1;
		m_size[pc_it->second->size()].i_size += pc_it->second->size();
		m_size[pc_it->second->size()].l_elem.push_back(pc_it->second);
 	}

	_DBG << "Display Resource stats of "<<f_get_name();
	for (m_size_iterator_t pc_it = m_size.begin();
					pc_it != m_size.end(); pc_it++) {
		_DBG << " - " << pc_it->second.i_nb << " block of " << pc_it->second.i_size << " bytes";
	}
}
#endif
