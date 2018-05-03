#!/system/bin/sh
#
#
# Copyright (C) Intel 2014
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

prop_list=( \
    persist.service.aplogfs.enable \
    persist.service.apklogfs.enable \
)
svc_list=( \
    ap_logfs \
    apk_logfs \
)

# Default property to use if the user didn't set any of the persist properties
default=$(getprop ro.service.default_logfs)

for i in ${!prop_list[@]}; do
    value=$(getprop ${prop_list[$i]})
    if [ -n "$value" ]; then
        # A user modified the configuration, do not use default
        default=
    fi

    if [ "$value" == "1" ]; then
        # Get the service to start
        svc=${svc_list[$i]}
        break
    fi
done

if [ -n "$default" ]; then
    # Set the persist property that will restart this service
    setprop persist.service.$default.enable 1
    exit 0
fi

if [ -n "$svc" ]; then
    start $svc
fi
