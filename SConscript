from building import *
import rtconfig

# get current directory
cwd     = GetCurrentDir()

LIBS    = []
LIBPATH = []
src = []

if rtconfig.CROSS_TOOL == 'gcc':
	LIBPATH = [cwd + '/src/cortex-m3/']
LIBS += ['microros']

# The set of source files associated with this SConscript file.
src     = Glob('src/*.c')
src     += Glob('examples/*.c')

path   = [cwd]
path   += [cwd + '/src']

if GetDepend('ROSSERIAL_USING_SERVICE_CLIENT_UART'):
    src    += Glob('examples/service_client.cpp')

LOCAL_CCFLAGS = ''

group = DefineGroup('microros', src, depend = [''], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS, LIBS = LIBS, LIBPATH = LIBPATH)

Return('group')
