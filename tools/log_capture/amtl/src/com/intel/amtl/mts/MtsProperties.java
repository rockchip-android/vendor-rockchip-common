/* Android Modem Traces and Logs
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
 */

package com.intel.amtl.mts;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.log.AlogMarker;

public class MtsProperties {

    // TODO: to be modified once mts is adapted

    public static String getInput() {
        AlogMarker.tAB("MtsProperties.getInput", "0");
        AlogMarker.tAE("MtsProperties.getInput", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.input" : "persist.service.mts.input";
    }

    public static String getOutput() {
        AlogMarker.tAB("MtsProperties.getOutput", "0");
        AlogMarker.tAE("MtsProperties.getOutput", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.output" : "persist.service.mts.output";
    }

    public static String getOutputType() {
        AlogMarker.tAB("MtsProperties.getOutputType", "0");
        AlogMarker.tAE("MtsProperties.getOutputType", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.output_type" : "persist.service.mts.output_type";
    }

    public static String getRotateNum() {
        AlogMarker.tAB("MtsProperties.getRotateNum", "0");
        AlogMarker.tAE("MtsProperties.getRotateNum", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.rotate_num" : "persist.service.mts.rotate_num";
    }

    public static String getRotateSize() {
        AlogMarker.tAB("MtsProperties.getRotateSize", "0");
        AlogMarker.tAE("MtsProperties.getRotateSize", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.rotate_size" : "persist.service.mts.rotate_size";
    }

    public static String getInterface() {
        AlogMarker.tAB("MtsProperties.getInterface", "0");
        AlogMarker.tAE("MtsProperties.getInterface", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.interface" : "persist.service.mts.interface";
    }

    public static String getBufferSize() {
        AlogMarker.tAB("MtsProperties.getBufferSize", "0");
        AlogMarker.tAE("MtsProperties.getBufferSize", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mts2.buffer_size" : "persist.service.mts.buffer_size";
    }

    public static String getPersistentService() {
        AlogMarker.tAB("MtsProperties.getPersistentService", "0");
        AlogMarker.tAE("MtsProperties.getPersistentService", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mtsp2.enable" : "persist.service.mtsp.enable";
    }

    public static String getOneshotService() {
        AlogMarker.tAB("MtsProperties.getOneshotService", "0");
        AlogMarker.tAE("MtsProperties.getOneshotService", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "persist.sys.mtso2.enable" : "persist.service.mtso.enable";
    }

    public static String getStatus() {
        AlogMarker.tAB("MtsProperties.getStatus", "0");
        AlogMarker.tAE("MtsProperties.getStatus", "0");
        return (AMTLApplication.getServiceToStart().equals("mts2"))
                ? "init.svc.mtsp2" : "init.svc.mtsp";
    }
}
