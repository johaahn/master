/***********************************************************************
 ** node
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
#ifndef NODE_HH_
#define NODE_HH_

/**
 * @file node.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Feb 9, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <c/common.h>
#include <bml_node.hh>
#include <c/third_ezxml.h>
#include <cpp/guard.hh>
/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/


class CT_NODE: public bml::node<std::string, CT_GUARD> {
public:
	enum ET_NODE_EXT {
		E_NODE_EXT_TIME = 0, E_NODE_EXT_NS,  E_NODE_EXT_TYPE, E_NODE_EXT_OFFSET_DATA, E_NODE_EXT_HEADER, E_NODE_EXT_ROUTING_MAP,
	};
	enum ET_NODE_ID {
		E_NODE_ID_CHILD_XML = 0, E_NODE_ID_NAME,
	};
	enum ET_NODE_TYPE {
		E_NODE_TYPE_UINT8 = 0,
		E_NODE_TYPE_INT8,
		E_NODE_TYPE_UINT16,
		E_NODE_TYPE_INT16,
		E_NODE_TYPE_UINT32,
		E_NODE_TYPE_INT32,
		E_NODE_TYPE_UINT64,
		E_NODE_TYPE_INT64,
		E_NODE_TYPE_FLOAT,
		E_NODE_TYPE_DOUBLE,
		E_NODE_TYPE_LONG_DOUBLE,
		E_NODE_TYPE_CHAR,
		E_NODE_TYPE_WCHAR,
		E_NODE_TYPE_RAW,
	};
private:
	enum ET_NODE_TYPE _e_type;

private:
  void operator=(const CT_NODE&);

public:

	typedef std::vector<CT_NODE*> childs_result;
	typedef std::vector<CT_NODE*>::iterator childs_iterator;

	CT_NODE();
	virtual ~CT_NODE();
#if 0
	/* operators */

	CT_NODE & operator()(uint64_t in_i_id, uint64_t in_i_indice = 0);
#endif
	/* get operator */
	CT_NODE & operator()(std::string in_str_name, uint64_t in_i_indice = 0);

	/* common assignment */
	CT_NODE & operator=(std::string const & other);
	CT_NODE & operator=(int const & other);
	CT_NODE & operator=(int64_t const & other);
	CT_NODE & operator=(double const & other);
	//CT_NODE & operator=(bool const & other);
	//node & operator=(node_data const & other);

	/* cast operator */
	//template<typename T2> operator T2() const;
	operator std::string() const;
	operator double() const;
	operator int64_t() const;
	operator bool() const;


	operator int() const;
	operator unsigned int() const;
	operator uint64_t() const;
	//operator void*() const;
	//operator node_data() const;
#if 0
	bool has(std::string in_str_name, uint64_t in_i_indice = 0);
	bool has(uint64_t in_i_id, uint64_t in_i_indice = 0);
#endif

	CT_GUARD<CT_NODE> get(std::string const & in_str_name, uint64_t const in_i_indice =
			0);
	CT_GUARD<CT_NODE> add(std::string in_str_name);

#if 0
	node::find_result find(std::string in_str_name);
	node::find_result childs(void);
#endif

	CT_NODE::childs_result get_childs(std::string in_str_name);
	CT_NODE::childs_result get_childs(void);
#if 0
	CT_NODE & add_from(std::string in_str_name, CT_NODE & in_c_node);
	CT_NODE & add(std::string in_str_name);
#endif

	int from_xml(std::string const & in_str_xml);
	int from_ezxml_node(ezxml_t in_ps_node, bool in_b_create = true);
	int from_xml_file(std::string const & in_str_file, bool in_b_create = true);

	void to_xml_file(std::string const & in_str_file);
	std::string to_xml(uint in_i_level = 0);

	/* Quick set methos */
	void set_name(std::string in_str_name) {
		_id = in_str_name;
	}

	/* Quick get methods */
	enum ET_NODE_TYPE get_type() {
		return _e_type;
	}

	/* Set type */
	void set_type(enum ET_NODE_TYPE in_e_type);

	/* events */
	virtual CT_GUARD<bml::node<std::string,CT_GUARD> > on_new(void);
	virtual void on_read(void);
	virtual void on_write(void);

	/* Copy methods */
	void copy_link(bml::node<std::string,CT_GUARD> const & other);
	void copy(bml::node<std::string,CT_GUARD> const & other);
	//virtual void events_handler(enum ET_NODE_EVENT_TYPE in_e_event, node * in_pc_node);
};

void f_test_node(void);

#endif /* NODE_HH_ */
