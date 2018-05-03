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

import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.util.TypedValue;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.Switch;

import com.intel.amtl.log.AlogMarker;

import java.util.Collection;
import java.util.Hashtable;
import java.util.Iterator;


public class LogOutput {

    private  final String TAG = "AMTL";
    private  final String MODULE = "LogOutput";

    private int index = -1;
    private String name = null;
    private String xsioValue = null;
    private String buttonColor = null;
    private String octDriverPath = null;
    private String mtsInput = null;
    private String mtsOutput = null;
    private String mtsOutputType = null;
    private String mtsRotateSize = null;
    private String mtsRotateNum = null;
    private String mtsInterface = null;
    private String mtsMode = null;
    private String mtsBufferSize = null;
    private String oct = null;
    private String octFcs = null;
    private String pti1 = null;
    private String pti2 = null;
    private Hashtable<String, Master> masterList = null;
    private Switch confSwitch = null;
    private String flcmd = null;
    private Alias alias = null;

    public LogOutput() {
        AlogMarker.tAB("LogOutput.LogOutput", "0");
        this.masterList = new Hashtable<String, Master>();
        AlogMarker.tAE("LogOutput.LogOutput", "0");
    }

    public LogOutput(int index, String name, String xsioValue, String buttonColor, String ioctl,
            String mtsInput, String mtsOutput, String mtsOutputType, String mtsRotateNum,
            String mtsRotateSize, String mtsInterface, String mtsMode, String mtsBufferSize,
            String oct, String octFcs, String pti1, String pti2, String flcmd) {
        AlogMarker.tAB("LogOutput.LogOutput", "0");
        this.setIndex(index);
        this.setName(name);
        this.setXsio(xsioValue);
        this.setButtonColor(buttonColor);
        this.setOctDriverPath(ioctl);
        this.setMtsInput(mtsInput);
        this.setMtsOutput(mtsOutput);
        this.setMtsRotateNum(mtsRotateNum);
        this.setMtsOutputType(mtsOutputType);
        this.setMtsRotateSize(mtsRotateSize);
        this.setMtsInterface(mtsInterface);
        this.setMtsMode(mtsMode);
        this.setMtsBufferSize(mtsBufferSize);
        this.masterList = new Hashtable<String, Master>();
        this.setOct(oct);
        this.setOctFcs(octFcs);
        this.setPti1(pti1);
        this.setPti2(pti2);
        this.setFlCmd(flcmd);
        AlogMarker.tAE("LogOutput.LogOutput", "0");
    }

    public void setIndex(int index) {
        AlogMarker.tAB("LogOutput.setIndex", "0");
        this.index = index;
        AlogMarker.tAE("LogOutput.setIndex", "0");
    }

    public void setName(String name) {
        AlogMarker.tAB("LogOutput.setName", "0");
        this.name = name;
        AlogMarker.tAE("LogOutput.setName", "0");
    }

    public void setXsio(String xsioValue) {
        AlogMarker.tAB("LogOutput.setXsio", "0");
        this.xsioValue = xsioValue;
        AlogMarker.tAE("LogOutput.setXsio", "0");
    }

    public void setButtonColor(String buttonColor) {
        AlogMarker.tAB("LogOutput.setButtonColor", "0");
        this.buttonColor = buttonColor;
        AlogMarker.tAE("LogOutput.setButtonColor", "0");
    }

    public void setOctDriverPath(String ioctl) {
        AlogMarker.tAB("LogOutput.setOctDriverPath", "0");
        this.octDriverPath = ioctl;
        AlogMarker.tAE("LogOutput.setOctDriverPath", "0");
    }

    public void setMtsInput(String mtsInput) {
        AlogMarker.tAB("LogOutput.setMtsInput", "0");
        this.mtsInput = mtsInput;
        AlogMarker.tAE("LogOutput.setMtsInput", "0");
    }

    public void setMtsOutput(String mtsOutput) {
        AlogMarker.tAB("LogOutput.setMtsOutput", "0");
        this.mtsOutput = mtsOutput;
        AlogMarker.tAE("LogOutput.setMtsOutput", "0");
    }

    public void setMtsOutputType(String mtsOutputType) {
        AlogMarker.tAB("LogOutput.setMtsOutputType", "0");
        this.mtsOutputType = mtsOutputType;
        AlogMarker.tAE("LogOutput.setMtsOutputType", "0");
    }

    public void setMtsRotateSize(String mtsRotateSize) {
        AlogMarker.tAB("LogOutput.setMtsRotateSize", "0");
        this.mtsRotateSize = mtsRotateSize;
        AlogMarker.tAE("LogOutput.setMtsRotateSize", "0");
    }

    public void setMtsRotateNum(String mtsRotateNum) {
        AlogMarker.tAB("LogOutput.setMtsRotateNum", "0");
        this.mtsRotateNum = mtsRotateNum;
        AlogMarker.tAE("LogOutput.setMtsRotateNum", "0");
    }

    public void setMtsInterface(String mtsInterface) {
        AlogMarker.tAB("LogOutput.setMtsInterface", "0");
        this.mtsInterface = mtsInterface;
        AlogMarker.tAE("LogOutput.setMtsInterface", "0");
    }

    public void setMtsMode(String mtsMode) {
        AlogMarker.tAB("LogOutput.setMtsMode", "0");
        this.mtsMode = mtsMode;
        AlogMarker.tAE("LogOutput.setMtsMode", "0");
    }

    public void setMtsBufferSize(String mtsBufferSize) {
        AlogMarker.tAB("LogOutput.setMtsBufferSize", "0");
        this.mtsBufferSize = mtsBufferSize;
        AlogMarker.tAE("LogOutput.setMtsBufferSize", "0");
    }

    public void setOct(String oct) {
        AlogMarker.tAB("LogOutput.setOct", "0");
        this.oct = oct;
        AlogMarker.tAE("LogOutput.setOct", "0");
    }

    public void setOctFcs(String octFcs) {
        AlogMarker.tAB("LogOutput.setOctFcs", "0");
        this.octFcs = octFcs;
        AlogMarker.tAE("LogOutput.setOctFcs", "0");
    }

    public void setPti1(String pti1) {
        AlogMarker.tAB("LogOutput.setPti1", "0");
        this.pti1 = pti1;
        AlogMarker.tAE("LogOutput.setPti1", "0");
    }

    public void setPti2(String pti2) {
        AlogMarker.tAB("LogOutput.setPti2", "0");
        this.pti2 = pti2;
        AlogMarker.tAE("LogOutput.setPti2", "0");
    }

    public void setFlCmd(String flcmd) {
        AlogMarker.tAB("LogOutput.setFlCmd", "0");
        this.flcmd = flcmd;
        AlogMarker.tAE("LogOutput.setFlCmd", "0");
    }

    public int getIndex() {
        AlogMarker.tAB("LogOutput.getIndex", "0");
        AlogMarker.tAE("LogOutput.getIndex", "0");
        return this.index;
    }

    public String getName() {
        AlogMarker.tAB("LogOutput.getName", "0");
        AlogMarker.tAE("LogOutput.getName", "0");
        return this.name;
    }

    public String getXsio() {
        AlogMarker.tAB("LogOutput.getXsio", "0");
        AlogMarker.tAE("LogOutput.getXsio", "0");
        return this.xsioValue;
    }

    public String getButtonColor() {
        AlogMarker.tAB("LogOutput.getButtonColor", "0");
        AlogMarker.tAE("LogOutput.getButtonColor", "0");
        return this.buttonColor;
    }

    public String getOctDriverPath() {
        AlogMarker.tAB("LogOutput.getOctDriverPath", "0");
        AlogMarker.tAE("LogOutput.getOctDriverPath", "0");
        return this.octDriverPath;
    }

    public String getMtsInput() {
        AlogMarker.tAB("LogOutput.getMtsInput", "0");
        AlogMarker.tAE("LogOutput.getMtsInput", "0");
        return this.mtsInput;
    }

    public String getMtsOutput() {
        AlogMarker.tAB("LogOutput.getMtsOutput", "0");
        AlogMarker.tAE("LogOutput.getMtsOutput", "0");
        return this.mtsOutput;
    }

    public String getMtsOutputType() {
        AlogMarker.tAB("LogOutput.getMtsOutputType", "0");
        AlogMarker.tAE("LogOutput.getMtsOutputType", "0");
        return this.mtsOutputType;
    }

    public String getMtsRotateSize() {
        AlogMarker.tAB("LogOutput.getMtsRotateSize", "0");
        AlogMarker.tAE("LogOutput.getMtsRotateSize", "0");
        return this.mtsRotateSize;
    }

    public String getMtsRotateNum() {
        AlogMarker.tAB("LogOutput.getMtsRotateNum", "0");
        AlogMarker.tAE("LogOutput.getMtsRotateNum", "0");
        return this.mtsRotateNum;
    }

    public String getMtsInterface() {
        AlogMarker.tAB("LogOutput.getMtsInterface", "0");
        AlogMarker.tAE("LogOutput.getMtsInterface", "0");
        return this.mtsInterface;
    }

    public String getMtsMode() {
        AlogMarker.tAB("LogOutput.getMtsMode", "0");
        AlogMarker.tAE("LogOutput.getMtsMode", "0");
        return this.mtsMode;
    }

    public String getMtsBufferSize() {
        AlogMarker.tAB("LogOutput.getMtsBufferSize", "0");
        AlogMarker.tAE("LogOutput.getMtsBufferSize", "0");
        return this.mtsBufferSize;
    }

    public String getOct() {
        AlogMarker.tAB("LogOutput.getOct", "0");
        AlogMarker.tAE("LogOutput.getOct", "0");
        return this.oct;
    }

    public String getOctFcs() {
        AlogMarker.tAB("LogOutput.getOctFcs", "0");
        AlogMarker.tAE("LogOutput.getOctFcs", "0");
        return this.octFcs;
    }

    public String getPti1() {
        AlogMarker.tAB("LogOutput.getPti1", "0");
        AlogMarker.tAE("LogOutput.getPti1", "0");
        return this.pti1;
    }

    public String getPti2() {
        AlogMarker.tAB("LogOutput.getPti2", "0");
        AlogMarker.tAE("LogOutput.getPti2", "0");
        return this.pti2;
    }

    public String getFlCmd() {
        AlogMarker.tAB("LogOutput.getFlCmd", "0");
        AlogMarker.tAE("LogOutput.getFlCmd", "0");
        return this.flcmd;
    }

    public Hashtable<String, Master> getMasterList() {
        AlogMarker.tAB("LogOutput.getMasterList", "0");
        AlogMarker.tAE("LogOutput.getMasterList", "0");
        return this.masterList;
    }

    public void setMasterList(Hashtable<String, Master> masterList) {
        AlogMarker.tAB("LogOutput.setMasterList", "0");
        AlogMarker.tAE("LogOutput.setMasterList", "0");
        this.masterList = masterList;
    }

    public Master getMasterFromList(String key) {
        AlogMarker.tAB("LogOutput.getMasterFromList", "0");
        AlogMarker.tAE("LogOutput.getMasterFromList", "0");
        return (Master)this.masterList.get(key);
    }

    public void addMasterToList(String key, Master master) {
        AlogMarker.tAB("LogOutput.addMasterToList", "0");
        AlogMarker.tAE("LogOutput.addMasterToList", "0");
        this.masterList.put(key, master);
    }

    public String concatMasterPort() {
        AlogMarker.tAB("LogOutput.concatMasterPort", "0");
        String ret = "";

        Collection<Master> c = this.masterList.values();
        Iterator<Master> it = c.iterator();

        ret += "\"";
        while (it.hasNext()) {
            Master master = it.next();
            ret += master.getName();
            ret += "=";
            ret += master.getDefaultPort();

            if (it.hasNext()) {
                ret += ";";
            }
        }
        if (ret.endsWith(";")) {
            ret = ret.substring(0, ret.length() - 1);
        }

        ret += "\"";
        if (ret.equals("\"\"")) {
            AlogMarker.tAE("LogOutput.concatMasterPort", "0");
            return "";
        }
        AlogMarker.tAE("LogOutput.concatMasterPort", "0");
        return ret;
    }

    public String concatMasterOption() {
        AlogMarker.tAB("LogOutput.concatMasterOption", "0");
        String ret = "";

        Collection<Master> c = this.masterList.values();
        Iterator<Master> it = c.iterator();

        ret += "\"";
        while (it.hasNext()) {
            Master master = it.next();
            if (master.getDefaultConf() != null
                  && !master.getDefaultConf().equals("")) {
                ret += master.getName();
                ret += "=";
                ret += master.getDefaultConf();
                if (it.hasNext()) {
                    ret += ";";
                }
            }
        }
        if (ret.endsWith(";")) {
            ret = ret.substring(0, ret.length() - 1);
        }

        ret += "\"";
        if (ret.equals("\"\"")) {
            AlogMarker.tAE("LogOutput.concatMasterOption", "0");
            return "";
        }

        AlogMarker.tAE("LogOutput.concatMasterOption", "0");
        return ret;
        }

    public void removeMasterFromList(String key) {
        AlogMarker.tAB("LogOutput.removeMasterFromList", "0");
        this.masterList.remove(key);
        AlogMarker.tAE("LogOutput.removeMasterFromList", "0");
    }

    public void setAlias(Alias alias) {
        AlogMarker.tAB("LogOutput.setAlias", "0");
        this.alias = alias;
        AlogMarker.tAE("LogOutput.setAlias", "0");
    }

    public Alias getAlias() {
        AlogMarker.tAB("LogOutput.getAlias", "0");
        AlogMarker.tAE("LogOutput.getAlias", "0");
        return this.alias;
    }

    public String concatAlias() {
        AlogMarker.tAB("LogOutput.concatAlias", "0");
        String ret = "";
        if (alias != null) {
            String profileName = alias.getProfileName();
            if (profileName != null) {
                ret = "\"" + alias.getProfileName() +  "\"";
                if (alias.getDestination() != null) {
                    ret += "," + "\"proute=" + alias.getDestination() +  "\"";
                }
            }
        }
        AlogMarker.tAE("LogOutput.concatAlias", "0");
        return ret;
    }

    public Switch setConfigSwitch(LinearLayout ll, int index, Context ctx, View view) {
        AlogMarker.tAB("LogOutput.setConfigSwitch", "0");
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
             LinearLayout.LayoutParams.MATCH_PARENT,
             LinearLayout.LayoutParams.MATCH_PARENT);
        params.setMargins(0, 40, 0, 0);
        confSwitch = new Switch(ctx);
        confSwitch.setId(index);
        int id = confSwitch.getId();
        confSwitch.setText(this.getName());
        confSwitch.setTextOff("OFF");
        confSwitch.setTextOn("ON");
        confSwitch.setEnabled(false);
        confSwitch.setTextColor(Color.parseColor(this.buttonColor));
        confSwitch.setTextSize(TypedValue.COMPLEX_UNIT_SP, 19);
        ll.addView(confSwitch, params);
        Switch configSwitch = ((Switch) view.findViewById(id));
        AlogMarker.tAE("LogOutput.setConfigSwitch", "0");
        return configSwitch;
    }

    public int getSwitchId() {
        AlogMarker.tAB("LogOutput.getSwitchId", "0");
        AlogMarker.tAE("LogOutput.getSwitchId", "0");
        return this.confSwitch.getId();
    }

    public Switch getConfSwitch() {
        AlogMarker.tAB("LogOutput.getConfSwitch", "0");
        AlogMarker.tAE("LogOutput.getConfSwitch", "0");
        return this.confSwitch;
    }

    public void printToLog() {
        AlogMarker.tAB("LogOutput.printToLog", "0");
        Collection<Master> c = this.masterList.values();
        Iterator<Master> it = c.iterator();
        Log.d(TAG, MODULE + ": index = " + this.index + ", name = " + this.name
                + ", value = " + this.xsioValue
                + ", color = " + this.buttonColor
                + ", mts_input = " + this.mtsInput
                + ", mts_output = " + this.mtsOutput
                + ", mts_output_type = " + this.mtsOutputType
                + ", mts_rotate_num = " + this.mtsRotateNum
                + ", mts_rotate_size = " + this.mtsRotateSize
                + ", mts_interface = " + this.mtsInterface
                + ", mts_mode = " + this.mtsMode
                + ", mts_buffer_size = " + this.mtsBufferSize
                + ", oct = " + this.oct + ", oct_fcs = " + this.octFcs
                + ", pti1 = " + this.pti1 + ", pti2 = " + this.pti2
                + ", flush_command = " + this.flcmd + ".");
        if (alias != null) {
            alias.printToLog();
        }
        while (it.hasNext()) {
            Master master = it.next();
            master.printToLog();
        }
        AlogMarker.tAE("LogOutput.printToLog", "0");
    }
}
