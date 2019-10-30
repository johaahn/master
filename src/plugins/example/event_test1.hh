/***********************************************************************
 ** event.hh
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

/* define against mutual inclusion */
#ifndef TEST1_EVENT_HH_
#define TEST1_EVENT_HH_

/**
 * @file event.hh
 * EVENT of TEST Plugin definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
//#include <event_base.hh>
/***********************************************************************
 * Defines
 ***********************************************************************/


#ifdef ID_DEFINITION
namespace master {
  namespace plugins {
    namespace test1 {


enum ET_ID {
	E_ID_TEST1_MSG1 = 0x10,
	E_ID_TEST1_MSG3,
};


}
}
}
#else
#include <event_test1.autogen.hh>
#endif

#endif /* TEST1_EVENT_HH_ */
