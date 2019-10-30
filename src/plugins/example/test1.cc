/***********************************************************************
 ** test1.cc
 ***********************************************************************
 ** Copyright (c) SEAGNAL SAS
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms are permitted
 ** provided that the above copyright notice and this paragraph are
 ** duplicated in all such forms and that any documentation,
 ** advertising materials, and other materials related to such
 ** distribution and use acknowledge that the software was developed
 ** by the SEAGNAL. The name of the
 ** SEAGNAL may not be used to endorse or promote products derived
 ** from this software without specific prior written permission.
 ** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 ** IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ***********************************************************************/

/**
 * @file test.cc
 * Plugin TEST.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <test1.hh>
#include <event_test1.hh>
#include <event_test2.hh>

using namespace std;
using namespace master::plugins::test1;


CT_TEST1::CT_TEST1(void) :
    CT_CORE() {
  D("Creating TEST1 CORE [%p]", this);

  /* Create data port */
  f_port_create(E_TEST_PORT_1, "port1",  M_PORT_BIND(CT_TEST1, f_event, this));
  f_port_create(E_TEST_PORT_2, "port2", M_PORT_BIND(CT_TEST1, f_event, this));
  f_port_create(E_TEST_PORT_3, "port3", M_PORT_BIND(CT_TEST1, f_event, this));

  _pc_timeout = CT_GUARD<CT_HOST_TIMEOUT>(CT_HOST::host->f_new_timeout());
  M_ASSERT(_pc_timeout);

  _pc_context = CT_GUARD<CT_HOST_CONTEXT>(CT_HOST::host->f_new_context());
  M_ASSERT(_pc_context);

  _s_result.b_timeout = false;
  _s_result.i_loop = 0;

  CHECK_EXAMPLE_EVENT_TEST1();
  CHECK_EXAMPLE_EVENT_TEST2();
}

CT_TEST1::~CT_TEST1() {
}

int CT_TEST1::f_event(CT_GUARD<CT_PORT_NODE> const & in_pc_node) {
#if 1
  _DBG << "Receiving node: "<< std::hex << in_pc_node->get_id()
      << " ("<< in_pc_node->get_size()<< " bytes) "
      << CT_HOST::host->f_id_name(in_pc_node->get_id());
#endif
  switch (in_pc_node->get_id()) {
  case master::plugins::test2::E_ID_TEST2_MSG2: {
    {
      uint32_t i_tmp = 0xDEAD;
      CT_PORT_NODE_GUARD c_tmp(E_ID_TEST1_MSG3, i_tmp);
      f_port_get(E_TEST_PORT_3).f_send(c_tmp);
    }
    break;
  }
  default:
    break;
  }

  return EC_SUCCESS;
}

int CT_TEST1::f_post_init(void) {
  WARN("CT_TEST1 POSTINIT - All cores are now ready - Sending msg to port 1");
  {
    uint32_t i_tmp = 0xDEAD;
    CT_PORT_NODE_GUARD c_tmp(E_ID_TEST1_MSG1, i_tmp);
    f_port_get(E_TEST_PORT_1).f_send(c_tmp);
  }
  return EC_SUCCESS;
}

int CT_TEST1::f_pre_destroy(void) {
  WARN("CT_TEST1 PREDESTROY");
  return EC_SUCCESS;
}

int CT_TEST1::f_post_profile(void) {
  WARN("CT_TEST1 POSTPROFILE");
  return EC_SUCCESS;
}

int CT_TEST1::f_pre_profile(void) {
  WARN("CT_TEST1 PREPROFILE");
  return EC_SUCCESS;
}

int CT_TEST1::f_settings(CT_NODE & in_c_node) {
  WARN("CT_TEST1 SETTINGS");
  return EC_SUCCESS;
}

/* Macro needed to register plugin */
M_PLUGIN_CORE(CT_TEST1, "TEST1", M_STR_VERSION);
