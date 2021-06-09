from building import *
import rtconfig

# get current directory
cwd     = GetCurrentDir()

LIBS    = []
LIBPATH = []
src = []

if rtconfig.CROSS_TOOL == 'gcc':
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M0_plus'):
		LIBPATH = [cwd + '/src/cortex-m0plus/']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M3'):
		LIBPATH = [cwd + '/src/cortex-m3/']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_D16_SOFT'):
		LIBPATH = [cwd + '/src/cortex-m7/fpv5-d16-softfp']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_SP_D16_SOFT'):
		LIBPATH = [cwd + '/src/cortex-m7/fpv5-sp-d16-softfp']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_D16_HARD'):
		LIBPATH = [cwd + '/src/imxrt1062/fpv5-d16-hard']

LIBS += ['microros']

# The set of source files associated with this SConscript file.
src     = Glob('src/*.c')
src     += Glob('examples/*.c')

path   = [cwd]
path   += [cwd + '/src']

if GetDepend('MICRO_ROS_USING_PUB_INT32'):
    src    += Glob('examples/micro_ros_pub_int32.c')

LOCAL_CCFLAGS = ''

group = DefineGroup('microros', src, depend = [''], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS, LIBS = LIBS, LIBPATH = LIBPATH)

Return('group')
