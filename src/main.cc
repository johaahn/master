/***********************************************************************
 ** main.cc
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
 ** -------------------------------------------------------------------
 **
 ** As a special exception to the GNU Lesser General Public License, you
 ** may link, statically or dynamically, a "work that uses this Library"
 ** with a publicly distributed version of this Library to produce an
 ** executable file containing portions of this Library, and distribute
 ** that executable file under terms of your choice, without any of the
 ** additional requirements listed in clause 6 of the GNU Lesser General
 ** Public License.  By "a publicly distributed version of this Library",
 ** we mean either the unmodified Library as distributed by the copyright
 ** holder, or a modified version of this Library that is distributed under
 ** the conditions defined in clause 3 of the GNU Lesser General Public
 ** License.  This exception does not however invalidate any other reasons
 ** why the executable file might be covered by the GNU Lesser General
 ** Public License.
 **
 ***********************************************************************/

/**
 * @file main.cc
 * Main function of program.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2012
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <host.hh>
#include <main.hh>
#include <c/common.h>
#include <cpp/debug.hh>
#include <getopt.h>
#include <signal.h>

/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/
struct ST_HOST_ARGS s_args;

CT_MAIN::CT_MAIN(int & argc, char ** argv) {

	const char *optString = "rgvh";

	const struct option longOpts[] = { { "qt", no_argument, NULL, 'q' }, { "boost", no_argument, NULL, 'b' }, {
			"verbose", no_argument, NULL, 'v' }, { "help", no_argument, NULL,
			'h' }, { "config", required_argument, NULL, 'c' }, { "redirect",
	required_argument, NULL, 'r' }, { NULL,
	no_argument, NULL, 0 } };

	/* Default commands */
	s_args.argc = argc;
	s_args.argv = argv;
	s_args.str_config = "";
	s_args.str_redirect = "";
	#ifdef FF_HOST_QT
	s_args.b_gui_enabled = true;
	#else
	s_args.b_gui_enabled = false;
	#endif
	s_args.b_debug_enabled = false;

	char *pc_tmp = getenv("MASTER_CONFIG");
	if (!pc_tmp) {
		WARN("No master config specified");
	}
	s_args.str_config = pc_tmp ? pc_tmp : "";

	int ec;

	/* Parse arguments */
	int longIndex = 0;
	ec = getopt_long(argc, argv, optString, longOpts, &longIndex);
	while (ec != -1) {
		switch (ec) {
		case 'q':
			s_args.b_gui_enabled = true;
			D("QT");
			; /* true */
			break;
		case 'b':
			s_args.b_gui_enabled = false;
			D("BOOST");
			; /* true */
			break;
		case 'v':
			s_args.b_debug_enabled = true;
			break;

		case 'c':
			s_args.str_config = optarg;
			break;
		case 'r':
			s_args.str_redirect = optarg;
			break;
		case 'h': /* fall-through is intentional */
		case '?':
			usage();
			break;

		case 0: /* long option without a short arg */
			break;

		default:
			/* You won't actually get here. */
			break;
		}

		ec = getopt_long(argc, argv, optString, longOpts, &longIndex);
	}

	if (s_args.str_redirect.size()) {
		_WARN<< "REDIRECTING STDOUT to " <<s_args.str_redirect;
		M_ASSERT(freopen(s_args.str_redirect.c_str(), "w",stdout));
		_WARN << "REDIRECTING STDERR to " <<s_args.str_redirect;
		M_ASSERT(freopen(s_args.str_redirect.c_str(), "w",stderr));
	}

	{
		_pc_host = NULL;

#ifdef FF_HOST_QT
		if(s_args.b_gui_enabled)
		{
#ifdef FF_HOST_QT
			D("Enabling QT GUI Host");
			CT_HOST * f_host_qt(struct ST_HOST_ARGS * in_ps_args);
			_pc_host = f_host_qt(&s_args);
#endif
		} else {
			D("Enabling Boost Host");
			CT_HOST * f_host_boost(struct ST_HOST_ARGS * in_ps_args);
			_pc_host = f_host_boost(&s_args);
		}
#else
    {
      D("Enabling Boost Host");
      CT_HOST * f_host_boost(struct ST_HOST_ARGS * in_ps_args);
      _pc_host = f_host_boost(&s_args);
    }
#endif
		assert(_pc_host);
	}
}

CT_MAIN::~CT_MAIN() {
	D("main destructor");
	if (_pc_host) {
		delete (_pc_host);
	}
}

void CT_MAIN::usage(void) {

}

int CT_MAIN::init(void) {
	//_WARN<< "Init main";
	return _pc_host->f_main_init();
}

int CT_MAIN::run(void) {
	//_WARN<< "Running main";
	_pc_host->f_main_loop();

	//_WARN << "Closing main";
	_pc_host->f_main_close();
	return EC_SUCCESS;
}

void main_sig_handler(int sig) {
	CRIT("Signal %d caught...", sig);

}

#ifdef FF_GPROF
#include <google/profiler.h>
#endif

int main(int argc, char *argv[]) {
	int ec;
	//signal(SIGABRT, &main_sig_handler);
	//signal(SIGTERM, &main_sig_handler);
	//signal(SIGINT, &main_sig_handler);

	/* Initialize debug */
	//f_debug_init(NULL);
	gc_memtrack.f_start();

#ifdef FF_GPROF
	ProfilerStart("/tmp/gprof.prof");
#endif
#if 0
	try {
#endif
	//f_test_node();
	CT_DEBUG::f_signal_init();

	{
		//_DBG<< "Start MAIN Creation";
		/* Create Main class */
		//D("%d %p", argc, argv);
		CT_MAIN c_main(argc, argv);

		//_DBG << "Start MAIN INIT";
		ec = c_main.init();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to initialize MASTER");
			return -1;
		}

#ifdef FF_GPROF
		ProfilerFlush();
#endif
		//_DBG << "Start MAIN RUN";
		ec = c_main.run();
		if (ec != EC_SUCCESS) {
			CRIT("Unable to run MASTER");
			return -1;
		}
	}

	{
		extern uint32_t gi_port_node_cnt;
		D("allocated CT_PORT_NODE: %d", gi_port_node_cnt);
	}
#if 0
} catch (std::exception const & e) {
	_CRIT << "exception propagated into main: " << e.what() << "\n";
} catch (...) {
	_CRIT << "unidentified thing caught in main.\n";
}
#endif

	gc_memtrack.f_end();
	return (0);
}
