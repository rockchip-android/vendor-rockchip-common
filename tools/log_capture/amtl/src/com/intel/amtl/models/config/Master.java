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
 * Author: Erwan Bracq <erwan.bracq@intel.com>
 * Author: Morgane Butscher <morganex.butscher@intel.com>
 */

package com.intel.amtl.models.config;

import android.util.Log;

import com.intel.amtl.log.AlogMarker;

public class Master {

    private final String TAG = "AMTL";
    private final String MODULE = "Master";
    private String name = "";
    private String defaultPort = "";
    private String defaultConf = "";

    public Master() {
    }

    public Master(String name, String defaultPort, String defaultConf) {
        AlogMarker.tAB("Master.Master", "0");
        this.setName(name);
        this.setDefaultPort(defaultPort);
        this.setDefaultConf(defaultConf);
        AlogMarker.tAE("Master.Master", "0");
    }

    public void setName(String name) {
        AlogMarker.tAB("Master.setName", "0");
        this.name = name;
        AlogMarker.tAE("Master.setName", "0");
    }

    public void setDefaultPort(String defaultPort) {
        AlogMarker.tAB("Master.setDefaultPort", "0");
        this.defaultPort = defaultPort;
        AlogMarker.tAE("Master.setDefaultPort", "0");
    }

    public void setDefaultConf(String defaultConf) {
        AlogMarker.tAB("Master.setDefaultConf", "0");
        this.defaultConf = defaultConf;
        AlogMarker.tAE("Master.setDefaultConf", "0");
    }

    public String getName() {
        AlogMarker.tAB("Master.getName", "0");
        AlogMarker.tAE("Master.getName", "0");
        return this.name;
    }

    public String getDefaultPort() {
        AlogMarker.tAB("Master.getDefaultPort", "0");
        AlogMarker.tAE("Master.getDefaultPort", "0");
        return this.defaultPort;
    }

    public String getDefaultConf() {
        AlogMarker.tAB("Master.getDefaultConf", "0");
        AlogMarker.tAE("Master.getDefaultConf", "0");
        return this.defaultConf;
    }

    public void printToLog() {
        AlogMarker.tAB("Master.printToLog", "0");
        Log.d(TAG, MODULE + ": name = " + name + ", defaultPort = " + defaultPort
                + ", defaultConf = " + defaultConf + ".");
        AlogMarker.tAE("Master.printToLog", "0");
    }
}
