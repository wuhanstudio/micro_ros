#!/bin/bash

PLATFORMS=()
while getopts "p:" o; do
    case "$o" in
        p)
            PLATFORMS+=(${OPTARG})
            ;;
    esac
done

if [ $OPTIND -eq 1 ]; then
    PLATFORMS+=("cortex_m0")
    PLATFORMS+=("cortex_m3")
    PLATFORMS+=("cortex_m4_sp_soft")
    PLATFORMS+=("cortex_m4_sp_hard")
    PLATFORMS+=("cortex_m7_soft")
    PLATFORMS+=("cortex_m7_hard")
    PLATFORMS+=("cortex_m7_sp_soft")
    PLATFORMS+=("cortex_m7_sp_hard")
    PLATFORMS+=("esp32")
fi

shift $((OPTIND-1))

######## Init ########

apt update

cd /uros_ws

source /opt/ros/$ROS_DISTRO/setup.bash
source install/local_setup.bash

ros2 run micro_ros_setup create_firmware_ws.sh generate_lib

export TOOLCHAIN_PREFIX=/toolchain/bin/arm-none-eabi-

######## Adding extra packages ########
pushd firmware/mcu_ws > /dev/null

    # Workaround: Copy just tf2_msgs
    git clone -b galactic https://github.com/ros2/geometry2
    cp -R geometry2/tf2_msgs ros2/tf2_msgs
    rm -rf geometry2

    # Import user defined packages
    mkdir extra_packages
    pushd extra_packages > /dev/null
        cp -R /project/extras/library_generation/extra_packages/* .
        vcs import --input extra_packages.repos
    popd > /dev/null

popd > /dev/null

# Workaround. Remove when https://github.com/ros2/rosidl/pull/596 is merged
touch firmware/mcu_ws/ros2/common_interfaces/actionlib_msgs/COLCON_IGNORE;
touch firmware/mcu_ws/ros2/common_interfaces/std_srvs/COLCON_IGNORE;
touch firmware/mcu_ws/ros2/example_interfaces/COLCON_IGNORE;

######## Clean and source ########
find /project/src/ ! -name micro_ros_arduino.h ! -name *.c ! -name *.cpp ! -name *.c.in -delete

######## Build for SAMD (e.g. Arduino Zero) ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m0 " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m0_toolchain.cmake /project/extras/library_generation/colcon_verylowmem.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m0plus
    cp -R firmware/build/libmicroros.a /project/src/cortex-m0plus/libmicroros.a
fi

######## Build for STM32F3 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m3 " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m3_toolchain.cmake /project/extras/library_generation/colcon_lowmem.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m3
    cp -R firmware/build/libmicroros.a /project/src/cortex-m3/libmicroros.a
fi

######## Build for STM32F4 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m4_sp_soft " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m4_sp_soft_toolchain.cmake /project/extras/library_generation/colcon_lowmem.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m4/fpv4-sp-d16-softfp
    cp -R firmware/build/libmicroros.a /project/src/cortex-m4/fpv4-sp-d16-softfp/libmicroros.a
fi

######## Build for STM32F4 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m4_sp_hard " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m4_sp_hard_toolchain.cmake /project/extras/library_generation/colcon_lowmem.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m4/fpv4-sp-d16-hard
    cp -R firmware/build/libmicroros.a /project/src/cortex-m4/fpv4-sp-d16-hard/libmicroros.a
fi

######## Build for STM32F7 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m7_soft " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m7_soft_toolchain.cmake /project/extras/library_generation/colcon.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m7/fpv5-d16-softfp
    cp -R firmware/build/libmicroros.a /project/src/cortex-m7/fpv5-d16-softfp/libmicroros.a
fi

######## Build for STM32F7 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m7_hard " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m7_hard_toolchain.cmake /project/extras/library_generation/colcon.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m7/fpv5-d16-hard
    cp -R firmware/build/libmicroros.a /project/src/cortex-m7/fpv5-d16-hard/libmicroros.a
fi

######## Build for STM32F7 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m7_sp_soft " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m7_sp_soft_toolchain.cmake /project/extras/library_generation/colcon.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m7/fpv5-sp-d16-softfp
    cp -R firmware/build/libmicroros.a /project/src/cortex-m7/fpv5-sp-d16-softfp/libmicroros.a
fi

######## Build for STM32F7 ########
if [[ " ${PLATFORMS[@]} " =~ " cortex_m7_sp_hard " ]]; then
    rm -rf firmware/build

    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/cortex_m7_sp_hard_toolchain.cmake /project/extras/library_generation/colcon.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/cortex-m7/fpv5-sp-d16-hard
    cp -R firmware/build/libmicroros.a /project/src/cortex-m7/fpv5-sp-d16-hard/libmicroros.a
fi


######## Build for ESP 32  ########
if [[ " ${PLATFORMS[@]} " =~ " esp32 " ]]; then
    rm -rf firmware/build

    export TOOLCHAIN_PREFIX=/uros_ws/xtensa-esp32-elf/bin/xtensa-esp32-elf-
    ros2 run micro_ros_setup build_firmware.sh /project/extras/library_generation/esp32_toolchain.cmake /project/extras/library_generation/colcon.meta

    find firmware/build/include/ -name "*.c"  -delete
    cp -R firmware/build/include/* /project/src/

    mkdir -p /project/src/esp32
    cp -R firmware/build/libmicroros.a /project/src/esp32/libmicroros.a
fi

######## Generate extra files ########
find firmware/mcu_ws/ros2 \( -name "*.srv" -o -name "*.msg" -o -name "*.action" \) | awk -F"/" '{print $(NF-2)"/"$NF}' > /project/available_ros2_types
find firmware/mcu_ws/extra_packages \( -name "*.srv" -o -name "*.msg" -o -name "*.action" \) | awk -F"/" '{print $(NF-2)"/"$NF}' >> /project/available_ros2_types
# sort it so that the result order is reproducible
sort -o /project/available_ros2_types /project/available_ros2_types

cd firmware
echo "" > /project/built_packages
for f in $(find $(pwd) -name .git -type d); do pushd $f > /dev/null; echo $(git config --get remote.origin.url) $(git rev-parse HEAD) >> /project/built_packages; popd > /dev/null; done;
# sort it so that the result order is reproducible
sort -o /project/built_packages /project/built_packages

######## Fix permissions ########
sudo chmod -R 777 .
