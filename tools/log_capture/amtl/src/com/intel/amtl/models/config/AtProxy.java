/* Android AMTL
 *
 * Copyright (C) Intel 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Morgane Butscher <morganex.butscher@intel.com>
 */

package com.intel.amtl.models.config;

import android.os.SystemProperties;

import com.intel.amtl.log.AlogMarker;

public class AtProxy {

    private final String TAG = "AMTL";
    private final String MODULE = "AtProxy";
    private static final String AT_PROXY_PROP = "persist.system.at-proxy.mode";
    private static final String USB_CONFIG_PROP = "persist.sys.usb.config";

    public AtProxy() {
        AlogMarker.tAB("AtProxy.AtProxy", "0");
        AlogMarker.tAE("AtProxy.AtProxy", "0");
    }

    public static String getAtProxyMode() {
        AlogMarker.tAB("AtProxy.getProxyMode", "0");
        AlogMarker.tAE("AtProxy.getProxyMode", "0");
        return SystemProperties.get(AT_PROXY_PROP, "0");
    }

    public static  void setAtProxyMode(String mode) {
        AlogMarker.tAB("AtProxy.setProxyMode", "0");
        SystemProperties.set(AT_PROXY_PROP, mode);
        AlogMarker.tAE("AtProxy.setProxyMode", "0");
    }

    public static String getUsbConfigValue() {
        AlogMarker.tAB("AtProxy.getProxyMode", "0");
        AlogMarker.tAE("AtProxy.getProxyMode", "0");
        return SystemProperties.get(USB_CONFIG_PROP, "");
    }

    public static void setUsbConfigValue(String value) {
        AlogMarker.tAB("AtProxy.setProxyMode", "0");
        SystemProperties.set(USB_CONFIG_PROP, value);
        AlogMarker.tAE("AtProxy.setProxyMode", "0");
    }
}
