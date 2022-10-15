set -e
set -o nounset
set -o pipefail
# get progect directory
SOURCE_DIR=$(readlink -f ../..)
MICROROS_DIR=$(basename "$PWD")

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
        sed -i "\$a WORKING_DIRECTORY \"\${CMAKE_CURRENT_SOURCE_DIR}/packages/${MICROROS_DIR}/microros\" " CMakeLists.txt
        sed -i '$a COMMAND sh generate_microros_library.sh ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} ${CMAKE_CURRENT_SOURCE_DIR} ' CMakeLists.txt
        sed -i '$a COMMENT "build micro-ROS..." '                   CMakeLists.txt
        sed -i '$a )'                                               CMakeLists.txt
    fi

popd > /dev/null


# build microROS
pushd ${SOURCE_DIR}/build > /dev/null
    echo $PWD
    rm -rf ./*
    cmake ..
    make build_microros
popd  > /dev/null

# add packages/microros/Kconfig for Kconfig file
pushd ${SOURCE_DIR} > /dev/null
	if [ "$(grep "$MICROROS_DIR" Kconfig)" ];then
		echo "Not Add source \"packages/$MICROROS_DIR/Kconfig\""
	else
		sed -i "\$a source \"packages/${MICROROS_DIR}/Kconfig\"" Kconfig
		echo "Add source \"Kconfig/$MICROROS_DIR/Kconfig\""
	fi
popd > /dev/null
