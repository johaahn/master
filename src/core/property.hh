/***********************************************************************
 ** property
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
#ifndef PROPERTY_HH_
#define PROPERTY_HH_

/**
 * @file property.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Jul 9, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <node.hh>
#include <port.hh>
#include <cpp/lock.hh>
/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/
enum {
	E_ID_PROP_NAME,
	E_ID_PROP_ID,
	E_ID_PROP_DESCRIPTION,
	E_ID_PROP_INT_CHILD,
	E_ID_PROP_INT_MIN,
	E_ID_PROP_INT_MAX,
};
#if 0
class CT_HOST_PROPERTY_GROUP {
	CT_HOST_PROPERTY * _pc_parent;
public:
	CT_HOST_PROPERTY_GROUP(CT_HOST_PROPERTY * in_pc_new);
	~CT_HOST_PROPERTY_GROUP();
};
#endif

class CT_HOST_PROPERTY_ELEMENT;

class CT_HOST_PROPERTY {
	//static std::map<std::string, >
protected:
	//CT_PORT_NODE * _pc_dict;
	CT_GUARD<CT_HOST_PROPERTY_ELEMENT> _pc_top;
public:
	CT_HOST_PROPERTY();
	virtual ~CT_HOST_PROPERTY();

	/* Update dictionnary */
	virtual int f_set_dict(CT_NODE & in_c_node);

	/*  set / fil methods */
	virtual int f_set_from(CT_NODE & in_c_node);
	virtual int f_set_from(CT_PORT_NODE & in_c_node);
	int f_fill(CT_NODE & in_c_node);
	int f_fill(CT_PORT_NODE & in_c_node);

	/* quick get top */
	CT_HOST_PROPERTY_ELEMENT * f_get_top(void) { return _pc_top.get(); }

	/* new property allocation */
	virtual CT_GUARD<CT_HOST_PROPERTY_ELEMENT> on_new_element(
			CT_HOST_PROPERTY * in_pc_new, CT_HOST_PROPERTY_ELEMENT * in_pc_parent);
};


class CT_HOST_PROPERTY_ELEMENT {

	typedef std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator m_nodes_iterator_t;
	std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> > _m_nodes;
	std::map<uint32_t, std::string> _m_id;
//	std::multimap<uint32_t, CT_HOST_PROPERTY_ELEMENT*>::iterator m_id_iterator_t;
//	std::multimap<uint32_t, CT_HOST_PROPERTY_ELEMENT*> _m_id;
	std::map<std::string, CT_NODE> _m_dict_name;
	std::map<uint32_t, CT_NODE> _m_dict_id;


	CT_NODE _c_config;
	uint32_t _i_id;

protected:
	CT_NODE _c_value;
	std::string _str_name;
	std::string _str_cfg_name;
	std::string _str_obj_name;

protected:
	CT_HOST_PROPERTY * _pc_property;
	CT_HOST_PROPERTY_ELEMENT * _pc_parent;
	//std::list<CT_HOST_PROPERTY_ELEMENT*> _l_childs;

public:
	static CT_SPINLOCK c_dict_property_lock;
	typedef std::map<std::string, CT_HOST_PROPERTY_ELEMENT *>::iterator m_dict_property_iterator_t;
	static std::map<std::string, CT_HOST_PROPERTY_ELEMENT *> m_dict_property;

private:
	int f_create(CT_GUARD<CT_HOST_PROPERTY_ELEMENT> * out_pc_element,
			CT_NODE & in_c_node, int in_i_count = 0);

#if 0
	int f_find_name(CT_HOST_PROPERTY_ELEMENT ** out_pc_element,
			std::string &in_str_name, int in_i_count);
	int f_find_id(CT_HOST_PROPERTY_ELEMENT ** out_pc_element, uint32_t &in_i_id,
			int in_i_count);
#endif
public:
	CT_HOST_PROPERTY_ELEMENT(CT_HOST_PROPERTY * in_pc_new, CT_HOST_PROPERTY_ELEMENT * in_pc_parent);
	virtual ~CT_HOST_PROPERTY_ELEMENT();

	virtual int f_configure(CT_NODE & in_c_node, int in_i_count);
	virtual int f_data_refresh();
	virtual int f_data_update(CT_PORT_NODE & in_c_node);
	virtual int f_data_update(CT_NODE & in_c_node);
	virtual void f_remove(void);

	int f_set_from(CT_NODE & in_c_node);
	int f_set_from(CT_PORT_NODE & in_c_node);
	int f_fill(CT_NODE & in_c_node);
	int f_fill(CT_PORT_NODE & in_c_node);

	int f_init(CT_NODE & in_c_node, int in_i_count = 0);
	int f_clear();

	std::string & f_get_name();
	std::string & f_get_cfg_name();
	std::string & f_get_obj_name();
	uint32_t  f_get_cfg_id();
};


#endif /* PROPERTY_HH_ */
