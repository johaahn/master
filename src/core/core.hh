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

/* define against mutual inclusion */
#ifndef CORE_HH_
#define CORE_HH_

/**
 * @file core.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Jan 1, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <port.hh>
#include <plugin.hh>
#include <string>
#include <vector>
/***********************************************************************
 * Defines
 ***********************************************************************/

enum ET_ACK {
	E_ACK_SUCCESS = EC_SUCCESS,
	E_ACK_FAILURE = EC_FAILURE,
};

/*!
 * Core Acknowledge structure, used to Acknowledge a specific command.
 */
struct ST_CORE_ACK {
	/*!
	 * Acknowledge result.
	 * Error, Success, ...
	 */
	uint8_t i_ack;
	/*!
	 * Id of command Acknowledged.
	 */
	uint32_t i_id;

	/*!
	 * Constructor of Acknowledge structure from enum type Acknowledge.
	 */
	ST_CORE_ACK(uint32_t in_i_id, enum ET_ACK in_e_ack) {
		i_ack = (uint8_t)in_e_ack;
		i_id = in_i_id;
	}


	/*!
	 * Constructor of Acknowledge structure from int8 type Acknowledge.
	 */
	ST_CORE_ACK(uint32_t in_i_id, uint8_t in_i_ack) {
		i_ack = in_i_ack;
		i_id = in_i_id;
	}
} __attribute__((packed));

/*!
 * Enum of ID used by CORE base class.
 * Those IDs are used for:
 * - CORE Initialisation
 * - CORE Profile loading
 * - CORE State store/restore
 *
 */
enum ET_ID_CORE {
	/*! Common Acknowledge ID.
	 * Use ST_CORE_ACK as type of data
	 */
	E_ID_CORE_ACK = 0,
	/*!
	 * Task Notification Id of common process.
	 * Id used for progress notification of process
	 */
	E_ID_CORE_ACK_STATUS,

	/*!
	 * ID of post initialisation event.
	 * Id received on CT_CORE config port when:
	 * - all CORE profile have been loaded
	 * - all CT_PORT context have been started or resumed.
	 *
	 * This ID is forwarded to childs.
	 * Acknowledge is sent once all childs has replied.
	 *
	 * Acknowledged with: E_ID_CORE_ACK
	 * Task Notification Id: E_ID_CORE_ACK_STATUS
	 */
	E_ID_CORE_POST_INIT,
	/*!
	 * ID of pre-destroy event.
	 * ID received on CT_CORE config port when:
	 * 	- CT_CORE will be destructed
	 *
	 * This ID is forwarded to childs.
	 * Acknowledged with: E_ID_CORE_ACK
	 * Task Notification Id: E_ID_CORE_ACK_STATUS
	 */
	E_ID_CORE_PRE_DESTROY,
	/*!
	 * Id of pre-profile event.
	 * ID received on CT_CORE config port when:
	 *  - CT_HOST has just started the loading of a new profile
	 *  - CT_HOST is closing application
	 *
	 * This ID is forwarded to childs of CT_CORE.
	 * Acknowledged with: E_ID_CORE_ACK
	 * Task Notification Id: E_ID_CORE_ACK_STATUS
	 */
	E_ID_CORE_PRE_PROFILE,

 /*!
  * Id of Post Profile.
  * ID received on CT_CORE config port when:
  *   - CT_HOST has loaded whole profile
  *    (all CT_CORE instances are ready)
  *
  *
	* This ID is forwarded to childs of CT_CORE.
	* Acknowledged with: E_ID_CORE_ACK
	* Task Notification Id: E_ID_CORE_ACK_STATUS
  */
	E_ID_CORE_POST_PROFILE,
	/*!
	 * Id of post-settings.
	 * ID received on CT_CORE config port when:
   *   - CT_HOST has sent settings to each CT_CORE.
   *
 	 * This ID is forwarded to childs of CT_CORE.
 	 * Acknowledged with: E_ID_CORE_ACK
	 * Task Notification Id: E_ID_CORE_ACK_STATUS
	 */
	E_ID_CORE_POST_SETTINGS,

	/*!
	 * Id of save state event.
	 * ID received on CT_CORE config port when:
   *   - CT_HOST is saving states of CT_CORE.
   *
	 * This ID is forwarded to childs of CT_CORE.
	 *
	 * Acknowledged with Id:
	 *  - E_ID_CORE_SAVE_STATE_REPLY on success
	 *  - E_ID_CORE_SAVE_STATE_ERROR on failure
	 *
	 * Task Notification Id: E_ID_CORE_SAVE_STATE_STATUS
	 */
	E_ID_CORE_SAVE_STATE,
	/*!
	 * Task Notification Id of save state process.
	 * Id used for progress notification of save state process
	 */
	E_ID_CORE_SAVE_STATE_STATUS,
	/*!
	 * Acknowledge Id of save state process in case of success.
	 * Id used for Acknowledge of save state process.
	 *
	 * Data associated to this id is the result of the save state process.
	 * Data format is XML (CT_NODE dumped as XML).
	 */
	E_ID_CORE_SAVE_STATE_REPLY,
	/*!
	 * Acknowledge Id of save state process in case of error.
	 * Id used for Acknowledge of save state process
	 */
	E_ID_CORE_SAVE_STATE_ERROR,

	/*!
	 * Id of restore state event.
	 * ID received on CT_CORE config port when:
	 *   - CT_HOST is restoring states of CT_CORE.
	 *
	 * This ID is forwarded to childs of CT_CORE.
	 *
	 * Data associated to this id is the state to be restored.
	 * Data format is XML (CT_NODE dumped as XML).
	 *
	 * Acknowledged with Id: E_ID_CORE_SETTINGS_ACK
	 *
	 * Task Notification Id: E_ID_CORE_SETTINGS_STATUS
	 */
	E_ID_CORE_SETTINGS,
	/*!
	 * Task Notification Id of restore state process.
	 * Id used for progress notification of restore state process
	 */
	E_ID_CORE_SETTINGS_STATUS,

	/*!
	 * Acknowledge Id of restore state process in case of error.
	 * Id used for Acknowledge of restore state process
	 */
	E_ID_CORE_SETTINGS_ACK,

	/*!
	 * Id of configure event.
	 * ID received on CT_CORE config port when:
	 *   - CT_HOST is configuring its CT_COREs. (Creation of Nodes)
	 *
	 * This ID is forwarded to childs of CT_CORE once
	 * created.
	 * Acknowledged with Id: E_ID_CORE_CONFIGURE_ACK
	 *
	 * Task Notification Id: E_ID_CORE_CONFIGURE_STATUS
	 */
	E_ID_CORE_CONFIGURE,
	/*!
	 * Task Notification Id of configure process.
	 * Id used for progress notification of configure process
	 */
	E_ID_CORE_CONFIGURE_STATUS,

	/*!
	 * Acknowledge Id of configure process in case of error.
	 * Id used for Acknowledge of congiure process
	 */
	E_ID_CORE_CONFIGURE_ACK,

	/*!
	 * Task Notification Id of profile loading/update process.
	 * Id used for progress notification
	 */
	E_ID_CORE_HOST_CONFIG_LOAD_STATUS,
	/*!
	 * Task Notification Id of profile update during
   * application close.
	 * Id used for progress notification
	 */
	E_ID_CORE_HOST_CONFIG_CLOSE_STATUS,
  /*!
   * Number of Id dedicated to
   * internal CT_CORE mechanisms.
   */
	E_ID_CORE_NB_ID,
};

/***********************************************************************
 * Types
 ***********************************************************************/
/*!
 * ALICE : Abstraction Layer of Interface and Context with Efficiency
 * CRIL : Context, Resources and Interface Layer
 * CIBAL : Context, Interface and Buffer Abstraction Layer
 * CIC : Communication Interface Context
 * CCC : Communication Context Configuration
 * Keyword:
Communication
Context
Configuration
Modudar
Design

Optimized
ZeroCopy

Buffer
Settings

Abstraction
Layer
*/

/*!
 * CT_CORE class represent an entity, a module of a design.
 * The core:
 *  - can own multiple childs. (sub module or sub entity)
 *  - can communicate with other CT_CORE through communication ports
 *
 *
 */
class CT_CORE: public CT_PLUGIN  {
	/*! Map of external ports with integer key */
	std::map<uint, CT_PORT *> _v_ports;
  /*! Map of external ports integer key with string key */
	std::map<std::string, uint> _m_ports_index;


  /*! Map of dedicated configuration ports with string key.
	*	This map will contain :
	*	- an entry for each child using profile name as key.
	*	- an entry for parent (using key:__parent__)
	*
	* Those ports use the same interface but are dedicated to configuration.
	*
	* They will be used during:
	*  - profile loading
	*  - state save and restore
	*/
	std::map<std::string, CT_PORT *> _m_ports_cfg;

	/*! Map of childs with string key (name) */
	std::map<std::string, CT_GUARD<CT_CORE> > _m_sub_cores;

	/*! Port update id.
	 * Counter used to check if a port is still used after a profile update.
	 * Unused port are deleted during profile update.
	 */
	int _i_port_update;


	/*! Child update id.
	 * Counter used to check if all child are still used after a profile update.
	 * Unused child are deleted during profile update.
	 */
	int _i_sub_core_update;

	/*! Dynamic core status.
	 * This flag is set if the core is created dynamicaly not created through profile
	 */
	bool _b_dynamic;

	/*! Update id.
	 * Counter used to check if all child of a parent are still used after a profile update.
	 * Unused child are deleted during profile update.
	 */
	int _i_update;

	/*! Name of node.
	 * Each child must have a different name
	 */
	std::string _str_name;

	/*! Full Name of node.
	 * Full name is composed with parent hierarchy.
	 * ex: /parent/child0
 	 * Each child having a different name ensure a different full name.
 	 * Full name are used for configuration ports.
	 */
	std::string _str_full_name;

	/*! Pointer to parent node - DEBUG purpose.
	 * May not be coherent.
	 */
	CT_CORE * _pc_parent;




public:
	CT_CORE();
	virtual ~CT_CORE();


	/*!
	 * Get all childs in pointer vector format.
	 *
	 * OBSOLETE: Do not use in new code. Parent and child must communicate through
	 * configuration port. Not pointer.
	 *
	 * TODO: Remove from API
	 *
	 * @param  out_v_list output vector of pointer
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_childs_get_OBSOLETE(std::vector<CT_CORE *> & out_v_list);


private:
	/*!
	 * Configure childs from profile description in CT_NODE format.
	 *
	 * Instanciate child from tags "node" found inside CT_NODE (BML node with string key).
	 *
	 * @param  in_c_config BML node with string as key (XML like)
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_childs_configure(CT_NODE & in_c_config);

	/*!
	 * Pre-check of child configuration before settings update (restore state).
	 *
	 * Configuration is in CT_NODE format (BML node with string key)
	 * Configuration childs description must match exactly current childs
	 * instanciated.
	 * T
	 * @param  in_c_config BML node with string as key (XML like)
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_settings(CT_NODE & in_c_config);


	/*!
	 * Child profile loading.
	 *
	 * Profile is in CT_NODE format (BML node with string key)
	 * Acording to current childs instanciated, and input profile:
	 *  - it creates new childs that does not exists
	 *  - it replaces childs with same name but different type
	 *
	 * @param  in_c_config BML node with string as key (XML like)
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_configure(CT_NODE & in_c_config, bool in_b_dynamic);

	/*!
	 * Search a child with a name equivalent to in_str_name.
	 *
	 * @param  in_str_name name of child to be searched for
	 * @return             EC_SUCCESS in case of child found
	 *                     EC_FAILURE in case of child not found
	 */
	int f_child_has(std::string in_str_name);

	/*!
	 * Search and get a child with a name equivalent to in_str_name.
	 *
	 * @param  out_pc_child In case of child found, set with child pointer
	 * @param  in_str_name  name of child to be searched for
	 * @return             EC_SUCCESS in case of child found
	 *                     EC_FAILURE in case of child not found
	 */
	int f_child_tryget(CT_CORE*& out_pc_child, std::string in_str_name);

	/*!
	 * Create a child with specific name and type.
	 *
	 * @param  in_str_name name of child to be created
	 * @param  in_str_type type of child to be created
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_create(std::string in_str_name, std::string in_str_type);

	/*!
	 * Remove a child from name.
	 *
	 * @param  in_str_name name of child to be removed
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_remove(std::string in_str_name);

public:
	/*!
	 * Remove a child from pointer.
	 *
	 * @param  in_pc_node pointer of child to be removed
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_remove(CT_CORE * in_pc_node);


	/*!
	 * Remove all child.
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_clear(void);

	/*!
	 * Remove childs from loaded plugin.
	 *
	 * @param  in_ps_desc pointer of plugin description
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_child_remove(ST_PLUGIN_DESCRIPTION * in_ps_desc);

private:
	/*!
	 * Configure ports from profile description in CT_NODE format.
	 *
	 * Instanciate port from tags "port" found inside CT_NODE (BML node with string key).
	 *
	 * Port name of profile must match port instanciated during CT_CORE constructor
	 * with f_port_create().
	 *
	 * @param  in_c_config BML node with string as key (XML like)
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_ports_configure(CT_NODE & in_c_config);

	/*!
	 * Search a port with a name equivalent to in_str_name.
	 *
	 * @param  in_str_name name of port to be searched for
	 * @return             EC_SUCCESS in case of port found
	 *                     EC_FAILURE in case of port not found
	 */
	int f_port_has(std::string in_str_name);

	/*!
	 * Search and get a port with a name equivalent to in_str_name.
	 *
	 * @param  out_pc_port In case of port found, set with port pointer
	 * @param  in_str_name  name of port to be searched for
	 * @return             EC_SUCCESS in case of port found
	 *                     EC_FAILURE in case of port not found
	 */
	int f_port_tryget(CT_PORT*& out_pc_port, std::string in_str_name);

	/*!
	 * Destroy a port from name.
	 *
	 * @param  in_str_name name of port to be removed
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_port_clear(std::string in_str_name);

	/*!
	 * Destroy a port from integer.
	 *
	 * @param  in_str_name name of port to be removed
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_port_clear(uint in_i_id);
public:

	/*!
	 * Destroy all ports.
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_port_clear(void);

	/*!
	 * Create a port.
	 *
	 * Port is used to communicate between CT_CORE
	 *
	 * This method must be called from CT_CORE inherited class constructor.
	 * Dynamic port creation is forbidden.
	 *
	 * @param  in_i_id     Integer id of port to be created
	 * @param  in_str_name Name of port to be created
	 * @param  in_c_cb     Callback to to be called when CT_PORT_NODE is received
	 *                     on port to be created. Default is a NULL callback.
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_port_create(uint in_i_id,
		std::string in_str_name,
		CT_PORT_BIND_CORE const & in_c_cb = CT_PORT_BIND_NULL(EC_FAILURE));

public:
	/*!
	 * Clearing internal configuration ports.
	 *
	 * TODO: Should be private
	 * @return  [description]
	 */
	int f_port_cfg_clear(void);

private:
	/*!
	 * Create an internal configuration port.
	 *
	 * @param  in_str_name Unique name of port to be created
	 * @param  in_c_cb     Callback to to be called when CT_PORT_NODE is received
	 *                     on port to be created.
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_port_cfg_create(std::string in_str_name, CT_PORT_BIND_CORE const & in_c_cb);

	/*!
	 * Remove internal configuration port from name.
	 *
	 * @param  in_str_name name of port to be deleted
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_port_cfg_remove(std::string in_str_name);

	/*!
	 * Search a configuration port with a name equivalent to in_str_name.
	 *
	 * @param  in_str_name name of port to be searched for
	 *
	 * @return             EC_SUCCESS in case of port found
	 *                     EC_FAILURE in case of port not found
	 */
	int f_port_cfg_has(std::string in_str_name);

	/*!
	 * Configure ports and childs according to a profile.
	 *
	 * Profile is in CT_NODE format.
	 *
	 * According to profile,
	 *  - Map each created ports with a type.
	 *  - Create childs.
	 *
	 * @param  in_c_config BML node using string as key (XML like).
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_configure(CT_NODE & in_c_config);

public:
	/*!
	 * Apply settings from a profile or a saved state.
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_SETTINGS is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 * @param  in_c_config BML node using string as key (XML like)
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_settings(CT_NODE & in_c_config);

	/*!
	 * Post initialisation event.
	 *
	 * Called when:
	 * - all cores have been loaded
	 * - all contexts have been started or resumed.
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_POST_INIT is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_post_init(void);

	/*!
	 * Pre destroy event.
	 *
	 * Called when:
	 *  - CT_HOST has just started the loading of a new profile
	 *  - CT_HOST is closing application
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_PRE_DESTROY is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_pre_destroy(void);

	/*!
	 * Post profile event.
	 *
	 * Called when:
   *   - CT_HOST has loaded whole profile
   *    (all CT_CORE instances are ready)
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_POST_PROFILE is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_post_profile(void);


	/*!
	 * Post settings event.
	 *
	 * Called when:
   *   - CT_HOST has sent settings to each CT_CORE.
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_POST_SETTINGS is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_post_settings(void);

	/*!
	 * Pre profile event.
	 *
	 * Called when:
	 *  - CT_HOST has just started the loading of a new profile
	 *  - CT_HOST is closing application
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_PRE_PROFILE is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_pre_profile(void);


	/*!
	 * Save state event.
	 *
	 * Called when:
   *   - CT_HOST is saving states of CT_CORE.
	 *
	 * This is a virtual method.
	 *
	 * By default, this method is called when E_ID_CORE_SAVE_STATE is received from
	 * __parent__ configuration port from f_cfg_event().
	 *
	 * In order to handle internal core event, inherited class can either:
	 * - handle all configuration node individually from __parent__ configuration port
	 *   by overriding f_cfg_event() and replying with proper configuration node.
	 *   (ex: E_ID_CORE_ACK for E_ID_CORE_POST_INIT)
	 * or
	 * - override below methods individually according to needs:
	 *    f_settings() mapped to E_ID_CORE_SETTINGS
	 *    f_post_init() mapped to E_ID_CORE_POST_INIT
	 *    f_pre_destroy() mapped to E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() mapped to E_ID_CORE_POST_PROFILE
	 *    f_post_settings() mapped to E_ID_CORE_POST_SETTINGS
	 *    f_state_save() mapped to E_ID_CORE_SAVE_STATE
	 *
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_state_save(CT_NODE & in_c_profile);

	/* Usage selfdestruct function */
	//void selfdestruct(void);

	/*!
	 * Set short name of core.
	 *
	 * @param in_str_name new name of core
	 */
	void f_set_name(std::string const & in_str_name) { /*D("Set name: %s %p",in_str_name.c_str(), this);*/ _str_name = in_str_name; }

	/*!
	 * Set full name of core.
	 * Full name includes hierarchy of nodes.
	 *
	 * @param in_str_name new name of core
	 */
	void f_set_full_name(std::string const & in_str_name) { /*D("Set name: %s %p",in_str_name.c_str(), this);*/ _str_full_name = in_str_name; }

	/*!
	 * Get short name of core.
	 *
	 * @return  short name of core
	 */
	std::string const & f_get_name(void) { return _str_name; }
	/*!
	 * Get full name of core.
	 * Full name includes hierarchy of nodes.
	 *
	 * @return  full name of core
	 */
	std::string const & f_get_full_name(void) { return _str_full_name; }


	/*!
	 * Set parent.
	 *
	 * @param in_pc_parent pointer to a CT_CORE
	 */
	void f_set_parent(CT_CORE * in_pc_parent);

	/*!
	 * Get parent.
	 *
	 * Parent may not be available (ex: Root Node or distant core)
	 *
	 * @return  pointer of parent if available
	 */
	CT_CORE * f_get_parent(void) { return _pc_parent; }

/*!
 * Get dynamic flag of core.
 * Dynamic flag is true if core has been created after profile loading
 * @return true if created after profile loading
 *         false if created before profile loading
 */
	bool f_is_dynamic(void) { return _b_dynamic; }

	/*!
	 * Default Internal configuration port handler.
	 *
	 * By default, this method calls below virtual methods acording to received
	 * event :
	 *    f_settings() on E_ID_CORE_SETTINGS
	 *    f_post_init()  on E_ID_CORE_POST_INIT
	 *    f_pre_destroy()  on E_ID_CORE_PRE_DESTROY
	 *    f_post_profile() on E_ID_CORE_POST_PROFILE
	 *    f_post_settings() on E_ID_CORE_POST_SETTINGS
	 *    f_state_save() on E_ID_CORE_SAVE_STATE
	 *
	 * @param  in_c_node smart pointer of configuration node
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	virtual int f_cfg_event(CT_GUARD<CT_PORT_NODE> const & in_c_node);

	/*!
	 * Send a node to the context port.
	 *
	 * Context port is the port that manage current context.
	 * (from where f_reply() have been called)
	 *
	 * @param  in_c_node node to be sent
	 *
	 * @return             EC_SUCCESS in case of success
	 *                     EC_FAILURE in case of failure
	 */
	int f_reply(CT_GUARD<CT_PORT_NODE> & in_c_node);

	/*!
	 * Get port from an interger identifier.
	 *
	 * in_i_id must match one of the ids provided during the call of
	 * f_port_create().
	 *
	 * @param  in_i_id interger id to be searched for
	 * @return         reference to CT_PORT
	 */
	CT_PORT & f_port_get(int in_i_id) { return *_v_ports.at(in_i_id); }


	/*!
	 * Get port from a string identifier.
	 *
	 * in_str_name must match one of the name provided during the call of
	 * f_port_create().
	 *
	 *
	 * @param  in_str_name string name to be searched for
	 * @return         reference to CT_PORT
	 */
	CT_PORT & f_port_cfg_get(std::string const & in_str_name ) { return *_m_ports_cfg.at(in_str_name); }
};
#else
class CT_CORE;
#endif /* CORE_HH_ */
