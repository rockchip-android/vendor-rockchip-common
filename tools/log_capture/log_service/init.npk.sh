#!/system/bin/sh
#
#
# Copyright (C) Intel 2015
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# definitions ##################################################################
# ----- masters -----
M_TSCU=0

# -- fw --
M_CSE=16
M_CSE_V=17
M_CSE_VV=18
M_PMC=23
M_IUNIT=27
M_PUNIT=29

M_FW=($M_CSE $M_CSE_V $M_CSE_VV $M_PMC $M_IUNIT $M_PUNIT)

# -- sw --
M_SW=256+

# ----- outputs -----
PORT_MEM0=0
PORT_PTI=2

OUT_DEV=(0-msc0 0-msc1 0-pti)

# globals ######################################################################
_enabled_masters=($M_TSCU)

# functions ####################################################################

# Get the configuration to apply
# By order of precedence, this is
#     property from the command line
#     persistant property set by the user
get_config()
{
    cfg=
    cmdline_cfg=$(getprop ro.boot.npk.cfg)
    user_cfg=$(getprop persist.npk.cfg)

    if [ -n "$cmdline_cfg" ]; then
        cfg=$cmdline_cfg
    elif [ -n "$user_cfg" ]; then
        cfg=$user_cfg
    fi

    echo $cfg
}

# Get output number for the given configuration
# $1: output configuration
output_config_to_number()
{
    case $1 in
        pti)
            echo $PORT_PTI
            ;;
        pstore|dvc)
            echo $PORT_MEM0
            ;;
    esac
}

# Disable all outputs
reset_outputs()
{
    for i in ${OUT_DEV[@]}; do
        echo 0 > /sys/bus/intel_th/devices/$i/active
    done
}

# Set PTI mode
set_output_pti()
{
    # set sync packets frequency
    echo 40 > /sys/bus/intel_th/devices/0-gth/outputs/2_smcfreq
    # default to 4-bit, most widely supported (Fido)
    echo 4 > /sys/bus/intel_th/devices/0-pti/mode
    # enable null packets
    echo 1 > /sys/bus/intel_th/devices/0-gth/outputs/2_null
}

# Allocate multi windows for pstore
set_output_pstore()
{
    # set sync packets frequency
    echo 100 > /sys/bus/intel_th/devices/0-gth/outputs/0_smcfreq
    # configure msc0 in multiwindow mode
    echo multi > /sys/bus/intel_th/devices/0-msc0/mode
    echo 32,32,32 > /sys/bus/intel_th/devices/0-msc0/nr_pages
}

# Allocate two windows for dvc no wrap
set_output_dvc()
{
    # set sync packets frequency
    echo 100 > /sys/bus/intel_th/devices/0-gth/outputs/0_smcfreq
    # configure msc0 in multiwindow mode / wrap enable
    echo multi > /sys/bus/intel_th/devices/0-msc0/mode
    echo 48,48 > /sys/bus/intel_th/devices/0-msc0/nr_pages
}

# Enable master
# $1: master number
# $2: output number for this master
enable_master()
{
    echo $2 > /sys/bus/intel_th/devices/0-gth/masters/$1
}

# Disable master
# $1: master number
disable_master()
{
    echo -1 > /sys/bus/intel_th/devices/0-gth/masters/$1
}

# Check if a value is in the given array
# $1: value
# $2: all array elements
value_in_array()
{
    val=$1
    shift
    for e in "$@"; do
        if [ "$e" == "$val" ]; then
            return 0
        fi
    done
    return 1
}

# Enable all selected masters for the given output configuration.
# Disable all other masters
# $1: output number
set_masters()
{
    for m in $(cd /sys/bus/intel_th/devices/0-gth/masters && ls *); do
        if value_in_array $m "${_enabled_masters[@]}"; then
            enable_master $m $1
        else
            disable_master $m
        fi
    done
}

start_logs_kernel()
{
    # link the sth device to the console stm source
    echo 0-sth > /sys/class/stm_source/console/stm_source_link
}

stop_logs_kernel()
{
    echo none > /sys/class/stm_source/console/stm_source_link
}

start_logs_android()
{
    start ap_lognpk
}

stop_logs_android()
{
    stop ap_lognpk
}

start_services()
{
    stop_logs_kernel
    if [ -n "$1" ]; then
        start_logs_kernel
    fi

    stop_logs_android
    if [ -n "$2" ]; then
        start_logs_android
    fi
}

# Setup output port and masters for the given configuration
# $1: output configuration
setup_tracehub()
{
    output_nb=$(output_config_to_number $1)

    set_masters $output_nb

    set_output_$output

    # Enable selected port
    echo 1 > /sys/bus/intel_th/devices/${OUT_DEV[$output_nb]}/active
}

# main #########################################################################

cfg=$(get_config)

# cfg=pti
# cfg=pti,kernel
# cfg=pti,kernel,fw,android
# cfg=pti,sw,fw - Note: sw=kernel,android
# ...
# parse the list of keywords
for c in ${cfg//,/ }; do
    case $c in
        pti|pstore|dvc)
            output=$c
            ;;
        kernel)
            sw_master=true
            kernel=true
            ;;
        android)
            sw_master=true
            android=true
            ;;
        fw)
            fw=true
            ;;
        sw)
            sw_master=true
            android=true
            kernel=true
            ;;
    esac
done

if [ -z "$output" ]; then
    echo "No output selected in config [$cfg] - disable"
    reset_outputs
    start_services "" ""
    exit 0
fi

if [ -n "$fw" ]; then
    _enabled_masters+=("${M_FW[@]}")
fi
if [ -n "$sw_master" ]; then
    _enabled_masters+=($M_SW)
fi

reset_outputs
setup_tracehub $output
start_services "$kernel" "$android"
