/* Android AMTL
 *
 * Copyright (C) Intel 2013
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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 * Author: Erwan Bracq <erwan.bracq@intel.com>
 */
package com.intel.amtl.platform;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.SystemProperties;
import android.util.Log;

import com.intel.amtl.log.AlogMarker;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileInputStream;

public class Platform {

    private final String TAG = "AMTL";
    private String catalogPath = "/system/vendor/etc/telephony/";

    public String getPlatformConf() {
        AlogMarker.tAB("Platform.getPlatformConf", "0");
        int inst_id = 1;
        String key = "service.amtl" + inst_id + ".cfg";
        String nameExtension = "";
        String multiSim = SystemProperties.get("persist.radio.multisim.config", "");
        String filename = SystemProperties.get(key, "");

        if (filename.equals("")) {
            AlogMarker.tAE("Platform.getPlatformConf", "0");
            return catalogPath + "AMTL.conf";
        }

        if (multiSim.equals("dsda")) {
            nameExtension = "_DSDA";
        }
        AlogMarker.tAE("Platform.getPlatformConf", "0");
        return catalogPath + "amtl_" + filename + nameExtension + ".cfg";
    }
}
