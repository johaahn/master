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

/**
 * @file node.cc
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Feb 9, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <host.hh>
#include <node.hh>
#include <cpp/string.hh>

#include <iostream>
#include <sstream>
#include <unistd.h>
using namespace std;
using namespace bml;

CT_NODE::CT_NODE() :
		node() {
	_e_type = E_NODE_TYPE_RAW;
}

CT_NODE::~CT_NODE() {
	//D("destructor CT_NODE");
}

CT_GUARD<CT_NODE> CT_NODE::get(std::string const &in_str_name,
		uint64_t in_i_indice) {
	CT_GUARD<node<std::string, CT_GUARD>> c_tmp =
			node<std::string, CT_GUARD>::get(in_str_name, in_i_indice, true);
	return dynamic_pointer_cast<CT_NODE>(c_tmp);
}

CT_GUARD<CT_NODE> CT_NODE::add(std::string in_str_name) {
	return dynamic_pointer_cast<CT_NODE>(
			node<std::string, CT_GUARD>::add(in_str_name));
}

CT_NODE::childs_result CT_NODE::get_childs(std::string in_str_name) {
	CT_NODE::childs_result v_tmp;
	std::pair<it, it> c_ret = _m_childs.equal_range(in_str_name);

	for (it pc_it = c_ret.first; pc_it != c_ret.second; ++pc_it) {
		v_tmp.push_back(static_cast<CT_NODE*>(pc_it->second.get()));
	}

	return v_tmp;
}

CT_NODE::childs_result CT_NODE::get_childs(void) {
	CT_NODE::childs_result v_tmp;
	for (it pc_it = _m_childs.begin(); pc_it != _m_childs.end(); ++pc_it) {
		v_tmp.push_back(static_cast<CT_NODE*>(pc_it->second.get()));
	}
	return v_tmp;
}
#if 0
bool CT_NODE::has(uint64_t in_i_id, uint64_t in_i_indice) {
	return node::has(in_i_id, in_i_indice);
}

bool CT_NODE::has(std::string in_str_name, uint64_t in_i_indice) {
	node::find_result v_tmp = find(in_str_name);
	//D("%s", in_str_name.c_str());
	if (v_tmp.size()) {
		return true;
	} else {
		return false;
	}
}

CT_NODE & CT_NODE::get(std::string in_str_name, uint64_t in_i_indice) {
	node::find_result v_tmp = find(in_str_name);
	try {
		node::iterator pc_node = v_tmp.at(in_i_indice);
		return *static_cast<CT_NODE*>(pc_node->second);
	} catch (...) {
		if (v_tmp.size() == in_i_indice) {
			return add(in_str_name);
		} else {
			throw std::runtime_error("Indice is incorrect :" + in_str_name);
		}
	}
}

CT_NODE & CT_NODE::get(uint64_t in_i_id, uint64_t in_i_indice,
		bool in_b_create) {
	return static_cast<CT_NODE&>(node::get(in_i_id, in_i_indice, in_b_create));
}

node::find_result CT_NODE::find(std::string in_str_name) {
	node::find_result v_tmp;
	for (node::iterator pc_it = _m_childs.begin(); pc_it != _m_childs.end();
			++pc_it) {
		//D("%p: Parse node %p", this, pc_it->second);
		if (pc_it->second->has(E_NODE_ID_NAME)) {
			CT_NODE & c_tmp = static_cast<CT_NODE&>(pc_it->second->get(
							E_NODE_ID_NAME));
			//D("%s %s", std::string(c_tmp).c_str(), in_str_name.c_str());

			if (std::string(c_tmp) == in_str_name) {
				v_tmp.push_back(pc_it);
			}
		}
	}

	return v_tmp;
}
node::find_result CT_NODE::childs(void) {
	node::find_result v_tmp;

	for (node::iterator pc_it = _m_childs.begin(); pc_it != _m_childs.end();
			++pc_it) {
		if (pc_it->first != E_NODE_ID_NAME) {
			v_tmp.push_back(pc_it);
		}
	}

	return v_tmp;
}

CT_NODE::childs_result CT_NODE::get_childs(std::string in_str_name) {
	CT_NODE::childs_result v_tmp;
	for (node::iterator pc_it = _m_childs.begin(); pc_it != _m_childs.end();
			++pc_it) {
		//D("%p: Parse node %p", this, pc_it->second);
		if (pc_it->second->has(E_NODE_ID_NAME)) {
			CT_NODE & c_tmp = static_cast<CT_NODE&>(pc_it->second->get(
							E_NODE_ID_NAME));
			//D("%s %s", std::string(c_tmp).c_str(), in_str_name.c_str());

			if (std::string(c_tmp) == in_str_name) {
				v_tmp.push_back(static_cast<CT_NODE*>(pc_it->second));
			}
		}
	}

	return v_tmp;
}

CT_NODE::childs_result CT_NODE::get_childs(void) {
	CT_NODE::childs_result v_tmp;
	for (node::iterator pc_it = _m_childs.begin(); pc_it != _m_childs.end();
			++pc_it) {
		//D("%p: Parse node %p", this, pc_it->second);
		if (pc_it->second->has(E_NODE_ID_NAME)) {
			v_tmp.push_back(static_cast<CT_NODE*>(pc_it->second));
		}
	}
	return v_tmp;
}

CT_NODE & CT_NODE::add_from(std::string in_str_name, CT_NODE & in_c_node) {

	std::pair<int64_t, node*> c_tmp;
	/* Fill pair values */
	c_tmp.first = E_NODE_ID_CHILD_XML;
	c_tmp.second = static_cast<node*>(&in_c_node);

	/* Update name child with name */
	in_c_node.set_name(in_str_name);
	//D("%p: Adding node %p %s %d",this, &in_c_node, in_str_name.c_str(), (bool)in_c_node.has((uint64_t)E_NODE_ID_NAME));

	/* Insert pair value */
	/*node::iterator c_it = */
	_m_childs.insert(c_tmp);

	return *static_cast<CT_NODE*>(c_tmp.second);
}

CT_NODE & CT_NODE::add(std::string in_str_name) {
	/* Create new node */
	CT_NODE * pc_node = (CT_NODE*) on_new();

	/* Insert created node */
	return add_from(in_str_name, *pc_node);
}

#endif

int CT_NODE::from_xml_file(std::string const & in_str_file, bool in_b_create) {
	int ec;
	clear();

	if (access(in_str_file.c_str(), R_OK)) {
		CRIT("Unable to open xml file: \"%s\"", in_str_file.c_str());
		ec = EC_FAILURE;
		goto out_err;
	}

	{
		ezxml_t ps_node_root = ezxml_parse_file((char*) in_str_file.c_str());
		if (!ps_node_root->name) {
			CRIT("Unable to read xml file");
			ec = EC_FAILURE;
			goto out_err;
		}
		from_ezxml_node(ps_node_root, in_b_create);
		ezxml_free(ps_node_root);
	}

	//D("XML DUMP: %s", f_stringify().c_str());

	ec = EC_SUCCESS;
	out_err: return ec;

}

int CT_NODE::from_xml(std::string const & in_str_xml) {
	clear();
	//printf("%s", (char*) in_str_xml.c_str());
	char *pc_tmp = strdup(in_str_xml.c_str());
	ezxml_t ps_node_root = ezxml_parse_str((char*)pc_tmp,
			in_str_xml.size());
	//printf("%s", (char*) pc_tmp);
	from_ezxml_node(ps_node_root, true);
	ezxml_free(ps_node_root);
	free(pc_tmp);

	return EC_SUCCESS;
}

void CT_NODE::to_xml_file(std::string const & in_str_file) {
	std::ofstream c_file;
	c_file.open(in_str_file.c_str());
	c_file << to_xml();
	c_file.close();
}

std::string CT_NODE::to_xml(uint in_i_level) {
	std::string str_tab = "";

	std::string str_name = (std::string) _id;
	std::string str_value = (std::string) *this;

	for (uint i = 0; i < in_i_level; i++) {
		str_tab += "\t";
	}

	std::string str_tmp = "";

	if (in_i_level == 0) {
		str_tmp += "<?xml version='1.0'?>\n";
	}

	CT_NODE::childs_result v_tmp = get_childs();
	if (v_tmp.size()) {
		str_tmp += str_tab + "<" + f_string_name2xml(str_name) + ">\n";

		if (str_value.size()) {
			str_tmp += str_tab + " " + str_value + "\n";
		}

		for (CT_NODE::childs_iterator pv_it = v_tmp.begin();
				pv_it != v_tmp.end(); pv_it++) {

			str_tmp += (*pv_it)->to_xml(in_i_level + 1);
		}

		str_tmp += str_tab + "</" + f_string_name2xml(str_name) + ">\n";
	} else {
		str_tmp += str_tab + "<" + f_string_name2xml(str_name) + ">" + str_value
				+ "</" + f_string_name2xml(str_name) + ">\n";
	}

	return str_tmp;
}

int CT_NODE::from_ezxml_node(ezxml_t in_ps_node, bool in_b_create) {
	std::string str_name;
	std::string str_value;

	if (in_ps_node->name == NULL) {
		str_name = "";
		str_value = "";
	} else {
		str_name = f_string_xml2name(in_ps_node->name);
		str_value = f_string_remove_whitespaces(in_ps_node->txt);
	}

	//D("FROM NODE:%s %s",str_name.c_str(), str_value.c_str());

	_id = str_name;
	*this = str_value;

	for (char ** ppc_attr = in_ps_node->attr; *ppc_attr; ppc_attr += 2) {
		std::string str_name = *ppc_attr;
		std::string str_value = *(ppc_attr + 1);
		//D("ATTR:%s %s", str_name.c_str(),str_value.c_str());
		if (has(str_name, 0)) {
			*get(str_name, 0) = str_value;
		} else if (in_b_create) {
			*add(str_name) = str_value;
		}
	}

	for (ezxml_t ps_node = in_ps_node->child; ps_node;
			ps_node = ps_node->sibling) {
		uint i_id = 0;
		for (ezxml_t ps_node_same = ps_node; ps_node_same; ps_node_same =
				ps_node_same->next) {
			//D("CHILD :%s %d %s %s", ps_node_same->name, i_id, str_name.c_str(), f_string_xml2name(ps_node_same->name).c_str());
			if (has(f_string_xml2name(ps_node_same->name), i_id)) {
				CT_NODE & c_node = *get(f_string_xml2name(ps_node_same->name),
						i_id);
				c_node.from_ezxml_node(ps_node_same, in_b_create);
			} else if (in_b_create) {
				CT_NODE & c_node = static_cast<CT_NODE&>(*add(
						f_string_xml2name(ps_node_same->name)));
				c_node.from_ezxml_node(ps_node_same, in_b_create);
			}
			assert(has(f_string_xml2name(ps_node_same->name), i_id));

			i_id++;
		}
	}
	//D("END NODE %s ", str_name.c_str());

	return EC_SUCCESS;
}
#if 0
CT_NODE & CT_NODE::operator()(uint64_t in_i_id, uint64_t in_i_indice) {
	return get(in_i_id, in_i_indice);
}
#endif

CT_NODE & CT_NODE::operator()(std::string in_str_name, uint64_t in_i_indice) {
	return *get(in_str_name, in_i_indice);
}

CT_NODE & CT_NODE::operator=(std::string const & other) {

	if (_s_segment.isempty() && (_e_type == E_NODE_TYPE_RAW)) {
		_e_type = E_NODE_TYPE_CHAR;
	}
	//D("%d", _e_type);

	switch (_e_type) {
	case E_NODE_TYPE_CHAR: {

		resize(other.size() + 1);
		//D("%p %d %d %d",get_segment()._pc_resource, get_segment()._pc_resource->size(),get_segment()._off_data, get_segment()._sz_data);
		_s_segment.memcpy_from_buffer(other.c_str(), other.size() + 1);
		//_s_segment.unmmap();
		break;
	}
	case E_NODE_TYPE_INT64: {
		int64_t i_tmp;
		std::istringstream instr(other);
		instr >> i_tmp;
		resize(sizeof(int64_t));
		_s_segment.memcpy_from_buffer((char*) &i_tmp, sizeof(int64_t));
		break;
	}
	default:
		throw _CRIT << "Current type " << _e_type
				<< "does not support string assignement";
		break;
	}

	return *this;
}
#if 0
CT_NODE & CT_NODE::operator=(bool const & other) {
	return this->operator =(int64_t(other));
}
#endif
CT_NODE & CT_NODE::operator=(int const & other) {
	return this->operator =(int64_t(other));
}
CT_NODE & CT_NODE::operator=(int64_t const & other) {
	if (_s_segment.isempty() && (_e_type == E_NODE_TYPE_RAW)) {
		_e_type = E_NODE_TYPE_INT64;
	}

	switch (_e_type) {
	case E_NODE_TYPE_INT64: {
		resize(sizeof(int64_t));
		_s_segment.memcpy_from_buffer((char*) &other, sizeof(int64_t));
		break;
	}
	case E_NODE_TYPE_CHAR: {
		std::string str_tmp;
		std::stringstream convertor;
		convertor << other;
		//_s_segment.unmmap();
		str_tmp = convertor.str();
		resize(str_tmp.size() + 1);
		_s_segment.memcpy_from_buffer(str_tmp.c_str(), str_tmp.size() + 1);
		break;
	}
	default:
		throw _CRIT << "Current type " << _e_type
				<< " does not support int64_t assignment";
		break;
	}

	return *this;
}
CT_NODE & CT_NODE::operator=(double const & other) {
	if (_s_segment.isempty() && (_e_type == E_NODE_TYPE_RAW)) {
		_e_type = E_NODE_TYPE_DOUBLE;
	}

	switch (_e_type) {
	case E_NODE_TYPE_DOUBLE: {
		resize(sizeof(double));
		_s_segment.memcpy_from_buffer((char*) &other, sizeof(double));
		break;
	}
	case E_NODE_TYPE_CHAR: {
		std::string str_tmp;
		std::stringstream convertor;
		convertor << other;
		//_s_segment.unmmap();
		str_tmp = convertor.str();
		resize(str_tmp.size() + 1);
		_s_segment.memcpy_from_buffer(str_tmp.c_str(), str_tmp.size() + 1);
		break;
	}
	default:
		throw _CRIT << "Current type " << _e_type
				<< " does not support double assignment";
		break;
	}

	return *this;
}
#if 0
/* cast operator */
template<typename T2>
CT_NODE::operator T2() const {
	M_BUG();
	T2 tmp = 0;
	return tmp;
}
#endif
//template<>
CT_NODE::operator std::string() const {
	//D("%d", _e_type);
	if (_s_segment.isempty()) {
		return std::string("");
	}

	switch (_e_type) {
	case E_NODE_TYPE_DOUBLE: {
		std::string str_tmp;
		std::stringstream convertor;
		void * pv_data = _s_segment.mmap();
		convertor << *((double*) pv_data);
		str_tmp = convertor.str();
		return str_tmp;
	}
		break;
	case E_NODE_TYPE_INT64: {
		std::string str_tmp;
		std::stringstream convertor;
		void * pv_data = _s_segment.mmap();
		convertor << *((int64_t*) pv_data);
		//_s_segment.unmmap();
		str_tmp = convertor.str();
		return str_tmp;
	}
		break;
	case E_NODE_TYPE_CHAR: {
		void * pv_data = _s_segment.mmap();
		std::string str_tmp = std::string((char*) pv_data);
		//_s_segment.unmmap();
		return str_tmp;
	}
		break;
	default:
		throw _CRIT << "Current type " << _e_type
				<< "does not support string cast";
		break;
	}
	return 0;
}

CT_NODE::operator int() const {
	return operator int64_t();
}
CT_NODE::operator unsigned int() const {
	return operator int64_t();
}
CT_NODE::operator uint64_t() const {
	return operator int64_t();
}
CT_NODE::operator int64_t() const {
	int64_t i_tmp;
	if (_s_segment.isempty()) {
		return 0;
	}

	switch (_e_type) {
	case E_NODE_TYPE_INT64: {
		void * pv_data = _s_segment.mmap();
		i_tmp = *((int64_t*) pv_data);
		//_s_segment.unmmap();
		return i_tmp;
	}
		break;
	case E_NODE_TYPE_CHAR: {
		void * pv_data = _s_segment.mmap();
		std::istringstream instr(std::string((char*) pv_data));
		instr >> i_tmp;
		//_s_segment.unmmap();
		return i_tmp;
	}
		break;
	default:
		throw _CRIT << "Current type " << _e_type
				<< " does not support int64_t cast";
		break;
	}
	return 0;
}
CT_NODE::operator double() const {
	double f_tmp;
	if (_s_segment.isempty()) {
		return 0;
	}

	switch (_e_type) {
	case E_NODE_TYPE_DOUBLE: {
		M_ASSERT(_s_segment.size() > sizeof(double));
		void * pv_data = _s_segment.mmap();
		f_tmp = *((double*) pv_data);
		//_s_segment.unmmap();
		return f_tmp;
	}
		break;
	case E_NODE_TYPE_INT64: {
		void * pv_data = _s_segment.mmap();
		f_tmp = *((int64_t*) pv_data);
		//_s_segment.unmmap();
		return f_tmp;
	}
		break;
	case E_NODE_TYPE_CHAR: {
		void * pv_data = _s_segment.mmap();
		std::istringstream instr(std::string((char*) pv_data));
		instr >> f_tmp;
		//_s_segment.unmmap();
		return f_tmp;
	}
		break;
	default:
		throw _CRIT << "Current type " << _e_type
				<< " does not support double cast";
		break;
	}
	return 0;
}
CT_NODE::operator bool() const {
	if (_s_segment.isempty()) {
		return 0;
	}

	switch (_e_type) {
	case E_NODE_TYPE_INT64: {
		void * pv_data = _s_segment.mmap();
		if(*((int64_t*) pv_data)) {
			return true;
		} else {
			return false;
		}
	}
		break;
	case E_NODE_TYPE_CHAR: {
		void * pv_data = _s_segment.mmap();
		if(f_string_tolower(std::string((char*) pv_data)) == "false" || (std::string((char*) pv_data) == "0")) {
			return false;
		} else {
			return true;
		}
	}
		break;
	default:
		throw _CRIT << "Current type " << _e_type
				<< " does not support boost cast";
		break;
	}
	return 0;
}

CT_GUARD<node<std::string, CT_GUARD>> CT_NODE::on_new(void) {
	CT_NODE * pc_node = new CT_NODE();
	if (!pc_node) {
		std::runtime_error("Unable to allocate memory");
	}
	return CT_GUARD<node<std::string, CT_GUARD>>(pc_node);
}

void CT_NODE::on_read(void) {
	_e_type =
			(ET_NODE_TYPE) _m_ext_values.at(E_NODE_EXT_TYPE).get_data<uint64_t>();
	//D("%d", _e_type);
}

void CT_NODE::on_write(void) {
	if (!_m_ext_values[E_NODE_EXT_TYPE].size()) {
		node_resource<CT_GUARD> * pc_tmp = new node_alloc_resource<CT_GUARD>(
				sizeof(uint64_t));
		//CT_HOST::host->f_get_cpu_resource().f_alloc_resource(sizeof(uint64_t));
		_m_ext_values[E_NODE_EXT_TYPE].set_from_resource(
				CT_GUARD<node_resource<CT_GUARD>>(pc_tmp));
	}
	M_ASSERT(_m_ext_values[E_NODE_EXT_TYPE].size() == sizeof(uint64_t));

	_m_ext_values[E_NODE_EXT_TYPE].set_data<uint64_t>(_e_type);
	//D("%d", _e_type);
}

void CT_NODE::set_type(enum ET_NODE_TYPE in_e_type) {
	M_ASSERT(_s_segment.isempty());
	_e_type = in_e_type;
}

void f_test_node(void) {
	// TEST 1
	CT_NODE c_node;
	DBG_LINE();
	c_node("0")("0")("0") = 456;
	DBG_LINE();
	c_node("0")("1") = 1;
	DBG_LINE();
	c_node("0") = "test";
	DBG_LINE();
	cout << std::string(c_node("0")("0")("0")) << endl;
	//c_parser = node_file_parser("/tmp/test.bml");

	cout << c_node.to_xml() << endl;
	c_node.to_file("/tmp/test.bml");

	CT_NODE c_node_read;
	c_node_read.from_file("/tmp/test.bml");
	if (std::string(c_node_read("0")) != "test") {
		throw std::runtime_error(
				"string bml failed :");
	}
	if (int64_t(c_node_read("0")("1")) != 1) {
		throw std::runtime_error("int bml failed");
	}
	if (std::string(c_node_read("0")("0")("0")) != "456") {
		throw std::runtime_error("int bml failed");
	}

	//TEST 2
	CT_NODE c_node_xml;
	D("TEST NAME ID");
	c_node_xml.set_name("root");
	c_node_xml("tata") = "tutu";

	D("TEST NAME ID 2");
	c_node_xml("tata")("toto") = "tutu2";

	D("TEST NAME ID 3");
	c_node_xml.to_xml_file("/tmp/test.xml");

	CT_NODE c_node_xml2;
	D("TEST NAME ID 4");
	c_node_xml2.from_xml_file("/tmp/test.xml");

	if (std::string(c_node_xml2("tata")("toto")) != "tutu2") {
		throw std::runtime_error(
				"xml failed:" + std::string(c_node_xml2("tata")("toto")));
	}
	cout << c_node_xml2 << endl;

#if 0
	sz_tmp = c_node.size();
	c_node.to_memory(pv_tmp);
	c_node.from_memory(pv_tmp);
#endif

}

void CT_NODE::copy_link(node<std::string, CT_GUARD> const & other) {
	CT_NODE const & c_other = dynamic_cast<CT_NODE const &>(other);
	_e_type = c_other._e_type;
	node<std::string, CT_GUARD>::copy_link(other);
}
void CT_NODE::copy(node<std::string, CT_GUARD> const & other) {
	CT_NODE const & c_other = dynamic_cast<CT_NODE const &>(other);
	_e_type = c_other._e_type;
	//_e_type = other._e_type;
	node<std::string, CT_GUARD>::copy(other);
}
