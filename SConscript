from glob import glob
from building import *
import rtconfig

# get current directory
cwd     = GetCurrentDir()

LIBS    = []
LIBPATH = []
src = []

if rtconfig.CROSS_TOOL == 'gcc':
		LIBPATH = [cwd + '/microros/build/']

LIBS += ['microros' ]

# The set of source files associated with this SConscript file.
src	= Glob('transports/*.c')

path	= [cwd]
path	+= [cwd + '/microros/build/include']
path	+= [cwd + '/transports']

if GetDepend('MICROS_EXAMPLE_PUB_INT32'):
    src    += Glob('examples/micro_ros_pub_int32.c')

if GetDepend('MICROS_EXAMPLE_SUB_INT32'):
    src    += Glob('examples/micro_ros_sub_int32.c')

if GetDepend('MICROS_EXAMPLE_PUB_SUB_INT32'):
    src    += Glob('examples/micro_ros_pub_sub_int32.c')

if GetDepend('MICROS_EXAMPLE_PING_PONG'):
    src    += Glob('examples/micro_ros_ping_pong.c')

if GetDepend('MICROS_EXAMPLE_ADDTWOINTS_SERVER'):
    src    += Glob('examples/micro_ros_addtwoints_server.c')
    
if GetDepend('MICROS_EXAMPLE_SUB_TWIST'):
    src    += Glob('examples/micro_ros_sub_twist.c')

LOCAL_CCFLAGS = ''
group = DefineGroup('microros', src, depend = ['USING_MICROROS'], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS, LIBS = LIBS, LIBPATH = LIBPATH)

Return('group')
