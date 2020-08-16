#!/bin/bash

#I don't like to set environment variables in the system, 
#so I put the environment variables in run.sh.
#Every time I use run.sh, the enviroment variables will be set, after use that will be unsetted.
PROJECT_ROOT=../..
TOOLS_PATH=$PROJECT_ROOT/tool
SDK_PATH=$PROJECT_ROOT/sdk
APP_PATH=$PROJECT_ROOT/app

XTENSA_LX106_ELF_PATH=$TOOLS_PATH/xtensa-lx106-elf
ESP_IDF_PATH=$SDK_PATH/ESP8266_RTOS_SDK

the_sdk_path=`cd $ESP_IDF_PATH; pwd`
the_tool_chain_path=`cd $XTENSA_LX106_ELF_PATH/bin; pwd`

export PATH="$PATH:$the_tool_chain_path"
export IDF_PATH="$the_sdk_path"

#GIT_ESP_ALIYUN_LINK=git@github.com:oldprogram/esp-aliyun.git
GIT_ESP_ALIYUN_LINK=https://github.com/espressif/esp-aliyun.git

#--------------------------------------------------------------------------
function install_project_from_github(){
    echo "> install project form github ..."
    if [ ! -d "./esp-aliyun/" ]; then
        git clone --recursive $GIT_ESP_ALIYUN_LINK
        
        # remove the mqtt_example.c
        # move cp mqtt_example.c replace pre-one
        rm -rf ./esp-aliyun/examples/mqtt/mqtt_example/main/mqtt_example.c 
        cp ./my_src/mqtt_example.c ./esp-aliyun/examples/mqtt/mqtt_example/main/ 
    fi
    
    cd $ESP_IDF_PATH
    git checkout release/v3.1
}

function sdk_op(){
    echo ">> APP_OP   "$1

    cd ./esp-aliyun/iotkit-embedded/
    if [ "$1" == "reconfig" ]; then
        make reconfig
    elif [ "$1" == "config" ]; then
        make menuconfig
    elif [ "$1" == "make" ]; then
        make 
    elif [ "$1" == "clean" ]; then
        make distclean
    else
        echo "error, try bash run.sh help"   
    fi
    cd -
}

function app_op(){
    echo ">> SDK_OP   "$1

    cd ./esp-aliyun/examples/mqtt/mqtt_example/
    if [ "$1" == "defconfig" ]; then
        make defconfig
    elif [ "$1" == "config" ]; then
        make menuconfig
    elif [ "$1" == "make" ]; then
        make
    elif [ "$1" == "erase" ]; then
        make erase
    elif [ "$1" == "flash" ]; then
        make print_flash_cmd
        make flash
    elif [ "$1" == "monitor" ]; then
        make monitor
    elif [ "$1" == "clean" ]; then
        make clean
    else
        echo "error, try bash run.sh help"
    fi
    cd -
}

if [ "$1" == "create" ]; then
    install_project_from_github
elif [ "$1" == "sdk" ]; then
    sdk_op $2
elif [ "$1" == "app" ];then
    app_op $2
elif [ "$1" == "help" ];then
    echo "|----------------------------------------------------"
    echo "| ./run.sh op param"
    echo "| op:"
    echo "|   create : downloads iotkit and aliyun-esp from github and make some change"
    echo "|   sdk : param = reconfig/config/make/clean"
    echo "|   app : param = deconfig/config/make/erase/flash/monitor/clean"
    echo "| examples:"
    echo "|   first create sdk lib : create -> sdk reconfig -> sdk config -> sdk make"
    echo "|   second create app : config -> make -> flash -> monitor"
    echo "|----------------------------------------------------"
else
    echo "error, try bash run.sh help"   
fi

