#!/usr/bin/python
import utils
import os

#Reset umask
os.umask(0022)

bs = utils.BuidSystem(ARGUMENTS)
Export('bs')


## Configure function
def ConfigureInit(bs):
  # Start of configuration
  bs.ConfigureStart()
  bs.ConfigureAddLib(name = 'boost', flags = '-lboost_thread -lboost_system -lboost_filesystem -lboost_regex -lrt')
  bs.ConfigureAddLib(name = 'rt', flags = '-lpthread -lrt')
  bs.ConfigureAddLib(name = 'dl', flags = '-ldl')
  bs.ConfigureAddLib(name = 'bfd', flags = '-lbfd')
  bs.ConfigureAddLib(name = 'openmp', flags = '-fopenmp')
  bs.ConfigureAddLib(name = 'python', tool = 'pkg-config --cflags --libs python3')
  bs.ConfigureAddLib(name = 'eigen', tool = 'pkg-config --cflags --libs eigen3')
  # Execute SConscript while bs.init = True
  for submodule_path in os.listdir('src/plugins/'):
    scons_path = os.path.join('src/plugins/', submodule_path, 'SConstruct.sub')
    if os.path.exists(scons_path):
        SConscript(scons_path)
  # End of configuration
  bs.ConfigureEnd()
  bs.env_run['ENV']['MASTER_BS_PATH'] = os.getcwd()


## CONFIGURE PART
if ARGUMENTS.get('config', None) :
  ConfigureInit(bs)
  print "\nConfiguration is now set to: /!\ "+bs['config'] + " /!\ \n";
  if len(COMMAND_LINE_TARGETS) == 0:
    Exit(0)

## INCLUDE PART
bs.ConfigureRead()

# Empty configuration
print ARGUMENTS.get('config', None)
if (not ARGUMENTS.get('config', None)) and ('config' not in bs):
  print "\nConfiguration must be set !";
  Exit(0)
################################################################################
#### LIBRARIES
################################################################################

libs = {'shared':['boost','dl','rt','host-boost','core','bml-sab'], 'static':['node','common-c','common-cpp'], 'module':[]}

bs.env_run['MASTER_DEFINES'] = []
host_qt = False


if bs['config'] in ['mviz-bistatic', 'mviz-bisbfu']:
	libs['shared'].append('host-qt')
	libs['shared'].append('host-qt-nogui')
	libs['shared'].append('qt-propertybrowser')

libs['module'] = [
	'plugin-transport-msgqueue',
	'plugin-transport-function',
	'plugin-transport-tcp',
	'plugin-transport-udp'
	]

libs['shared'].append('bfd')
bs.env_run['MASTER_LIBS'] = libs
modules = []
bs.env_run['MASTER_MODULES'] = modules
depends = []
bs.env_run['MASTER_DEPENDS'] = depends
bs.env_run['ENV']['MASTER_DEPENDS'] = ""

################################################################################
#### SOURCE FOLDERS
################################################################################
# Variant dir
bs['buildDir'] = os.path.join('__out__',bs['remote'],bs['config'])


dirs = [
        'src/libs/bml',
        'src/libs/common',
        'src/core',
        'src/core/host/boost',
        'src/plugins/transport-common',
        ]

bs.env_run['MASTER_DIRS'] = dirs

################################################################################
#### PARSE SUBMODULE
################################################################################
for submodule_path in os.listdir('src/plugins/'):
    #print(submodule_path)
    scons_path = os.path.join('src/plugins/', submodule_path, 'SConstruct.sub')
    #print(scons_path)
    if os.path.exists(scons_path):
        print(scons_path)
        SConscript(scons_path)

# Master program
if 'MASTER_CONFIG' in bs.env_run['ENV']:
    dirs.append('src')
    print 'MASTER_CONFIG: '+bs.env_run['ENV']['MASTER_CONFIG']
else:
    print 'NO MASTER CONFIG!'
bs.AddSubDirs(bs.env_run['MASTER_DIRS'], bs['buildDir'])
