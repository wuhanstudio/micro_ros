#!/bin/bash
set -e
set -o nounset
set -o pipefail

# Check if colcon is available
if ! command -v colcon &> /dev/null
then
    echo "colcon could not be found"
    exit 1
fi

function clean {
    echo $(echo $(echo $1 | sed 's/:/\n/g' | \
      grep -v -E "($(echo $AMENT_PREFIX_PATH | sed 's/:/\|/g'))" ) | sed 's/ /:/g' )
}

COMPONENT_DIR=$(pwd)
INSTALL_DIR=$(pwd)/install

X_CC=$1
X_CXX=$2
CFLAGS_INTERNAL=$3
CXXFLAGS_INTERNAL=$4
SOURCE_DIR=$5


FIRMARE_DIR=$(pwd)
BUILD_DIR=$(pwd)/build

echo ${FIRMARE_DIR}
echo ${BUILD_DIR}

# amnemt
mkdir -p ${FIRMARE_DIR}/micro_ros_dev
pushd ${FIRMARE_DIR}/micro_ros_dev > /dev/null
	# rm -rf build install log toolchain.cmake

    # git clone -b humble https://github.com/ament/ament_cmake src/ament_cmake
    # git clone -b humble https://github.com/ament/ament_lint src/ament_lint
    # git clone -b humble https://github.com/ament/ament_package src/ament_package
	# git clone -b humble https://github.com/ament/googletest src/googletest
	# git clone -b humble https://github.com/ros2/ament_cmake_ros src/ament_cmake_ros
	# git clone -b humble https://github.com/ament/ament_index src/ament_index

    # colcon build --cmake-args -DBUILD_TESTING=OFF
popd > /dev/null

# lcean env
export PATH=$(clean $PATH)
unset AMENT_PREFIX_PATH;
unset RMW_IMPLEMENTATION;

set +o nounset
. $FIRMARE_DIR/micro_ros_dev/install/local_setup.sh;
set -o nounset

# toolchain.cmake

ESCAPED_CFLAGS_INTERNAL=$(echo $CFLAGS_INTERNAL | sed 's/-Dgcc//g' | sed 's/-gdwarf-2//g' | sed 's/-g//g')
ESCAPED_CXXFLAGS_INTERNAL=$(echo $CXXFLAGS_INTERNAL | sed 's/-Dgcc//g' | sed 's/-gdwarf-2//g' | sed 's/-g//g')
ESCAPED_X_CC=$(echo $X_CC | sed 's/\//\\\//g' | sed 's/"//g')
ESCAPED_X_CXX=$(echo $X_CXX | sed 's/\//\\\//g' | sed 's/"//g')

echo "Using X_CC: $X_CC"
echo "Using X_CXX: $X_CXX"

echo ${ESCAPED_CFLAGS_INTERNAL}
echo ${ESCAPED_CXXFLAGS_INTERNAL}


cat $FIRMARE_DIR/toolchain.cmake.in | \
    sed "s/@CMAKE_C_COMPILER@/$ESCAPED_X_CC/g" | \
    sed "s/@CMAKE_CXX_COMPILER@/$ESCAPED_X_CXX/g" | \
    sed "s/@CFLAGS@/$ESCAPED_CFLAGS_INTERNAL/g" | \
    sed "s/@CXXFLAGS@/$ESCAPED_CXXFLAGS_INTERNAL/g" \
    > $FIRMARE_DIR/toolchain.cmake


mkdir -p ${FIRMARE_DIR}/micro_ros_src
# buld micro-ROS
pushd ${FIRMARE_DIR}/micro_ros_src > /dev/null

    # rm -rf build install log 

    # git clone -b ros2 https://github.com/eProsima/micro-CDR src/micro-CDR;
	# git clone -b ros2 https://github.com/eProsima/Micro-XRCE-DDS-Client src/Micro-XRCE-DDS-Client;
	# git clone -b humble https://github.com/micro-ROS/rcl src/rcl;
	# git clone -b humble https://github.com/ros2/rclc src/rclc;
	# git clone -b humble https://github.com/micro-ROS/micro_ros_utilities src/micro_ros_utilities;
	# git clone -b humble https://github.com/micro-ROS/rcutils src/rcutils;
	# git clone -b humble https://github.com/micro-ROS/micro_ros_msgs src/micro_ros_msgs;
	# git clone -b humble https://github.com/micro-ROS/rmw-microxrcedds src/rmw-microxrcedds;
	# git clone -b humble https://github.com/micro-ROS/rosidl_typesupport src/rosidl_typesupport;
	# git clone -b humble https://github.com/micro-ROS/rosidl_typesupport_microxrcedds src/rosidl_typesupport_microxrcedds;
	# git clone -b humble https://gitlab.com/micro-ROS/ros_tracing/ros2_tracing.git/  src/ros2_tracing;

	# git clone -b humble https://github.com/ros2/common_interfaces src/common_interfaces;
	# git clone -b humble https://github.com/ros2/example_interfaces src/example_interfaces;
	# git clone -b humble https://github.com/ros2/libyaml_vendor.git src/libyaml_vendor;
	# git clone -b humble https://github.com/ros2/rcl_interfaces src/rcl_interfaces;
	# git clone -b humble https://github.com/ros2/rcl_logging src/rcl_logging;
	# git clone -b humble https://github.com/ros2/rmw src/rmw;
	# git clone -b humble https://github.com/ros2/rmw_implementation src/rmw_implementation;
	# git clone -b humble https://github.com/ros2/rosidl src/rosidl;
	# git clone -b humble https://github.com/ros2/rosidl_dds.git src/rosidl_dds;
	# git clone -b humble https://github.com/ros2/rosidl_defaults src/rosidl_defaults;
	# git clone -b humble https://github.com/ros2/test_interface_files src/test_interface_files;
	# git clone -b humble https://github.com/ros2/unique_identifier_msgs src/unique_identifier_msgs;
	
    
	# touch src/rcl_logging/rcl_logging_spdlog/COLCON_IGNORE;
   	# touch src/rosidl/rosidl_typesupport_introspection_cpp/COLCON_IGNORE;
	# touch src/rcl/rcl_yaml_param_parser/COLCON_IGNORE;	
	# touch src/rclc/rclc_examples/COLCON_IGNORE;
	
    # colcon build							\
	# 	--merge-install						\
	# 	--packages-ignore-regex=.*_cpp  	\
	# 	--metas ${FIRMARE_DIR}/colcon.meta 	\
	# 	--cmake-args 						\
	# 	"--no-warn-unused-cli" 				\
	# 	-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=OFF 	\
	# 	-DTHIRDPARTY=ON 					\
	# 	-DBUILD_SHARED_LIBS=OFF 			\
	# 	-DBUILD_TESTING=OFF					\
	# 	-DCMAKE_BUILD_TYPE=Release 			\
	# 	-DCMAKE_TOOLCHAIN_FILE=$FIRMARE_DIR/toolchain.cmake	 	\
	# 	-DCMAKE_VERBOSE_MAKEFILE=ON  		\

	

    # Create packaged library
    mkdir -p $FIRMARE_DIR/libmicroros; cd $FIRMARE_DIR/libmicroros;
    for file in $(find $FIRMARE_DIR/micro_ros_src/install/lib/ -name '*.a'); do \
		folder=$(echo $file | sed -E "s/(.+)\/(.+).a/\2/"); \
		mkdir -p $folder; cd $folder; ar x $file; \
		for f in *; do \
			mv $f ../$folder-$f; \
		done; \
		cd ..; rm -rf $folder; \
	done ; \
	ar rc libmicroros.a $(ls *.o *.obj 2> /dev/null); mkdir -p $BUILD_DIR; cp libmicroros.a $BUILD_DIR; \
    cp -R $FIRMARE_DIR/micro_ros_src/install/include $BUILD_DIR/; \
	cd ..; rm -rf libmicroros;

	######## Fix include paths  ########
	INCLUDE_ROS2_PACKAGES=$(cd ${FIRMARE_DIR}/micro_ros_src && colcon list | awk '{print $1}' | awk -v d=" " '{s=(NR==1?s:s d)$0}END{print s}')

	for var in ${INCLUDE_ROS2_PACKAGES}; do
		if [ -d "$BUILD_DIR/include/${var}/${var}" ]
		then
			rsync -r $BUILD_DIR/include/${var}/${var}/* $BUILD_DIR/include/${var}/ || true
			rm -rf $BUILD_DIR/include/${var}/${var}/
		fi
	done

popd > /dev/null







