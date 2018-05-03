/* Android Modem Traces and Logs
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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.modem.command;

import android.util.Log;

import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.Master;

import java.io.IOException;
import java.util.ArrayList;

public class CommandParser {

    private static final String TAG = "AMTL";
    private static final String MODULE = "CommandParser";

    public static String parseXsioResponse(String xsio) {
        AlogMarker.tAB("CommandParser.parseXsioResponse", "0");
        int indexXsio = xsio.indexOf("+XSIO: ");
        AlogMarker.tAE("CommandParser.parseXsioResponse", "0");
        return xsio.substring(indexXsio + 7, indexXsio + 8);
    }

    public static String parseMasterResponse(String xsystrace, String master) {
        AlogMarker.tAB("CommandParser.parseMasterResponse", "0");
        int indexOfMaster;
        String masterResponse = "";
        int sizeOfMaster = master.length();

        if (sizeOfMaster <= 0) {
            Log.e(TAG, MODULE + ": cannot parse at+xsystrace=10 response");
        } else {
            indexOfMaster = xsystrace.indexOf(master);
            if ((indexOfMaster == -1)) {
                Log.e(TAG, MODULE + ": cannot parse at+xsystrace=10 response for master: "
                        + master);
            } else {
                String sub = xsystrace.substring(indexOfMaster + sizeOfMaster + 2);
                masterResponse = (sub.substring(0, sub.indexOf("\r\n"))).toUpperCase();
            }
        }
        AlogMarker.tAE("CommandParser.parseMasterResponse", "1");
        return masterResponse;
    }

    public static ArrayList<Master> parseXsystraceResponse(String xsystrace,
            ArrayList<Master> masterArray) {
        AlogMarker.tAB("CommandParser.parseXsystraceResponse", "0");
        if (masterArray != null) {
            for (Master m: masterArray) {
                m.setDefaultPort(parseMasterResponse(xsystrace, m.getName()));
            }
        }
        AlogMarker.tAE("CommandParser.parseXsystraceResponse", "0");
        return masterArray;
    }

    public static String parseTraceResponse(String trace) {
        AlogMarker.tAB("CommandParser.parseTraceResponse", "0");
        int indexTrace = trace.indexOf("+TRACE: ");
        AlogMarker.tAE("CommandParser.parseTraceResponse", "0");
        return trace.substring(indexTrace + 8, indexTrace + 9);
    }

    public static String parseOct(String xsystrace) {
        AlogMarker.tAB("CommandParser.parseOct", "0");
        String oct = "";

        if (xsystrace != null) {
            int indexOfOct = xsystrace.indexOf("mode");
            String sub = xsystrace.substring(indexOfOct + 5);
            oct = sub.substring(0, 1);
        }
        AlogMarker.tAE("CommandParser.parseOct", "0");
        return oct;
    }

    public static String parseProfileName(String xsystrace) {
        AlogMarker.tAB("CommandParser.parseProfileName", "0");
        String profileName = "";

        if (xsystrace != null) {
            int indexOfProfileName = xsystrace.indexOf("Active profile is:");
            String sub = xsystrace.substring(indexOfProfileName + 23);
            if (sub.contains("\r\n")) {
                profileName = sub.substring(0, sub.indexOf("\r\n"));
            } else {
                profileName = sub;
            }
        }
        AlogMarker.tAE("CommandParser.parseProfileName", "0");
        return profileName;
    }
}
