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

import android.util.Log;

import com.intel.amtl.log.AlogMarker;

public class Alias {

    private final String TAG = "AMTL";
    private final String MODULE = "Alias";
    private String profileName = "";
    private String destination = "";

    public Alias(String profileName, String destination) {
        AlogMarker.tAB("Alias.Alias", "0");
        this.setProfileName(profileName);
        this.setDestination(destination);
        AlogMarker.tAE("Alias.Alias", "0");
    }

    public void setProfileName(String profileName) {
        AlogMarker.tAB("Alias.setProfileName", "0");
        this.profileName = profileName;
        AlogMarker.tAE("Alias.setProfileName", "0");
    }

    public void setDestination(String destination) {
        AlogMarker.tAB("Alias.setDestination", "0");
        this.destination = destination;
        AlogMarker.tAE("Alias.setDestination", "0");
    }

    public String getProfileName() {
        AlogMarker.tAB("Alias.getProfileName", "0");
        AlogMarker.tAE("Alias.getProfileName", "0");
        return this.profileName;
    }

    public String getDestination() {
        AlogMarker.tAB("Alias.getDestination", "0");
        AlogMarker.tAE("Alias.getDestination", "0");
        return this.destination;
    }

    public void printToLog() {
        AlogMarker.tAB("Alias.printToLog", "0");
        Log.d(TAG, MODULE + ": profileName = " + profileName + ", destination = " + destination
                + ".");
        AlogMarker.tAE("Alias.printToLog", "0");
    }
}
