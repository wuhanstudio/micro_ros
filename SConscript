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
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M4_FPV4_SP_D16_SOFT'):
		LIBPATH = [cwd + '/src/cortex-m4/fpv4-sp-d16-softfp']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M4_FPV4_SP_D16_HARD'):
		LIBPATH = [cwd + '/src/cortex-m4/fpv4-sp-d16-hard']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_D16_SOFT'):
		LIBPATH = [cwd + '/src/cortex-m7/fpv5-d16-softfp']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_SP_D16_SOFT'):
		LIBPATH = [cwd + '/src/cortex-m7/fpv5-sp-d16-softfp']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_D16_HARD'):
		LIBPATH = [cwd + '/src/cortex-m7/fpv5-d16-hard']
	if GetDepend('MICRO_ROS_USING_ARCH_CORTEX_M7_FPV5_SP_D16_HARD'):
		LIBPATH = [cwd + '/src/cortex-m7/fpv5-sp-d16-hard']

LIBS += ['microros']

# The set of source files associated with this SConscript file.
src	= Glob('src/*.c')

path	= [cwd]
path	+= [cwd + '/src']

if GetDepend('MICRO_ROS_USING_PUB_INT32'):
    src    += Glob('examples/micro_ros_pub_int32.c')

if GetDepend('MICRO_ROS_USING_SUB_INT32'):
    src    += Glob('examples/micro_ros_sub_int32.c')

if GetDepend('MICRO_ROS_USING_SUB_TWIST'):
    src    += Glob('examples/micro_ros_sub_twist.c')

if GetDepend('MICRO_ROS_USING_ADD_INTS_SERVICE'):
    src    += Glob('examples/micro_ros_add_ints_service.c')

if GetDepend('MICRO_ROS_USING_DEINITIALIZATION'):
    src    += Glob('examples/micro_ros_deinitialization.c')

if GetDepend('MICRO_ROS_USING_KOBUKI_CONTROL'):
    src    += Glob('examples/micro_ros_kobuki_control.c')

if GetDepend('MICRO_ROS_USING_PUB_SUB'):
    src    += Glob('examples/micro_ros_pub_sub_int32.c')

if GetDepend('MICRO_ROS_USING_PING_PONG'):
    src    += Glob('examples/micro_ros_ping_pong.c')

LOCAL_CCFLAGS = ''

group = DefineGroup('microros', src, depend = ['PKG_USING_MICRO_ROS'], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS, LIBS = LIBS, LIBPATH = LIBPATH)

Return('group')
