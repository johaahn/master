/***********************************************************************
 ** core
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
 * @file core.cc
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Jan 1, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <node.hh>
#include <core.hh>
#include <host.hh>
#include <task.hh>
#include <iostream>
#include <cpp/string.hh>

using namespace std;
using namespace master::core;

CT_CORE::CT_CORE() {
	_i_port_update = 0;
	_i_sub_core_update = 0;
	_pc_parent = NULL;
	_str_full_name = "";
	_str_name = "";

	;
	f_port_cfg_create("__parent__", M_PORT_BIND(CT_CORE, f_cfg_event, this));

	//TODO remove
	//f_port_create(E_CORE_PORT_CFG, "__parent__", f_register_cfg_cb);
}

CT_CORE::~CT_CORE() {
	f_port_clear();
	f_port_cfg_clear();
	f_child_clear();
}

void CT_CORE::f_set_parent(CT_CORE * in_pc_parent) {
	_pc_parent = in_pc_parent;

	{
		CT_NODE c_node;
		c_node("url")("tx") = "func://__" + f_get_full_name() + "_parent";
		c_node("url")("rx") = "func://__" + f_get_full_name() + "_child";

		f_port_cfg_get("__parent__").f_set_url(c_node);
	}

}

int CT_CORE::f_child_clear(void) {
	int ec;
#if 0
	/* Removing childs */
	{
		while (_m_sub_cores.size()) {
			std::map<std::string, CT_CORE *>::iterator pc_it =
			_m_sub_cores.begin();
			//D("Clearing subnode ... %s", pc_it->first.c_str());
			/* Removing unused child */
			ec = f_child_remove(pc_it->first);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to remove old child");
				goto out_err;
			}
		}
	}
#endif

	//D("Clearing childs");
	_m_sub_cores.clear();

	ec = EC_SUCCESS;
	//out_err:
	return ec;

}

int CT_CORE::f_childs_get_OBSOLETE(std::vector<CT_CORE *> & out_v_list) {
	int ec;
	out_v_list.clear();

	std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
	for (pc_it = _m_sub_cores.begin(); pc_it != _m_sub_cores.end(); pc_it++) {
		out_v_list.push_back(pc_it->second.get());
	}

	ec = EC_SUCCESS;
	//out_err:
	return ec;
}

int CT_CORE::f_child_has(std::string in_str_name) {
	int ec;
	if (_m_sub_cores.find(in_str_name) == _m_sub_cores.end()) {
		ec = EC_FAILURE;
	} else {
		ec = EC_SUCCESS;
	}
	return ec;
}

int CT_CORE::f_port_has(std::string in_str_name) {
	int ec;
	if (_m_ports_index.find(in_str_name) == _m_ports_index.end()) {
		ec = EC_FAILURE;
	} else {
		ec = EC_SUCCESS;
	}
	return ec;
}

int CT_CORE::f_port_cfg_has(std::string in_str_name) {
	int ec;
	if (_m_ports_cfg.find(in_str_name) == _m_ports_cfg.end()) {
		ec = EC_FAILURE;
	} else {
		ec = EC_SUCCESS;
	}
	return ec;
}

int CT_CORE::f_child_tryget(CT_CORE*& out_pc_child, std::string in_str_name) {
	std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
	int ec;

	pc_it = _m_sub_cores.find(in_str_name);
	if (pc_it == _m_sub_cores.end()) {
		ec = EC_FAILURE;
	} else {

		out_pc_child = pc_it->second.get();
		ec = EC_SUCCESS;
	}
	return ec;
}

int CT_CORE::f_port_tryget(CT_PORT*& out_pc_child, std::string in_str_name) {
	std::map<std::string, uint>::iterator pc_it;
	int ec;

	pc_it = _m_ports_index.find(in_str_name);
	if (pc_it == _m_ports_index.end()) {
		ec = EC_FAILURE;
	} else {

		out_pc_child = &(*_v_ports[pc_it->second]);
		ec = EC_SUCCESS;
	}
	return ec;
}

int CT_CORE::f_child_create(std::string in_str_name, std::string in_str_type) {
	CT_CORE * pc_new;
	int ec;

	/* Allocation of new core with host library */
	ec = CT_HOST::host->f_core_new(pc_new, in_str_type);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to allocate new core");
		goto out_err;
	}

	/* Adding node to sub core */
	_m_sub_cores[in_str_name] = CT_GUARD<CT_CORE>(pc_new);

	/* Store name in subnode : debug purpose */
	pc_new->f_set_name(in_str_name);

	/* Store full name */
	pc_new->f_set_full_name(_str_full_name + "/" + in_str_name);

	/* Store parent pointer and configure transport */
	pc_new->f_set_parent(this);

	/* Add config port */
	f_port_cfg_create(in_str_name, M_PORT_BIND(CT_CORE, f_cfg_event, this));

	/* Configure dedicated port in parent */
	{
		CT_NODE c_node;
		c_node("url")("rx") = "func://__" + pc_new->f_get_full_name()
				+ "_parent";
		c_node("url")("tx") = "func://__" + pc_new->f_get_full_name()
				+ "_child";

		f_port_cfg_get(in_str_name).f_set_url(c_node);
	}

	//CRIT("allocating child %s %p", in_str_name.c_str(), pc_new);

	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_port_clear(void) {
	int ec;
	/* Removing ports */
	{
		std::map<uint, CT_PORT*>::iterator pc_it;
		for (pc_it = _v_ports.begin(); pc_it != _v_ports.end(); pc_it++) {
			D("Clearing port ...");
			/* Clear unused ports */
			ec = pc_it->second->f_clear();
			if (ec != EC_SUCCESS) {
				CRIT("Unable to clear port");
				goto out_err;
			}
			delete pc_it->second;
		}
		_v_ports.clear();

	}

	ec = EC_SUCCESS;
	out_err: return ec;

}

int CT_CORE::f_port_cfg_clear(void) {
	int ec;

	/* Removing config ports */
	{
		std::map<std::string, CT_PORT*>::iterator pc_it;
		for (pc_it = _m_ports_cfg.begin(); pc_it != _m_ports_cfg.end();
				pc_it++) {
			D("Clearing config port ...");
			/* Clear unused ports */
			ec = pc_it->second->f_clear();
			if (ec != EC_SUCCESS) {
				CRIT("Unable to clear port");
				goto out_err;
			}
			delete pc_it->second;
		}
		_m_ports_cfg.clear();

	}

	ec = EC_SUCCESS;
	out_err: return ec;

}


int CT_CORE::f_port_create(uint in_i_id, std::string in_str_name,
		CT_PORT_BIND_CORE const & in_c_cb) {
	int ec;
	/* REsize vector if needed */
	if (_v_ports.find(in_i_id) != _v_ports.end()) {
		throw std::runtime_error("Port ID already used");
	}
	if (_m_ports_index.find(in_str_name) != _m_ports_index.end()) {
		throw std::runtime_error("Port Name already used");
	}

	/* Create port if needed */
	_v_ports[in_i_id] = new CT_PORT(this);

	/* Update name to id map */
	_m_ports_index[in_str_name] = in_i_id;
	_v_ports[in_i_id]->f_register_cb_rx(in_c_cb);
	_v_ports[in_i_id]->str_name = in_str_name;
	_v_ports[in_i_id]->i_id = in_i_id;
	//D("%p",&(_v_ports.at(0)));
	//_v_ports[in_i_id].i_id = in_i_id;
	ec = EC_SUCCESS;
	///out_err:
	return ec;
}

int CT_CORE::f_port_cfg_create(std::string in_str_name, CT_PORT_BIND_CORE const & in_c_cb) {
	int ec;

	//D("Creating CFG port %s for %s", in_str_name.c_str(), f_get_name().c_str());

	/* Create port if needed */
	if (_m_ports_cfg.find(in_str_name) == _m_ports_cfg.end()) {
		_m_ports_cfg[in_str_name] = new CT_PORT(this);
	} else {
		CRIT("Port %s (%s) already exists !", in_str_name.c_str(),
				_m_ports_cfg[in_str_name]->str_name.c_str());
		ec = EC_FAILURE;
		goto out_err;
	}

	/* Update name to id map */
	_m_ports_cfg[in_str_name]->f_register_cb_rx(in_c_cb);
	_m_ports_cfg[in_str_name]->str_name = in_str_name;
	_m_ports_cfg[in_str_name]->i_id = -1;
	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_child_remove(std::string in_str_name) {

	std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
	pc_it = _m_sub_cores.find(in_str_name);
	M_ASSERT(pc_it != _m_sub_cores.end());

	D("removing child %s %p", in_str_name.c_str(), pc_it->second.get());
	/* Delete child form list */
	_m_sub_cores.erase(pc_it);

	return EC_SUCCESS;
}

int CT_CORE::f_port_cfg_remove(std::string in_str_name) {

	std::map<std::string, CT_PORT *>::iterator pc_it;
	pc_it = _m_ports_cfg.find(in_str_name);
	M_ASSERT(pc_it != _m_ports_cfg.end());
	CT_PORT * pc_tmp = pc_it->second;
	D("removing port %s %p", in_str_name.c_str(), pc_tmp);
	/* Delete child form list */
	_m_ports_cfg.erase(pc_it);

	/* Unuse child */
	delete pc_tmp;

	return EC_SUCCESS;
}

int CT_CORE::f_child_remove(CT_CORE * in_pc_node) {
	std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
	for (pc_it = _m_sub_cores.begin(); pc_it != _m_sub_cores.end(); pc_it++) {
		//D("%p %p", pc_it->second.get(), in_pc_node);
		if (pc_it->second.get() == in_pc_node) {

			/* Remove corresponding port */
			f_port_cfg_remove(pc_it->first);

			/* Erase child */
			_m_sub_cores.erase(pc_it);

			return EC_SUCCESS;
		}
	}
	return EC_FAILURE;
}

int CT_CORE::f_child_remove(ST_PLUGIN_DESCRIPTION * in_ps_desc) {
	int ec;
	std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
	for (pc_it = _m_sub_cores.begin(); pc_it != _m_sub_cores.end();) {
		//D("%p %p", pc_it->second->f_get_desc(), in_ps_desc);

		if (pc_it->second->f_get_desc() == in_ps_desc) {
			D("Removing instance %p of plugin %s", pc_it->second.get(),
					in_ps_desc->str_name.c_str());

			_m_sub_cores.erase(pc_it++);

		} else {
			ec = pc_it->second->f_child_remove(in_ps_desc);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to remove child from plugin desc");
				goto out_err;
			}
			pc_it++;
		}

	}
	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_port_clear(std::string in_str_name) {
	std::map<std::string, uint>::iterator pc_it;
	pc_it = _m_ports_index.find(in_str_name);
	M_ASSERT(pc_it != _m_ports_index.end());

	_v_ports.at(_m_ports_index.at(in_str_name))->f_clear();
	return EC_SUCCESS;
}

int CT_CORE::f_port_clear(uint in_i_id) {
	_v_ports.at(in_i_id)->f_clear();
	return EC_SUCCESS;
}

int CT_CORE::f_child_configure(CT_NODE & in_c_config, bool in_b_dynamic) {
	int ec;
	std::string str_name, str_type;
	CT_CORE * pc_child = NULL;
	//_DBG << in_c_config.to_xml();
	if (!in_c_config.has("type")) {
		CRIT("No type on node");
		ec = EC_FAILURE;
		goto out_err;
	}

	/* If no name set name like type */
	if (!in_c_config.has("name")) {
		in_c_config("name") = (std::string) in_c_config("type");
	}

	str_name = (std::string) in_c_config("name");
	str_type = (std::string) in_c_config("type");

	//D("Initializing %s,%s", str_name.c_str(), str_type.c_str());

	/* Create node */
	if (f_child_tryget(pc_child, str_name) == EC_FAILURE) {
		/* Create child */
		ec = f_child_create(str_name, str_type);
		if (ec != EC_SUCCESS) {
			CRIT("Unable to create new child");
			ec = EC_FAILURE;
			goto out_err;
		}

		/* Get child */
		ec = f_child_tryget(pc_child, str_name);
		M_ASSERT(ec == EC_SUCCESS);
	} else {
		if (pc_child->f_get_desc()->str_name != str_type) {
#if 0
			D(
					"child %s type differ from configuration (%s, %s), replacing child ...",
					str_name.c_str(), pc_child->f_get_desc()->str_name.c_str(),
					str_type.c_str());
#endif
			/* Removing child */
			ec = f_child_remove(str_name);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to remove old child");
				ec = EC_FAILURE;
				goto out_err;
			}

			/* Create child */
			ec = f_child_create(str_name, str_type);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to remove old child");
				ec = EC_FAILURE;
				goto out_err;
			}
		}
	}
	pc_child->_i_update = _i_sub_core_update;
	pc_child->_b_dynamic = in_b_dynamic;

#if 0
	if (!pv_it->has("use")) {
		pc_core->i_use += (int)pv_it->get("use");
	} else {
		pc_core->i_use += 1;
	}
#endif
	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_child_settings(CT_NODE & in_c_config) {
	int ec;
	std::string str_name, str_type;
	CT_CORE * pc_child = NULL;

	//_WARN << "Dumping configuration node:" << in_c_config.to_xml();
	//fflush(0);
	//fflush(stderr);
	//std::cout << in_c_config.to_xml().c_str() << std::endl;
	//fflush(0);
	//fflush(stderr);

	if (!in_c_config.has("name")) {
		CRIT("No name on node");
		ec = EC_FAILURE;
		goto out_err;
	}

	if (!in_c_config.has("type")) {
		_CRIT << "No type on node: " << _V( (std::string) in_c_config("name"));
		ec = EC_FAILURE;
		goto out_err;
	}

	str_name = (std::string) in_c_config("name");
	str_type = (std::string) in_c_config("type");

	//D("Configuring %s,%s", str_name.c_str(), str_type.c_str());

	/* Check node existence */
	if (f_child_tryget(pc_child, str_name) == EC_FAILURE) {
		CRIT("Child %s not found", str_name.c_str());
		ec = EC_FAILURE;
		goto out_err;
	} else {
		if (pc_child->f_get_desc()->str_name != str_type) {
			CRIT("Child %s type %s!=%s does not match", str_name.c_str(),
					pc_child->f_get_desc()->str_name.c_str(), str_type.c_str());
			ec = EC_FAILURE;
			goto out_err;
		}
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_settings(CT_NODE & in_c_config) {
	//D("Settings ports of node %s ...", in_c_config.get_id().c_str());
	//WARN("Node %s does not handle settings", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_configure(CT_NODE & in_c_config) {
	int ec;

	/* Configure ports if needed */
	if (in_c_config.has("ports")) {
		ec = f_ports_configure(in_c_config("ports"));
		if (ec != EC_SUCCESS) {
			CRIT("Unable to configure ports");
			ec = EC_FAILURE;
			goto out_err;
		}
	}

	/* Configure nodes if needed */
	ec = f_childs_configure(in_c_config);
	if (ec != EC_SUCCESS) {
		CRIT("Unable to configure childs");
		ec = EC_FAILURE;
		goto out_err;
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_childs_configure(CT_NODE & in_c_config) {
	int ec;
	CT_NODE::childs_result v_tmp = in_c_config.get_childs("node");

	/* Parse sub nodes */
	_i_sub_core_update++;
	for (CT_NODE::childs_iterator pv_it = v_tmp.begin(); pv_it != v_tmp.end();
			pv_it++) {
		CT_NODE & c_node = **pv_it;
		ec = f_child_configure(c_node, false);
		if (ec != EC_SUCCESS) {
			CRIT("Unable to create child");
			ec = EC_FAILURE;
			goto out_err;
		}
	}

	/* Removing unused childs, that are not dynamics (not created with profile ) */
	{
		std::map<std::string, CT_GUARD<CT_CORE>>::iterator pc_it;
		for (pc_it = _m_sub_cores.begin(); pc_it != _m_sub_cores.end();) {
			if ((pc_it->second->_b_dynamic == false)
					&& (pc_it->second->_i_update != _i_sub_core_update)) {
				std::string str_name = pc_it->first;
				pc_it++;
				/* Removing unused child */
				ec = f_child_remove(str_name);
				if (ec != EC_SUCCESS) {
					CRIT("Unable to remove old child");
					ec = EC_FAILURE;
					goto out_err;
				}
			} else {
				pc_it++;
			}
		}
	}

	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_ports_configure(CT_NODE & in_c_config) {
	int ec;
	CT_NODE::childs_result v_tmp = in_c_config.get_childs("port");
	//D("Configuring %d ports ...", v_tmp.size());

	/* Increment update ID */
	_i_port_update++;

	/* Parse all port node */
	for (CT_NODE::childs_iterator pv_it = v_tmp.begin(); pv_it != v_tmp.end();
			pv_it++) {
		CT_NODE & c_tmp = **pv_it;
		if (!c_tmp.has("name")) {
			CRIT("No name on port");
			ec = EC_FAILURE;
			goto out_err;
		}

		if (!c_tmp.has("url")) {
			CRIT("No URL");
			ec = EC_FAILURE;
			goto out_err;
		}

		string str_name = *c_tmp.get("name");
		CT_PORT * pc_port;

		/* Create node */
		if (f_port_tryget(pc_port, str_name) == EC_FAILURE) {
			CRIT("Unable to find port %s on core %s", str_name.c_str(),
					_str_name.c_str());
			ec = EC_FAILURE;
			goto out_err;
		} else {
			//D("Configuring port: %s", str_name.c_str());
			ec = pc_port->f_set_url(c_tmp);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to set URL");
				ec = EC_FAILURE;
				goto out_err;
			}
			pc_port->i_update = _i_port_update;
		}
	}

	/* Removing unused ports */
	{
		std::map<uint, CT_PORT*>::iterator pc_it;
		for (pc_it = _v_ports.begin(); pc_it != _v_ports.end(); pc_it++) {

			if (pc_it->second->i_update != _i_port_update) {
				//D("Clearing port ...");
				/* Clear unused ports */
				ec = pc_it->second->f_clear();
				if (ec != EC_SUCCESS) {
					CRIT("Unable to remove old child");
					ec = EC_FAILURE;
					goto out_err;
				}
			}
		}
	}
	ec = EC_SUCCESS;
	out_err: return ec;
}

int CT_CORE::f_cfg_event(CT_GUARD<CT_PORT_NODE> const & in_c_node) {
	int ec;
#if 0
	_DBG << "[" << _str_name.c_str() << "] Handling configure event " <<
			in_c_node->get_id();
#endif
	switch (in_c_node->get_id()) {
	case E_ID_CORE_PRE_DESTROY: {
		ec = f_pre_destroy();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to execute pre destroy");
			goto out_err;
		}
		break;
	}

	case E_ID_CORE_POST_INIT: {
		ec = f_post_init();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to execute pre init");
			goto out_err;
		}
		break;
	}
	case E_ID_CORE_PRE_PROFILE: {
		ec = f_pre_profile();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to execute pre profile");
			goto out_err;
		}
		break;
	}
	case E_ID_CORE_POST_PROFILE: {
		ec = f_post_profile();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to execute post profile");
			goto out_err;
		}
		break;
	}
	case E_ID_CORE_POST_SETTINGS: {
		ec = f_post_settings();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to execute post profile");
			goto out_err;
		}
		break;
	}
#if 0
		case E_ID_CORE_CONFIGURE: {
			CT_NODE c_node;
			c_node.from_xml(in_c_node.get_data<std::string>());
			ec = f_configure(c_node, false);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to execute configure");
				goto out_err;
			}
			break;
		}

		case E_ID_CORE_SETTINGS: {
			CT_NODE c_node;
			c_node.from_xml(in_c_node.get_data<std::string>());
			ec = f_configure(c_node, true);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to execute configure settings");
				goto out_err;
			}
			break;
		}
#endif
	default:
		break;
	}

	/* Forward node to childs */

	switch (in_c_node->get_id()) {
	case E_ID_CORE_PRE_DESTROY:
	case E_ID_CORE_POST_INIT:
	case E_ID_CORE_POST_SETTINGS:
	case E_ID_CORE_PRE_PROFILE:
	case E_ID_CORE_POST_PROFILE: {
		uint i_part = 0;
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_new<uint32_t>(
				f_string_format("CORE_FORWARD_CHILDS_%d_%s",
						in_c_node->get_id(), f_get_name().c_str()),
				E_ID_CORE_ACK_STATUS, f_port_cfg_get("__parent__"),
				_m_sub_cores.size());

		/* Fill reply node with task status if present */
		c_task->f_reply_set_parent_task(*in_c_node);
		std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
		c_task->f_enable();

		c_task->f_get_data<uint32_t>() = in_c_node->get_id();

		for (pc_it = _m_sub_cores.begin(); pc_it != _m_sub_cores.end();
				pc_it++) {
#if 0
			D("[%s] Handling configure event forward %d for node %s",
					_str_name.c_str(), in_c_node.get_id(), pc_it->second->f_get_name().c_str());
#endif
			CT_PORT_NODE_GUARD c_node(in_c_node->get_id());
			c_node->copy(*in_c_node);

			c_task->f_status_init(*c_node, i_part);

			//D("Child name %s", pc_it->second->f_get_name().c_str());
			ec = f_port_cfg_get(pc_it->second->f_get_name()).f_send(c_node);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to execute cfg event (child:%s, current:%s)",
						pc_it->first.c_str(), _str_name.c_str());
				goto out_err;
			}

			i_part++;
		}

		break;
	}
	case E_ID_CORE_ACK_STATUS: {
		//D("E_ID_CORE_ACK_STATUS for node %s", f_get_name().c_str());
		ST_TASK_NOTIFICATION & c_notif = in_c_node->map<ST_TASK_NOTIFICATION>();
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(c_notif.s_status);
		//D("Received task status : %d", c_notif.e_status);

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TASK_COMPLETE: {
			//
			//D("Task complete:%s", c_task->f_get_name().c_str());
			if (c_task) {
				uint32_t & i_id = c_task->f_get_data<uint32_t>();

				switch (i_id) {
				case E_ID_CORE_PRE_DESTROY:
					//D("E_ID_CORE_PRE_DESTROY END");
					break;
				case E_ID_CORE_POST_INIT:
					//D("E_ID_CORE_POST_INIT END");
					break;
				case E_ID_CORE_PRE_PROFILE:
					//D("E_ID_CORE_PRE_PROFILE END");
					break;
				case E_ID_CORE_POST_PROFILE:
					//D("E_ID_CORE_POST_PROFILE END");
					break;
				}

				/* Send state to parent node */
				{
					CT_PORT_NODE_GUARD & c_port_node =
							c_task->f_reply_get_node();
					c_port_node->set_id(E_ID_CORE_ACK);
					c_port_node->set_data(ST_CORE_ACK(i_id, E_ACK_SUCCESS));
#if 0
					f_guard_dbg_push(c_port_node->get_segment().resource());
					f_guard_dbg_push(c_port_node.get());
					f_guard_dbg_push(c_task.get());
#endif
					//D("Previous Task %d",c_port_node(E_ID_TASK_STATUS).map<ST_TASK_STATUS>().i_uuid);
					ec = f_port_cfg_get("__parent__").f_send(c_port_node);
					if (ec != EC_SUCCESS) {
						CRIT("Unable to execute cfg event (%s)",
								_str_name.c_str());
						goto out_err;
					}
				}

			}
			break;
		}
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_ABORTED:
		case E_TASK_STATUS_ERROR: {
			CRIT("Task Error");

			/* Send state to parent node */
			{
				CT_PORT_NODE_GUARD & c_port_node = c_task->f_reply_get_node();
				uint32_t & i_id = c_task->f_get_data<uint32_t>();
				c_port_node->set_data<ST_CORE_ACK>(
						ST_CORE_ACK(i_id, E_ACK_FAILURE));
				c_port_node->set_id(E_ID_CORE_ACK);

				ec = f_port_cfg_get("__parent__").f_send(c_port_node);
				if (ec != EC_SUCCESS) {
					CRIT("Unable to execute cfg event (%s)", _str_name.c_str());
					goto out_err;
				}
			}
		}

		default:
			break;
		}

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_TASK_COMPLETE:
		case E_TASK_STATUS_KILLED:
		case E_TASK_STATUS_ABORTED:
			//case E_TASK_STATUS_ERROR:
		{
			CT_HOST::tasks->f_release(c_task);
			break;
		}
		default:
			break;
		}

		break;
	}
	case E_ID_CORE_ACK: {
		//D("E_ID_CORE_ACK for node %s", f_get_name().c_str());
		ST_TASK_STATUS & s_status = CT_TASK::f_reply_get_status(*in_c_node);
		ec = in_c_node->map<ST_CORE_ACK>().i_ack;
		//D("Task %d", s_status.i_uuid);
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(s_status);

		if (!c_task) {
			WARN("Task seems to be closed");
		} else {
			if (ec == E_ACK_SUCCESS) {
				c_task->f_complete(s_status);
			} else {
				c_task->f_error();
			}
		}

		break;
	}

	case E_ID_CORE_SAVE_STATE: {
		//D("E_ID_CORE_SAVE_STATE for node %s", f_get_name().c_str());

		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_new<CT_NODE>(
				f_string_format("CORE_SAVE_STATE_%s", f_get_name().c_str()));

		//D("New Task %d", c_task->f_get_uuid());

		c_task->f_notification_set_id(E_ID_CORE_SAVE_STATE_STATUS);
		c_task->f_notification_set_internal_port(f_port_cfg_get("__parent__"));

		/* Describe task */
		c_task->f_step_set_nb(_m_sub_cores.size());

		/* Fill reply node with task status if present */
		c_task->f_reply_set_parent_task(*in_c_node);

		uint32_t i_part = 0;
		CT_NODE & c_node = c_task->f_get_data<CT_NODE>();
		c_node.set_id("node");
		c_node("name") = f_get_name();
		c_node("type") = f_get_desc()->str_name;

		std::map<std::string, CT_GUARD<CT_CORE> >::iterator pc_it;
		for (pc_it = _m_sub_cores.begin(); pc_it != _m_sub_cores.end();
				pc_it++) {
			CT_PORT_NODE_GUARD c_tmp(E_ID_CORE_SAVE_STATE);

			//D("[%s] %s",
			//		_str_name.c_str(), pc_it->second->f_get_name().c_str());

			/* Add new node */
			c_node.add("node");

			/* Init task subnode */
			c_task->f_status_init(*c_tmp, i_part);

			ec = f_port_cfg_get(pc_it->second->f_get_name()).f_send(c_tmp);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to execute cfg event (child:%s, current:%s)",
						pc_it->first.c_str(), _str_name.c_str());
				c_task->f_error();
				goto out_err;
			}

			i_part++;
		}
		M_ASSERT(i_part == c_task->f_step_get_nb());

		/* Enable task, complete is triggered here if no step/subpart */
		c_task->f_enable();

		break;
	}

	case E_ID_CORE_SAVE_STATE_STATUS: {
		//D("E_ID_CORE_SAVE_STATE_STATUS for node %s", f_get_name().c_str());
		ST_TASK_NOTIFICATION & c_notif = in_c_node->map<ST_TASK_NOTIFICATION>();
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(c_notif.s_status);
		//D("Received task status : %d", c_notif.e_status);

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TASK_COMPLETE: {
			//D("Task complete");

			if (c_task) {
				CT_NODE & c_node = c_task->f_get_data<CT_NODE>();

				/* State save */
				ec = f_state_save(c_node("settings"));
				if (ec != EC_SUCCESS) {
					c_task->f_error("Unable to execute state save");
					goto out_err;
				}

				/* Send state to parent node */
				{
					CT_PORT_NODE_GUARD & c_port_node =
							c_task->f_reply_get_node();
					c_port_node->set_data<std::string>(c_node.to_xml());
					c_port_node->set_id(E_ID_CORE_SAVE_STATE_REPLY);

					//D("Previous Task %d", c_port_node(E_ID_TASK_STATUS).map<ST_TASK_STATUS>().i_uuid);
					ec = f_port_cfg_get("__parent__").f_send(c_port_node);
					if (ec != EC_SUCCESS) {
						CRIT("Unable to execute cfg event (%s)",
								_str_name.c_str());
						goto out_err;
					}
				}
			}
			break;
		}
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_ABORTED:
		case E_TASK_STATUS_ERROR: {
			CRIT("Task Error");

			/* Send state to parent node */
			{
				CT_PORT_NODE_GUARD & c_port_node = c_task->f_reply_get_node();
				c_port_node->set_id(E_ID_CORE_SAVE_STATE_ERROR);

				ec = f_port_cfg_get("__parent__").f_send(c_port_node);
				if (ec != EC_SUCCESS) {
					CRIT("Unable to execute cfg event (%s)", _str_name.c_str());
					goto out_err;
				}
			}
		}

		default:
			break;
		}

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_TASK_COMPLETE:
		case E_TASK_STATUS_KILLED:
		case E_TASK_STATUS_ABORTED:
			//case E_TASK_STATUS_ERROR:
		{
			CT_HOST::tasks->f_release(c_task);
			break;
		}
		default:
			break;
		}

		break;
	}
	case E_ID_CORE_SAVE_STATE_ERROR: {
		//D("E_ID_CORE_SAVE_STATE_ERROR for node %s", f_get_name().c_str());
			ST_TASK_STATUS & s_status = CT_TASK::f_reply_get_status(*in_c_node);
			//D("Task %d", s_status.i_uuid);
			CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(s_status);

			if (!c_task) {
				WARN("Task seems to be closed");
			} else {
				/* inform task that this part of step is completed */
				c_task->f_error();
			}
		break;
	}
	case E_ID_CORE_SAVE_STATE_REPLY: {
		//D("E_ID_CORE_SAVE_STATE_REPLY for node %s", f_get_name().c_str());
		ST_TASK_STATUS & s_status = CT_TASK::f_reply_get_status(*in_c_node);
		//D("Task %d", s_status.i_uuid);
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(s_status);

		if (!c_task) {
			WARN("Task seems to be closed");
		} else {
			/* update task node */
			CT_NODE & c_node = c_task->f_get_data<CT_NODE>();

			/* Fill node */
			M_ASSERT(c_node.has("node", s_status.i_step));
			c_node("node", s_status.i_step).from_xml(
					in_c_node->get_data<std::string>());

			//D("%s", c_node.to_xml().c_str());

			/* inform task that this part of step is completed */
			c_task->f_complete(s_status);

		}

	}
		break;
	case E_ID_CORE_SETTINGS: {
		//D("E_ID_CORE_SETTINGS for node %s", f_get_name().c_str());

		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_new<CT_NODE>(
				f_string_format("CORE_SETTINGS_%s", f_get_name().c_str()));

		//D("New Task %d", c_task->f_get_uuid());

		c_task->f_notification_set_id(E_ID_CORE_SETTINGS_STATUS);
		c_task->f_notification_set_internal_port(f_port_cfg_get("__parent__"));

		/* Fill reply node with task status if present */
		c_task->f_reply_set_parent_task(*in_c_node);

		/* Copy node into task */
		CT_NODE & c_node = c_task->f_get_data<CT_NODE>();
		//std::cout << in_c_node->get_data<std::string>().c_str() << std::endl;
		c_node.from_xml(in_c_node->get_data<std::string>());
		CT_NODE::childs_result v_tmp = c_node.get_childs("node");

		/* Describe task */
		c_task->f_step_set_nb(v_tmp.size());

		uint32_t i_part = 0;

		/* Removing unused childs, that are not dynamics (not created with profile ) */

		for (CT_NODE::childs_iterator pc_it = v_tmp.begin();
				pc_it != v_tmp.end(); pc_it++) {

			std::string str_name = *((*pc_it)->get("name"));

			/* Check settings, parent perspective */
			ec = f_child_settings(**pc_it);
			if (ec != EC_SUCCESS) {
				CRIT("Unable to check settings of child %s", str_name.c_str());
				c_task->f_error();
				goto out_err;
			}

			{
				CT_PORT_NODE_GUARD c_tmp(E_ID_CORE_SETTINGS,
						(*pc_it)->to_xml());

				/* Init task subnode */
				c_task->f_status_init(*c_tmp, i_part);

				M_ASSERT(f_port_cfg_has(str_name) == EC_SUCCESS);
				/* Send msg */
				ec = f_port_cfg_get(str_name).f_send(c_tmp);
				if (ec != EC_SUCCESS) {
					CRIT("Unable to send settings event");
					c_task->f_error();
					goto out_err;
				}

			}

			i_part++;
		}
		M_ASSERT(i_part == c_task->f_step_get_nb());

		/* Enable task, complete is triggered here if no step/subpart */
		c_task->f_enable();

		break;
	}
	case E_ID_CORE_SETTINGS_STATUS: {
		//D("E_ID_CORE_SETTINGS_STATUS for node %s", f_get_name().c_str());
		ST_TASK_NOTIFICATION & c_notif = in_c_node->map<ST_TASK_NOTIFICATION>();
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(c_notif.s_status);
		//D("Received task status : %d", c_notif.e_status);

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TASK_COMPLETE: {
			//D("Task complete");

			if (c_task) {
				CT_NODE & c_node = c_task->f_get_data<CT_NODE>();

				/* Configure current node after child */
				if (c_node.has("settings")) {
					ec = f_settings(*c_node.get("settings"));
					if (ec != EC_SUCCESS) {
						_CRIT<< "Unable to set settings parameters of node "
						<< _str_name.c_str();
						/* Report error and bypass release */
						c_task->f_error("Unable to set settings parameters");
						ec = EC_FAILURE;
						goto out_err;
					}
				}

						/* Send state to parent node */
				{
					CT_PORT_NODE_GUARD & c_port_node =
							c_task->f_reply_get_node();
					c_port_node->set_data<ST_CORE_ACK>(
							ST_CORE_ACK(E_ID_CORE_SETTINGS_ACK, E_ACK_SUCCESS));
					c_port_node->set_id(E_ID_CORE_SETTINGS_ACK);

					ec = f_port_cfg_get("__parent__").f_send(c_port_node);
					if (ec != EC_SUCCESS) {
						CRIT("Unable to execute cfg event (%s)",
								_str_name.c_str());
						goto out_err;
					}
				}

			}
			break;
		}

		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_ABORTED:
		case E_TASK_STATUS_ERROR: {
			CRIT("Task Error");

			/* Send state to parent node */
			{
				CT_PORT_NODE_GUARD & c_port_node = c_task->f_reply_get_node();
				c_port_node->set_data<ST_CORE_ACK>(
						ST_CORE_ACK(E_ID_CORE_SETTINGS_ACK, E_ACK_FAILURE));
				c_port_node->set_id(E_ID_CORE_SETTINGS_ACK);

				ec = f_port_cfg_get("__parent__").f_send(c_port_node);
				if (ec != EC_SUCCESS) {
					CRIT("Unable to execute cfg event (%s)", _str_name.c_str());
					goto out_err;
				}
			}
		}

		default:
			break;
		}

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_TASK_COMPLETE:
		case E_TASK_STATUS_KILLED:
		case E_TASK_STATUS_ABORTED:
			//case E_TASK_STATUS_ERROR:
		{
			CT_HOST::tasks->f_release(c_task);
			break;
		}
		default:
			break;
		}

		break;
	}
	case E_ID_CORE_SETTINGS_ACK: {
	//	D("E_ID_CORE_SETTINGS_ACK for node %s", f_get_name().c_str());
		ST_TASK_STATUS & s_status = CT_TASK::f_reply_get_status(*in_c_node);
		ec = in_c_node->map<ST_CORE_ACK>().i_ack;
		//D("Task %d", s_status.i_uuid);
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(s_status);

		if (!c_task) {
			WARN("Task seems to be closed");
		} else {
			if (ec == EC_SUCCESS) {
				c_task->f_complete(s_status);
			} else {
				c_task->f_error();
			}
		}
		break;
	}

	case E_ID_CORE_CONFIGURE: {
		//D("E_ID_CORE_CONFIGURE for node %s", f_get_name().c_str());

		/* Create Task */
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_new<CT_NODE>(
				f_string_format("CORE_CONFIGURE_%s", f_get_name().c_str()));

		c_task->f_notification_set_id(E_ID_CORE_CONFIGURE_STATUS);
		c_task->f_notification_set_internal_port(f_port_cfg_get("__parent__"));
		/* Fill reply node with task status if present */
		c_task->f_reply_set_parent_task(*in_c_node);

		/* Configure current node */
		{
			CT_NODE c_node;
			std::string str_tmp;
			str_tmp=in_c_node->get_data<std::string>();
			c_node.from_xml(str_tmp);
			/* Create childs and ports of current node */
			ec = f_configure(c_node);
			if (ec != EC_SUCCESS) {
				_CRIT<< "Unable to configure :" << _str_name;
				c_task->f_error();
				goto out_err;
			}

		}

				/* Forward creation to childs */
		{

			//D("New Task %d", c_task->f_get_uuid());

			/* Copy node into task */
			CT_NODE & c_node = c_task->f_get_data<CT_NODE>();
			c_node.from_xml(in_c_node->get_data<std::string>());
			CT_NODE::childs_result v_tmp = c_node.get_childs("node");

			/* Describe task */
			c_task->f_step_set_nb(v_tmp.size());

			uint32_t i_part = 0;

			/* Loop on childs */
			for (CT_NODE::childs_iterator pc_it = v_tmp.begin();
					pc_it != v_tmp.end(); pc_it++) {
				std::string str_name;

				/* Use type if no name */
				if (!(*pc_it)->has("name")) {
					str_name = (std::string) * ((*pc_it)->get("type"));
				} else {
					str_name = (std::string) * ((*pc_it)->get("name"));
				}

				//D("Send E_ID_CORE_CONFIGURE to %s", str_name.c_str());
				/* Send msg with node info */
				{
					CT_PORT_NODE_GUARD c_tmp(E_ID_CORE_CONFIGURE,
							(*pc_it)->to_xml());

					/* Init task subnode */
					c_task->f_status_init(*c_tmp, i_part);

					M_ASSERT(f_port_cfg_has(str_name) == EC_SUCCESS);

					/* Send msg */
					ec = f_port_cfg_get(str_name).f_send(c_tmp);
					if (ec != EC_SUCCESS) {
						CRIT(
								"Unable to send settings event (child:%s, current:%s)",
								"", _str_name.c_str());
						c_task->f_error();
						goto out_err;
					}
				}

				i_part++;
			}
			M_ASSERT(i_part == c_task->f_step_get_nb());

			/* Enable task, complete is triggered here if no step/subpart */
			c_task->f_enable();
		}

		break;
	}
	case E_ID_CORE_CONFIGURE_STATUS: {
		//D("E_ID_CORE_CONFIGURE_STATUS for node %s", f_get_name().c_str());
		ST_TASK_NOTIFICATION & c_notif = in_c_node->map<ST_TASK_NOTIFICATION>();
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(c_notif.s_status);
		//D("Received task status : %d", c_notif.e_status);

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TASK_COMPLETE: {
			//D("Task complete");
			CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(c_notif.s_status);

			if (c_task) {
				CT_NODE & c_node = c_task->f_get_data<CT_NODE>();

				/* Configure current node after child */
				if (c_node.has("settings")) {
					D("Configure node %s", _str_name.c_str());
					ec = f_settings(*(c_node.get("settings")));
					if (ec != EC_SUCCESS) {
						CRIT("Unable to set child parameters");
						c_task->f_error("Unable to set settings parameters");
						ec = EC_FAILURE;
						goto out_err;
					}
				}

				/* Send state to parent node */
				{
					CT_PORT_NODE_GUARD & c_port_node =
							c_task->f_reply_get_node();
					c_port_node->set_data<ST_CORE_ACK>(
							ST_CORE_ACK(E_ID_CORE_CONFIGURE_ACK,
									E_ACK_SUCCESS));
					c_port_node->set_id(E_ID_CORE_ACK);


					ec = f_port_cfg_get("__parent__").f_send(c_port_node);
					if (ec != EC_SUCCESS) {
						CRIT("Unable to execute cfg event (%s)",
								_str_name.c_str());
						goto out_err;
					}
				}
			}
			break;
		}
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_ABORTED:
		case E_TASK_STATUS_ERROR: {
			CRIT("Task Error");

			/* Send state to parent node */
			{
				CT_PORT_NODE_GUARD & c_port_node = c_task->f_reply_get_node();
				c_port_node->set_data<ST_CORE_ACK>(
						ST_CORE_ACK(E_ID_CORE_SETTINGS_ACK, E_ACK_FAILURE));
				c_port_node->set_id(E_ID_CORE_SETTINGS_ACK);

				ec = f_port_cfg_get("__parent__").f_send(c_port_node);
				if (ec != EC_SUCCESS) {
					CRIT("Unable to execute cfg event (%s)", _str_name.c_str());
					goto out_err;
				}
			}
			break;
		}
		default:
			break;
		}

		switch (c_notif.e_status) {
		case E_TASK_STATUS_TIMEOUT:
		case E_TASK_STATUS_TASK_COMPLETE:
		case E_TASK_STATUS_KILLED:
		case E_TASK_STATUS_ABORTED:
			//case E_TASK_STATUS_ERROR:
		{
			CT_HOST::tasks->f_release(c_task);
			break;
		}
		default:
			break;
		}

		break;
	}
	case E_ID_CORE_CONFIGURE_ACK: {
		//D("E_ID_CORE_CONFIGURE_ACK for node %s", f_get_name().c_str());
		ST_TASK_STATUS & s_status = CT_TASK::f_reply_get_status(*in_c_node);
		ec = in_c_node->get_data<int>();
		//D("Task %d", s_status.i_uuid);
		CT_GUARD<CT_TASK> c_task = CT_HOST::tasks->f_get(s_status);

		if (!c_task) {
			WARN("Task seems to be closed");
		} else {
			if (ec == EC_SUCCESS) {
				c_task->f_complete(s_status);
			} else {
				c_task->f_error();
			}
		}
		break;
	}
	}

	ec = EC_SUCCESS;

	out_err: return ec;
}

int CT_CORE::f_post_init(void) {
	//D("Default post init %s", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_pre_destroy(void) {
	//D("Default pre destroy %s", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_post_profile(void) {
	//D("Default post profile %s", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_post_settings(void) {
	D("Default post settings %s", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_pre_profile(void) {
	//D("Default pre profile %s", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_state_save(CT_NODE & out_c_node) {
	//D("Default state save %s", _str_name.c_str());
	return EC_SUCCESS;
}
#if 0
int CT_CORE::f_state_notify(CT_NODE & out_c_node) {
	//D("Default state save end %s", _str_name.c_str());
	return EC_SUCCESS;
}

int CT_CORE::f_state_restore(CT_NODE & in_c_profile) {
	//D("Default state restore %s", _str_name.c_str());
	return EC_SUCCESS;
}
#endif
/* Quick reply function */
int CT_CORE::f_reply(CT_GUARD<CT_PORT_NODE> & in_c_node) {
	/* SAve record id */
	CT_HOST_CONTEXT::context_reply_id = in_c_node->get_id();

	if (CT_HOST_CONTEXT::context_port) {
		/*D("Rply to context port %s [%p]",
				CT_HOST_CONTEXT::context_port->str_name.c_str(),
				CT_HOST_CONTEXT::context_port)*/
		return CT_HOST_CONTEXT::context_port->f_send(in_c_node);
	} else {
		CRIT("Reply No Context: ID %x", in_c_node->get_id());
		return EC_FAILURE;
	}

}
#if 0
void CT_CORE::selfdestruct(void) {
	delete this;
}
#endif
