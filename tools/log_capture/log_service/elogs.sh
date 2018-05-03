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

if [ -d /cache/elogs ] ; then
    if [ -e /cache/elogs/elog ] ; then
        mv /cache/elogs/elog /cache/elogs/elog-old
    else
        touch /cache/elogs/elog
        restorecon /cache/elogs/elog
    fi

    COUNTER=0
    #we use a retry mechanism as logd might not be started yet
    while [  $COUNTER -lt 50 ]; do

        #at this stage, the target file should be either missing or empty. Otherwise, we assume
        #that logcatext has crashed, with logd running, and we chose not to restart it.
        if [ -s "/cache/elogs/elog" ]; then
            exit
        else
            /system/vendor/bin/logcat_ep.sh /system/vendor/bin/logcatext -b all -v threadtime \
            -f /cache/elogs/elog
        fi

        let COUNTER=COUNTER+1
        #sleep 100 ms, while waiting for logd to start
        usleep 100000
    done
fi
