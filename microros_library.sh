set -e
set -o nounset
set -o pipefail
# get progect directory
SOURCE_DIR=$(readlink -f ../..)
# add build_microros command for CMakeLists.txt
pushd ${SOURCE_DIR} > /dev/null
    if [ ! -e "CMakeLists.txt" ];then 
        scons --target=cmake
    fi
    if [ "$(grep build_microros CMakeLists.txt)" ];then
        echo " "
    else
        echo "add build_microros command for CMakeLists.txt"
        sed -i '$a add_custom_target(build_microros '               CMakeLists.txt
        sed -i '$a WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/micro-ROS-rtthread-app/microros" ' CMakeLists.txt
        sed -i '$a COMMAND sh generate_microros_library.sh ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} ${CMAKE_CURRENT_SOURCE_DIR} ' CMakeLists.txt
        sed -i '$a COMMENT "build micro-ROS..." '                   CMakeLists.txt
        sed -i '$a )'                                               CMakeLists.txt
    fi

popd > /dev/null

# build microROS
pushd ${SOURCE_DIR}/build > /dev/null
    rm -rf ./*
    cmake ..
    make build_microros
popd  > /dev/null

# add packages/micro-ROS-rtthread-app/Kconfig for Kconfig file
pushd ${SOURCE_DIR} > /dev/null
	if [ "$(grep micro-ROS-rtthread-app Kconfig)" ];then
		echo "Not Add source \"packages/micro-ROS-rtthread-app/Kconfig\""
	else
		sed -i '$a source "packages/micro-ROS-rtthread-app/Kconfig" ' Kconfig
		echo "Add source \"packages/micro-ROS-rtthread-app/Kconfig\""
	fi
popd > /dev/null
