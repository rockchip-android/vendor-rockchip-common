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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.models.config;

import android.util.Log;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.mts.MtsConf;
import com.intel.amtl.models.config.AliasModemConf;
import com.intel.amtl.models.config.OctModemConf;
import com.intel.amtl.models.config.TraceLegacyModemConf;

public abstract class ModemConf {

    private static final String TAG = "AMTL";
    private static final String MODULE = "ModemConf";
    private String atXsio = "";
    private String atXsystrace = "";
    private String mtsMode = "";
    private String profileName = "UNKNOWN";
    private String flCmd = "";
    private String octMode = "";
    private String octDriverPath = "";
    private boolean mtsRequired = false;
    private int confIndex = -1;

    private LogOutput config = null;
    private MtsConf mtsConf = null;
    private Alias alias = null;

    protected String atTrace = "";

    public abstract boolean confTraceEnabled();
    public abstract void activateConf(boolean activate);

    public synchronized static ModemConf getInstance(LogOutput config) {
        AlogMarker.tAB("ModemConf.getInstance", "0");
        AlogMarker.tAE("ModemConf.getInstance", "0");
        return (AMTLApplication.getTraceLegacy())
                ? new TraceLegacyModemConf(config) : new OctModemConf(config);
    }

    public synchronized static ModemConf getInstance(String xsio, String trace, String xsystrace,
            String flcmd, String octMode) {
        AlogMarker.tAB("ModemConf.getInstance", "0");
        AlogMarker.tAE("ModemConf.getInstance", "0");
        return (AMTLApplication.getTraceLegacy())
                ? new TraceLegacyModemConf(xsio, trace, xsystrace, flcmd, octMode)
                : (AMTLApplication.getIsAliasUsed())
                ? new AliasModemConf(xsio, trace, xsystrace, flcmd, octMode)
                : new OctModemConf(xsio, trace, xsystrace, flcmd, octMode);
    }

    public ModemConf(LogOutput config) {
        AlogMarker.tAB("ModemConf.ModemConf", "0");
        this.config = config;
        this.confIndex = this.config.getIndex();
        this.alias = this.config.getAlias();
        if (alias != null) {
            this.atXsystrace = "AT+XSYSTRACE=" + this.config.concatAlias();
        } else {
            this.atXsystrace = "AT+XSYSTRACE=1," + this.config.concatMasterPort();
        }

        if (this.config.getFlCmd() != null) {
            this.flCmd = this.config.getFlCmd() + "\r\n";
        }
        if (this.config.getXsio() != null || this.config.getOct() != null) {

            this.atXsystrace += "," + this.config.concatMasterOption();

            if (this.config.getOct() != null || this.config.getOctFcs() != null) {
                this.atXsystrace += ",\"";
                if (this.config.getOct() != null) {
                    this.atXsystrace += "oct=";
                    this.atXsystrace += this.config.getOct();
                }
                if (this.config.getOctFcs() != null) {
                    if (this.config.getOct() != null) {
                        this.atXsystrace += ";";
                    }
                    this.atXsystrace += "oct_fcs=";
                    this.atXsystrace += this.config.getOctFcs();
                }
                this.atXsystrace += "\"";
            }
            if (this.config.getPti1() != null) {
                this.atXsystrace += ",\"pti1=";
                this.atXsystrace += this.config.getPti1();
                this.atXsystrace += "\"";
            }
            if (this.config.getPti2() != null) {
                this.atXsystrace += ",\"pti2=";
                this.atXsystrace += this.config.getPti2();
                this.atXsystrace += "\"";
            }
        }
        this.atXsystrace += "\r\n";

        if (this.config.getXsio() != null) {
            this.atXsio = "AT+XSIO=" + this.config.getXsio() + "\r\n";
        }

        this.mtsMode = this.config.getMtsMode();
        this.mtsConf = new MtsConf(this.config.getMtsInput(), this.config.getMtsOutput(),
                this.config.getMtsOutputType(), this.config.getMtsRotateNum(),
                this.config.getMtsRotateSize(), this.config.getMtsInterface(),
                this.config.getMtsBufferSize());
        AlogMarker.tAE("ModemConf.ModemConf", "0");
    }

    public ModemConf(String xsio, String trace, String xsystrace, String flcmd, String octMode) {
        AlogMarker.tAB("ModemConf.ModemConf", "0");
        this.atTrace = trace;
        this.atXsio = xsio;
        this.atXsystrace = xsystrace;
        this.flCmd = flcmd;
        this.confIndex = -1;
        this.mtsConf = new MtsConf();
        if (!octMode.equals("")) {
            this.octMode = octMode;
        }

        if (AMTLApplication.getIsAliasUsed()) {
            String allOff = "all_off";
            String xsystraceProf = "AT+XSYSTRACE=pn";
            if (xsystrace.contains(allOff)) {
                setProfileName(allOff);
            } else if (xsystrace.contains(xsystraceProf)) {
                setProfileName(xsystrace.substring(xsystraceProf.length(), xsystrace.indexOf(",")));
            }
        }
        AlogMarker.tAE("ModemConf.ModemConf", "0");
    }

    public void setMtsConf(MtsConf conf) {
        AlogMarker.tAB("ModemConf.setMtsConf", "0");
        if (conf != null) {
            this.mtsConf = conf;
        }
        AlogMarker.tAE("ModemConf.setMtsConf", "0");
    }

    public MtsConf getMtsConf() {
        AlogMarker.tAB("ModemConf.getMtsConf", "0");
        AlogMarker.tAE("ModemConf.getMtsConf", "0");
        return this.mtsConf;
    }

    public String getXsio() {
        AlogMarker.tAB("ModemConf.getXsio", "0");
        AlogMarker.tAE("ModemConf.getXsio", "0");
        return this.atXsio;
    }

    public String getTrace() {
        AlogMarker.tAB("ModemConf.getTrace", "0");
        AlogMarker.tAE("ModemConf.getTrace", "0");
        return this.atTrace;
    }

    public String getXsystrace() {
        AlogMarker.tAB("ModemConf.getXsystrace", "0");
        AlogMarker.tAE("ModemConf.getXsystrace", "0");
        return this.atXsystrace;
    }

    public String getFlCmd() {
        AlogMarker.tAB("ModemConf.getFlCmd", "0");
        AlogMarker.tAE("ModemConf.getFlCmd", "0");
        return this.flCmd;
    }

    public int getIndex() {
        AlogMarker.tAB("ModemConf.getIndex", "0");
        AlogMarker.tAE("ModemConf.getIndex", "0");
        return this.confIndex;
    }

    public void setIndex(int index) {
        AlogMarker.tAB("ModemConf.setIndex", "0");
        AlogMarker.tAE("ModemConf.setIndex", "0");
        this.confIndex = index;
    }

    public String getMtsMode() {
        AlogMarker.tAB("ModemConf.getMtsMode", "0");
        AlogMarker.tAE("ModemConf.getMtsMode", "0");
        return this.mtsMode;
    }

    public void setMtsMode(String mode) {
        AlogMarker.tAB("ModemConf.setMtsMode", "0");
        AlogMarker.tAE("ModemConf.setMtsMode", "0");
        this.mtsMode = mode;
    }

    public String getOctDriverPath() {
        AlogMarker.tAB("ModemConf.getOctDriverPath", "0");
        AlogMarker.tAE("ModemConf.getOctDriverPath", "0");
        return this.octDriverPath;
    }

    public void setOctDriverPath(String path) {
        AlogMarker.tAB("ModemConf.setOctDriverPath", "0");
        AlogMarker.tAE("ModemConf.setOctDriverPath", "0");
        this.octDriverPath = path;
    }

    public String getProfileName() {
        AlogMarker.tAB("ModemConf.getProfileName", "0");
        AlogMarker.tAE("ModemConf.getProfileName", "0");
        return this.profileName;
    }

    public void setProfileName(String name) {
        AlogMarker.tAB("ModemConf.setProfileName", "0");
        AlogMarker.tAE("ModemConf.setProfileName", "0");
        this.profileName = name;
    }

    public void applyMtsParameters() {
        AlogMarker.tAB("ModemConf.applyMtsParameters", "0");
        if (this.mtsConf != null) {
            this.mtsConf.applyParameters();
            printMtsToLog();
        }
        AlogMarker.tAE("ModemConf.applyMtsParameters", "0");
    }

    public boolean isMtsRequired() {
        AlogMarker.tAB("ModemConf.isMtsRequired", "0");
        if (this.mtsConf != null) {
            AlogMarker.tAE("ModemConf.isMtsRequired", "0");
            return (!this.mtsConf.getOutput().equals("")
                && !this.mtsConf.getOutputType().equals("") && !getMtsMode().equals(""));
        }
        AlogMarker.tAE("ModemConf.isMtsRequired", "0");
        return false;
    }

    public void setOctMode(String octMode) {
        AlogMarker.tAB("ModemConf.setOctMode", "0");
        this.octMode = octMode;
        AlogMarker.tAE("ModemConf.setOctMode", "0");
    }

    public String getOctMode() {
        AlogMarker.tAB("ModemConf.getOctMode", "0");
        AlogMarker.tAE("ModemConf.getOctMode", "0");
        return this.octMode;
    }

    public void updateProfileName(String profile) {
        AlogMarker.tAB("ModemConf.updateModemProfile", "0");
        String xsystrace = "AT+XSYSTRACE=";
        String previousProfile = this.atXsystrace.substring(xsystrace.length(),
                this.atXsystrace.indexOf(","));
        this.atXsystrace = this.atXsystrace.replace(previousProfile, "pn" + profile);
        AlogMarker.tAE("ModemConf.updateModemProfile", "0");
    }

    public void printMtsToLog() {
        AlogMarker.tAB("ModemConf.printMtsToLog", "0");
        if (this.mtsConf != null) {
            Log.d(TAG, MODULE + ": ========= MTS CONFIGURATION =========");
            Log.d(TAG, MODULE + ": INPUT = " + this.mtsConf.getInput());
            Log.d(TAG, MODULE + ": OUTPUT = " + this.mtsConf.getOutput());
            Log.d(TAG, MODULE + ": OUTPUT TYPE = " + this.mtsConf.getOutputType());
            Log.d(TAG, MODULE + ": ROTATE NUM = " + this.mtsConf.getRotateNum());
            Log.d(TAG, MODULE + ": ROTATE SIZE = " + this.mtsConf.getRotateSize());
            Log.d(TAG, MODULE + ": INTERFACE = " + this.mtsConf.getInterface());
            Log.d(TAG, MODULE + ": BUFFER SIZE = " + this.mtsConf.getBufferSize());
            Log.d(TAG, MODULE + ": MODE = " + this.mtsMode);
            Log.d(TAG, MODULE + ": =======================================");
        }
        AlogMarker.tAE("ModemConf.printMtsToLog", "0");
    }

    public void printToLog() {
        AlogMarker.tAB("ModemConf.printToLog", "0");
        Log.d(TAG, MODULE + ": ========= MODEM CONFIGURATION =========");
        Log.d(TAG, MODULE + ": XSIO = " + this.atXsio);
        Log.d(TAG, MODULE + ": TRACE = " + this.atTrace);
        Log.d(TAG, MODULE + ": XSYSTRACE = " + this.atXsystrace);
        Log.d(TAG, MODULE + ": OCT = " + this.octMode);
        Log.d(TAG, MODULE + ": =======================================");
        AlogMarker.tAE("ModemConf.printToLog", "0");
    }

}
