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

import android.os.SystemProperties;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.StoredSettings;

public class MtsConf {

    private String mtsInput = "";
    private String mtsOutput = "";
    private String mtsOutputType = "";
    private String mtsRotateNum = "";
    private String mtsRotateSize = "";
    private String mtsInterface = "";
    private String mtsBufferSize = "";


    public MtsConf(String input, String output, String outputType, String rotateNum,
            String rotateSize, String interFace, String bufferSize) {
        AlogMarker.tAB("MtsConf.MtsConf", "0");
        this.mtsInput = (input != null) ? input : this.mtsInput;
        this.mtsOutput = (output != null) ? output : this.mtsOutput;
        this.mtsOutputType = (outputType != null) ? outputType : this.mtsOutputType;
        this.mtsRotateNum = (rotateNum != null) ? rotateNum : this.mtsRotateNum;
        this.mtsRotateSize = (rotateSize != null) ? rotateSize : this.mtsRotateSize;
        this.mtsInterface = (interFace != null) ? interFace : this.mtsInterface;
        this.mtsBufferSize = (bufferSize != null) ? bufferSize : this.mtsBufferSize;
        AlogMarker.tAE("MtsConf.MtsConf", "0");
    }

    public MtsConf() {
        AlogMarker.tAB("MtsConf.MtsConf", "0");
        AlogMarker.tAE("MtsConf.MtsConf", "0");
    }

    public String getInput() {
        AlogMarker.tAB("MtsConf.getInput", "0");
        AlogMarker.tAE("MtsConf.getInput", "0");
        return this.mtsInput;
    }

    public String getOutput() {
        AlogMarker.tAB("MtsConf.getOutput", "0");
        AlogMarker.tAE("MtsConf.getOutput", "0");
        return this.mtsOutput;
    }

    public String getOutputType() {
        AlogMarker.tAB("MtsConf.getOutputType", "0");
        AlogMarker.tAE("MtsConf.getOutputType", "0");
        return this.mtsOutputType;
    }

    public String getRotateNum() {
        AlogMarker.tAB("MtsConf.getRotateNum", "0");
        AlogMarker.tAE("MtsConf.getRotateNum", "0");
        return this.mtsRotateNum;
    }

    public String getRotateSize() {
        AlogMarker.tAB("MtsConf.getRotateSize", "0");
        AlogMarker.tAE("MtsConf.getRotateSize", "0");
        return this.mtsRotateSize;
    }

    public String getInterface() {
        AlogMarker.tAB("MtsConf.getInterface", "0");
        AlogMarker.tAE("MtsConf.getInterface", "0");
        return this.mtsInterface;
    }

    public String getBufferSize() {
        AlogMarker.tAB("MtsConf.getBufferSize", "0");
        AlogMarker.tAE("MtsConf.getBufferSize", "0");
        return this.mtsBufferSize;
    }

    public void applyParameters() {
        AlogMarker.tAB("MtsConf.applyParameters", "0");
        SystemProperties.set(MtsProperties.getInput(), this.mtsInput);
        SystemProperties.set(MtsProperties.getOutputType(), this.mtsOutputType);
        if (this.mtsOutputType.equals("f")) {
            StoredSettings privatePrefs = new StoredSettings(AMTLApplication.getContext());
            String oldPath = mtsOutput.substring(0, mtsOutput.indexOf("/bplog"));
            String newPath = mtsOutput.replace(oldPath, privatePrefs.getBPLoggingPath());
            SystemProperties.set(MtsProperties.getOutput(), newPath);
            SystemProperties.set(MtsProperties.getRotateNum(), privatePrefs.getBPFileCount());
            SystemProperties.set(MtsProperties.getRotateSize(), privatePrefs.getBPTraceSize());
        } else {
            SystemProperties.set(MtsProperties.getOutput(), this.mtsOutput);
            SystemProperties.set(MtsProperties.getRotateNum(), this.mtsRotateNum);
            SystemProperties.set(MtsProperties.getRotateSize(), this.mtsRotateSize);
        }
        SystemProperties.set(MtsProperties.getInterface(), this.mtsInterface);
        SystemProperties.set(MtsProperties.getBufferSize(), this.mtsBufferSize);
        AlogMarker.tAE("MtsConf.applyParameters", "0");
    }
}
