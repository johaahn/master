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

/**
 * @file property.cc
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Jul 9, 2013
 *
 * @version 1.0 Original version
 */

#include <property.hh>
#include <cpp/string.hh>
/***********************************************************************
 * Includes
 ***********************************************************************/
#if 0
CT_HOST_PROPERTY_GROUP::CT_HOST_PROPERTY_GROUP(
		CT_HOST_PROPERTY * in_pc_new) {
	_pc_parent = in_pc_new;
}
#endif


CT_SPINLOCK CT_HOST_PROPERTY_ELEMENT::c_dict_property_lock;
std::map<std::string, CT_HOST_PROPERTY_ELEMENT*> CT_HOST_PROPERTY_ELEMENT::m_dict_property;

CT_HOST_PROPERTY_ELEMENT::CT_HOST_PROPERTY_ELEMENT(CT_HOST_PROPERTY * in_pc_new,
		CT_HOST_PROPERTY_ELEMENT * in_pc_parent) {
	_pc_parent = in_pc_parent;
	_pc_property = in_pc_new;

}
CT_HOST_PROPERTY_ELEMENT::~CT_HOST_PROPERTY_ELEMENT() {

}

int CT_HOST_PROPERTY_ELEMENT::f_configure(CT_NODE & in_c_node, int in_i_count) {
	int ec;
	if (_pc_parent || in_c_node.has("name")) {
		if(in_i_count) {
			_str_name = std::string(in_c_node("name"))+f_string_format("-%d",in_i_count);
		} else {
			_str_name = std::string(in_c_node("name"));
		}
		_str_cfg_name = std::string(in_c_node("name"));

	} else {
		_str_name = "top";
		_str_cfg_name = _str_name;
	}

	_str_obj_name = "";
	if (_pc_parent) {
		_str_obj_name += _pc_parent->f_get_obj_name();
		_str_obj_name += "/";
	}
	_str_obj_name += _str_name;

	{
		CT_GUARD_LOCK c_guard_prop(c_dict_property_lock);
		_DBG << _str_obj_name;
		M_ASSERT(m_dict_property.find(_str_obj_name) == m_dict_property.end());
		m_dict_property[_str_obj_name] = this;
	}
	/* Check */
	if (!in_c_node.has("child")) {
		if (!in_c_node.has("id")) {
			CRIT("no id field in node %s", _str_name.c_str());
			ec = EC_FAILURE;
			goto out_err;
		}
		if (!in_c_node.has("type")) {
			CRIT("no type field in node %s", _str_name.c_str());
			ec = EC_FAILURE;
			goto out_err;
		}
	}

	if (!in_c_node.has("child")) {
		std::string str_type = in_c_node("type");
		if (str_type == "int64") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_INT64);
		} else if (str_type == "string") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_CHAR);
		} else if (str_type == "int") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_INT64);
		} else if (str_type == "enum") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_INT64);
		} else if (str_type == "bool") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_INT64);
		} else if (str_type == "file") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_CHAR);
		} else if (str_type == "double") {
			_c_value.set_type(CT_NODE::E_NODE_TYPE_CHAR);
		} else {
			_FATAL << "Unknown type: " << str_type;
		}

		_c_value = (std::string) in_c_node("default");
		_i_id = (uint) in_c_node("id");
	}

	//D("Initialize %s", _str_name.c_str());
	//_c_config = in_c_node;

	ec = EC_SUCCESS;
	out_err: return ec;
}
std::string & CT_HOST_PROPERTY_ELEMENT::f_get_name() {
	return _str_name;
}
std::string & CT_HOST_PROPERTY_ELEMENT::f_get_cfg_name() {
	return _str_cfg_name;
}
std::string & CT_HOST_PROPERTY_ELEMENT::f_get_obj_name() {
	return _str_obj_name;
}
uint32_t CT_HOST_PROPERTY_ELEMENT::f_get_cfg_id() {
	return _i_id;
}
int CT_HOST_PROPERTY_ELEMENT::f_create(
		CT_GUARD<CT_HOST_PROPERTY_ELEMENT> * out_pc_element,
		CT_NODE & in_c_node, int in_i_count) {
	int ec;

	M_ASSERT(in_c_node.has("name"));
	M_ASSERT(in_c_node.has("id"));

	std::string str_name = (std::string) *(in_c_node.get("name"));
	int i_id = (int) (*in_c_node.get("id"));

	CT_GUARD<CT_HOST_PROPERTY_ELEMENT> pc_tmp = _pc_property->on_new_element(
			_pc_property, this);
	M_ASSERT(pc_tmp);

	std::pair<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> > c_pair_name(
			str_name, pc_tmp);
	std::pair<uint32_t, std::string> c_pair_id(i_id, str_name);
	_m_nodes.insert(c_pair_name);
	_m_id.insert(c_pair_id);

	//_l_childs.push_back(pc_tmp);
	ec = pc_tmp->f_init(in_c_node, in_i_count);
	if (ec != EC_SUCCESS) {
		CRIT("unable to initialize element");
		goto out_err;
	}

	if (out_pc_element) {
		*out_pc_element = pc_tmp;
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_HOST_PROPERTY_ELEMENT::f_init(CT_NODE & in_c_node, int in_i_count) {
	int ec;

	ec = f_configure(in_c_node, in_i_count);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to configure Property element");
		goto out_err;
	}

	if (in_c_node.has("child")) {

		CT_NODE::childs_result c_res = in_c_node.get_childs("child");
		CT_NODE::childs_iterator pc_it;
		for (pc_it = c_res.begin(); pc_it != c_res.end(); pc_it++) {
			CT_NODE & c_child = *(*pc_it);
			std::string str_name;
			uint32_t i_id;
			int i_count;

			if (!c_child.has("name")) {
				CRIT("no id field in child");
				ec = EC_FAILURE;
				goto out_err;
			}

			str_name = (std::string) *((*pc_it)->get("name"));

			if (!c_child.has("count")) {
				c_child("count") = "1";
			}

			i_id = (uint32_t) *(*pc_it)->get("id");
			i_count = (int) *(*pc_it)->get("count");

			/* Update creation dictionnary */
			_m_dict_name[str_name].copy(**pc_it);
			_m_dict_id[i_id].copy(**pc_it);
			_m_id[i_id] = str_name;

			//D("New child %s, id:%x, count:%d", str_name.c_str(), i_id, i_count);
			/* Create property element if count  specified */
			for (int i = 0; i < i_count; i++) {

				ec = f_create(NULL, **pc_it, i);
				if (ec != EC_SUCCESS) {
					CRIT("unable to create property element");
					goto out_err;
				}

			}
		}
	}
	ec = EC_SUCCESS;
	out_err: return ec;
}
#if 0
int CT_HOST_PROPERTY_ELEMENT::f_find_name(
		CT_HOST_PROPERTY_ELEMENT ** out_pc_element, std::string &in_str_name,
		int in_i_count) {
	int ec;
	std::pair<std::multimap<std::string, CT_HOST_PROPERTY_ELEMENT*>::iterator,
	std::multimap<std::string, CT_HOST_PROPERTY_ELEMENT*>::iterator> c_res =
	_m_nodes.equal_range(in_str_name);
	std::multimap<std::string, CT_HOST_PROPERTY_ELEMENT*>::iterator pc_it;

	/* Return null by default*/
	*out_pc_element = NULL;

	if (c_res.first == c_res.second) {
		return EC_FAILURE;
	}

	int i_cnt = 0;
	for (pc_it = c_res.first; pc_it != c_res.second; ++pc_it) {
		*out_pc_element = pc_it->second;
		if (i_cnt == in_i_count) {
			return EC_SUCCESS;
		}
		i_cnt++;
	}

	M_ASSERT(i_cnt == in_i_count);

	/* Check name into dictionnary */
	if (_m_dict_name.find(in_str_name) == _m_dict_name.end()) {
		CRIT("No such element name %s in dictionnary", in_str_name.c_str());
		ec = EC_FAILURE;
		goto out_err;
	}

	/* Create element */
	ec = f_create(out_pc_element, _m_dict_name[in_str_name]);
	if (ec != EC_SUCCESS) {
		CRIT("unable to create property element");
		goto out_err;
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_HOST_PROPERTY_ELEMENT::f_find_id(
		CT_HOST_PROPERTY_ELEMENT ** out_pc_element, uint32_t &in_i_id,
		int in_i_count) {
	int ec;
	std::pair<std::multimap<uint32_t, CT_HOST_PROPERTY_ELEMENT*>::iterator,
	std::multimap<uint32_t, CT_HOST_PROPERTY_ELEMENT*>::iterator> c_res =
	_m_id.equal_range(in_i_id);
	std::multimap<uint32_t, CT_HOST_PROPERTY_ELEMENT*>::iterator pc_it;

	/* Return null by default*/
	*out_pc_element = NULL;

	if (c_res.first == c_res.second) {
		return EC_FAILURE;
	}

	int i_cnt = 0;
	for (pc_it = c_res.first; pc_it != c_res.second; ++pc_it) {
		*out_pc_element = pc_it->second;
		if (i_cnt == in_i_count) {
			return EC_SUCCESS;
		}
		i_cnt++;
	}

	M_ASSERT(i_cnt == in_i_count);

	/* Check name into dictionnary */
	if (_m_dict_id.find(in_i_id) == _m_dict_id.end()) {
		CRIT("No such element id %x in dictionnary", in_i_id);
		ec = EC_FAILURE;
		goto out_err;
	}

	/* Create element */
	ec = f_create(out_pc_element, _m_dict_id[in_i_id]);
	if (ec != EC_SUCCESS) {
		CRIT("unable to create property element");
		goto out_err;
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}
#endif

int CT_HOST_PROPERTY_ELEMENT::f_clear(void) {
	std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator pc_it;
	//_l_childs.clear();
	_m_nodes.clear();
	_m_id.clear();

	_m_dict_id.clear();
	_m_dict_name.clear();

	return EC_SUCCESS;
}

int CT_HOST_PROPERTY_ELEMENT::f_data_refresh() {
	return EC_SUCCESS;
}

int CT_HOST_PROPERTY_ELEMENT::f_data_update(CT_NODE & in_c_node) {
	//M_ASSERT(in_c_node.get_id() == _str_name);
	_c_value = (std::string) in_c_node;
	return EC_SUCCESS;
}

void CT_HOST_PROPERTY_ELEMENT::f_remove(void) {
	CT_GUARD_LOCK c_guard_prop(c_dict_property_lock);
	M_ASSERT(m_dict_property.find(_str_obj_name) != m_dict_property.end());
	m_dict_property.erase(_str_obj_name);
}

int CT_HOST_PROPERTY_ELEMENT::f_data_update(CT_PORT_NODE & in_c_node) {
	M_ASSERT(in_c_node.get_id() == _i_id);
	switch (_c_value.get_type()) {
	case CT_NODE::E_NODE_TYPE_CHAR:
		_c_value = in_c_node.get_data<std::string>();
		break;
	case CT_NODE::E_NODE_TYPE_INT64:
		_c_value = in_c_node.get_data<int64_t>();
		break;
	default:
		FATAL("Not supported type");
		break;
	}
	return EC_SUCCESS;
}

int CT_HOST_PROPERTY_ELEMENT::f_set_from(CT_NODE & in_c_node) {
	int ec;
	//D("%s", in_c_node.to_xml().c_str());
	//D("%s %s", in_c_node.get_id().c_str(), _str_name.c_str());
	//M_ASSERT(in_c_node.get_id() == _str_name);

	ec = f_data_update(in_c_node);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to update data in node %s", _str_name.c_str());
		goto out_err;
	}

	/* Update childs */
	{
		CT_NODE::childs_result c_res_in = in_c_node.get_childs();
		std::string str_last_name = "";
		uint i_cnt = 0;

		std::pair<
				std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator,
				std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator> c_res_int;

		bool b_create = true;
		std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator pc_it_int;

		for (CT_NODE::childs_iterator pc_it = c_res_in.begin();
				pc_it != c_res_in.end(); pc_it++) {
			std::string str_new_name = (*pc_it)->get_id();
			CT_GUARD<CT_HOST_PROPERTY_ELEMENT> pc_element;
			//_DBG << str_new_name << " " << str_last_name;
			/* Reset count on new name */
			if (str_new_name != str_last_name) {
				i_cnt = 0;

				if (b_create == false) {
					pc_it_int++;
					/* Delete remaining node */
					while (pc_it_int != c_res_int.second) {
						pc_element = pc_it_int->second;
						pc_it_int = _m_nodes.erase(pc_it_int);
						pc_element->f_remove();
					}
				}

				c_res_int = _m_nodes.equal_range(str_new_name);

				if (c_res_int.first == c_res_int.second) {
				//	_DBG << "No node " << str_new_name << " found";
					b_create = true;
				} else {
					//_DBG << "node " << str_new_name << " found";
					b_create = false;
					pc_it_int = c_res_int.first;
				}
			} else {
				i_cnt++;

				if (b_create == false) {
					pc_it_int++;
					if (pc_it_int == c_res_int.second) {
						b_create = true;
					}
				}
			}

			if (b_create) {
				//_DBG << "Create node " << str_new_name << _V(i_cnt);
				/* Check name into dictionnary */
				if (_m_dict_name.find(str_new_name) == _m_dict_name.end()) {
					CRIT("No such element name %s in dictionnary",
							str_new_name.c_str());
					ec = EC_FAILURE;
					goto out_err;
				}

				/* Create element */
				ec = f_create(&pc_element, _m_dict_name[str_new_name]);
				if (ec != EC_SUCCESS) {
					CRIT("unable to create property element");
					goto out_err;
				}

			} else {
				M_ASSERT(str_new_name == pc_it_int->first);
				//_DBG << "Using current node " << pc_it_int->first << _V(i_cnt);
				pc_element = pc_it_int->second;
			}

			pc_element->f_set_from(**pc_it);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to set property from CT_NODE");
				goto out_err;
			}

			str_last_name = str_new_name;
		}
	}
	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_HOST_PROPERTY_ELEMENT::f_set_from(CT_PORT_NODE & in_c_node) {
	int ec;
	M_ASSERT(in_c_node.get_id() == _i_id);

	ec = f_data_update(in_c_node);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to update data in node %s", _str_name.c_str());
		goto out_err;
	}

	/* Update childs */
	{
		CT_PORT_NODE::childs_result c_res_in = const_cast<CT_PORT_NODE&>(in_c_node).get_childs();
		//uint32_t i_last_id = -1;
		std::string str_last_name = "";
		uint i_cnt = 0;

		std::pair<
				std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator,
				std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator> c_res_int;

		bool b_create = true;
		std::multimap<std::string, CT_GUARD<CT_HOST_PROPERTY_ELEMENT> >::iterator pc_it_int;

		for (CT_PORT_NODE::childs_iterator pc_it = c_res_in.begin();
				pc_it != c_res_in.end(); pc_it++) {
			uint32_t i_new_id = (*pc_it)->get_id();
			std::string str_new_name = _m_id[i_new_id];
			CT_GUARD<CT_HOST_PROPERTY_ELEMENT> pc_element;

			/* Reset count on new name */
			if (str_new_name != str_last_name) {
				i_cnt = 0;

				if (b_create == false) {
					pc_it_int++;
					/* Delete remaining node */
					while (pc_it_int != c_res_int.second) {
						pc_element = pc_it_int->second;
						pc_it_int = _m_nodes.erase(pc_it_int);
						pc_element->f_remove();
					}
				}

				c_res_int = _m_nodes.equal_range(str_new_name);

				if (c_res_int.first == c_res_int.second) {
					b_create = true;
				} else {
					b_create = false;
					pc_it_int = c_res_int.first;
				}
			} else {
				i_cnt++;

				if (b_create == false) {
					pc_it_int++;
					if (pc_it_int == c_res_int.second) {
						b_create = true;
					}
				}
			}

			if (b_create) {
				/* Check name into dictionnary */
				if (_m_dict_name.find(str_new_name) == _m_dict_name.end()) {
					CRIT("No such element name %s in dictionnary",
							str_new_name.c_str());
					ec = EC_FAILURE;
					goto out_err;
				}

				/* Create element */
				ec = f_create(&pc_element, _m_dict_name[str_new_name]);
				if (ec != EC_SUCCESS) {
					CRIT("unable to create property element");
					goto out_err;
				}

			} else {
				pc_element = pc_it_int->second;
			}

			ec = pc_element->f_set_from(**pc_it);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to set property from CT_NODE");
				goto out_err;
			}

			str_last_name = str_new_name;
		}
	}
	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_HOST_PROPERTY_ELEMENT::f_fill(CT_NODE & in_c_node) {

	/* Refresh data */
	f_data_refresh();
	//CT_NODE& c_new = in_c_node.add(_str_name);
	//c_new = (std::string) _c_value;
	in_c_node = (std::string) _c_value;
	//_DBG << (std::string) _c_value;
	m_nodes_iterator_t pc_it;

	for (pc_it = _m_nodes.begin(); pc_it != _m_nodes.end(); pc_it++) {
		//_DBG << pc_it->first;
		CT_GUARD<CT_NODE> c_new = in_c_node.add(
				pc_it->second->f_get_cfg_name());
		pc_it->second->f_fill(*c_new);

	}

	return EC_SUCCESS;
}

int CT_HOST_PROPERTY_ELEMENT::f_fill(CT_PORT_NODE & in_c_node) {
	CT_PORT_NODE c_tmp;

	switch (_c_value.get_type()) {
	case CT_NODE::E_NODE_TYPE_CHAR:
		in_c_node.set_data<std::string>(std::string(_c_value));
		break;
	case CT_NODE::E_NODE_TYPE_INT64:
		in_c_node.set_data<int64_t>(int64_t(_c_value));
		break;
	default:
		FATAL("Not supported type");
		break;
	}

	m_nodes_iterator_t pc_it;
	for (pc_it = _m_nodes.begin(); pc_it != _m_nodes.end(); pc_it++) {
		CT_GUARD<CT_PORT_NODE> c_new = in_c_node.add(
				pc_it->second->f_get_cfg_id());
		pc_it->second->f_fill(*c_new);
	}

	return EC_SUCCESS;
}

CT_HOST_PROPERTY::CT_HOST_PROPERTY() {
	_pc_top = NULL;
}

CT_HOST_PROPERTY::~CT_HOST_PROPERTY() {
}

int CT_HOST_PROPERTY::f_set_dict(CT_NODE & in_c_node) {
	int ec;

	if (!_pc_top) {
		_pc_top = on_new_element(this, NULL);
		M_ASSERT(_pc_top);
	} else {
		ec = _pc_top->f_clear();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to clear property");
			goto out_err;
		}
	}

	ec = _pc_top->f_init(in_c_node);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to clear property");
		goto out_err;
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}
#if 0
int CT_HOST_PROPERTY::f_create(CT_NODE &in_c_node) {
	std::string str_data = (std::string) pc_it->get("name");
	int i_id = (int) pc_it->get("id");
}
#endif

int CT_HOST_PROPERTY::f_set_from(CT_NODE & in_c_node) {
	return _pc_top->f_set_from(in_c_node);
}

int CT_HOST_PROPERTY::f_set_from(CT_PORT_NODE & in_c_node) {
	return _pc_top->f_set_from(in_c_node);
}


int CT_HOST_PROPERTY::f_fill(CT_NODE & in_c_node) {
	//in_c_node.set_id(_pc_top->f)
	in_c_node.clear();
	return _pc_top->f_fill(in_c_node);
}

int CT_HOST_PROPERTY::f_fill(CT_PORT_NODE & in_c_node) {
	in_c_node.clear();
	return _pc_top->f_fill(in_c_node);
}

CT_GUARD<CT_HOST_PROPERTY_ELEMENT> CT_HOST_PROPERTY::on_new_element(
		CT_HOST_PROPERTY * in_pc_new, CT_HOST_PROPERTY_ELEMENT * in_pc_parent) {
	return CT_GUARD<CT_HOST_PROPERTY_ELEMENT>(new CT_HOST_PROPERTY_ELEMENT(in_pc_new, in_pc_parent));
}
