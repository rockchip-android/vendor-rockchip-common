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

# This script is meant to serve as a domain entrypoint for ap[k]_logfs
# services

function start_auto {
    logger=/system/vendor/bin/logcatext
    rot_size=5000
    rot_count=0

    tmp=$(getprop ro.intel.logger)

    if [ $tmp ]; then
        logger=$tmp
    fi

    tmp=${logger##*/}
    case "$tmp" in
        logcat)
        ;;

        logcatext)
        ;;

        *)
            echo "The logger can only be logcat[ext] ($logger)"
            exit 1
        ;;
    esac

    if [ ! -e "$logger" ] ; then
        echo "Cannot find $logger"
        exit 1
    fi

    tmp=$(getprop persist.intel.logger.rot_size)
    if [ $tmp ] && [ $tmp -gt "0" ]; then
        rot_size=$tmp
    fi

    tmp=$(getprop persist.intel.logger.rot_cnt)
    if [ $tmp ] && [ $tmp -gt "0" ]; then
        rot_count=$tmp
    fi

    $logger -n $rot_count -r $rot_size $@
}


case "$1" in
    *logcat)
       "$@"
    ;;

    *logcatext)
        "$@"
    ;;

    auto)
        shift
        start_auto $@
    ;;

    *)
        echo "$0 should only be used for logcat[ext]"
    ;;
esac

