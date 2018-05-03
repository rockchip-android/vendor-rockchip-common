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

package com.intel.amtl.models;

import android.util.Log;
import android.widget.Toast;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.ConfigManager;
import com.intel.amtl.models.config.AtProxy;
import com.intel.amtl.models.config.ExpertConfig;
import com.intel.amtl.models.config.LogOutput;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.modem.controller.ModemController;
import com.intel.amtl.mts.MtsManager;
import com.intel.amtl.StoredSettings;

import java.util.ArrayList;

public class ConfigManager {

    private static final String TAG = "AMTL";
    private static final String MODULE = "ConfigManager";
    private static final String NO_ACM = "mtp,adb";
    private static final String ONE_ACM = "rndis,acm,adb";
    private static final String TWO_ACM = "rndis,acm,acm,adb";
    private static final String AT_PROXY_OFF = "0";

    private static StoredSettings sSettings = null;

    public ConfigManager() {
        if (sSettings == null) {
            sSettings = new StoredSettings(AMTLApplication.getContext());
        }
    }

    private static StoredSettings getStoredSettings() {
        if (sSettings == null) {
            sSettings = new StoredSettings(AMTLApplication.getContext());
        }
        return sSettings;
    }

    public static int applyConfig(ModemConf mdmConf, ModemController modemCtrl)
        throws ModemControlException {
        AlogMarker.tAB("ConfigManager.applyConfig", "0");
        if (modemCtrl != null) {
            if (mdmConf != null) {
                // send the commands to set the new configuration
                modemCtrl.confTraceAndModemInfo(mdmConf);
                mdmConf = checkProfileSent(mdmConf);
                modemCtrl.switchTrace(mdmConf);
                modemCtrl.sendFlushCmd(mdmConf);

                MtsManager.stopServices();
                // set mts parameters through mts properties
                mdmConf.applyMtsParameters();
                // check if the configuration requires mts
                if (mdmConf.isMtsRequired()) {
                    // start mts in the chosen mode: persistent or oneshot
                    MtsManager.startService(mdmConf.getMtsMode());
                }
                // restart modem by a cold reset
                modemCtrl.restartModem();
            } else {
                throw new ModemControlException("no configuration to apply");
            }
        } else {
            throw new ModemControlException("cannot apply configuration: modemCtrl is null");
        }
        AlogMarker.tAE("ConfigManager.applyConfig", "0");
        return mdmConf.getIndex();
    }

    private static ModemConf checkProfileSent(ModemConf mdmCf) {
        AlogMarker.tAB("ConfigManager.checkProfileSent", "0");
        if (AMTLApplication.getIsAliasUsed()) {
            String currModemProfile = mdmCf.getProfileName();
            if (!currModemProfile.equals("all_off")) {
                String profileName = getStoredSettings().getModemProfile();
                if (!profileName.equals(currModemProfile)) {
                    mdmCf.updateProfileName(profileName);
                }
            }
        }
        AlogMarker.tAE("ConfigManager.checkProfileSent", "0");
        return mdmCf;
    }

    private static int resetConf(ModemController mdmCtrl, ModemConf conf)
            throws ModemControlException {
        AlogMarker.tAB("ConfigManager.resetConf", "0");
        mdmCtrl.switchOffTrace();
        mdmCtrl.sendFlushCmd(conf);
        Toast toast = Toast.makeText(AMTLApplication.getContext(), "Configuration not recognized, "
                + "stopping traces", Toast.LENGTH_LONG);
        toast.show();
        AlogMarker.tAE("ConfigManager.resetConf", "0");
        return -1;
    }

    private static boolean checkOutputConfig(LogOutput out) {
        AlogMarker.tAB("ConfigManager.checkOutputConfig", "0");
        boolean bRet = true;
        if (out != null) {
            if (AMTLApplication.getXsioDefined()) {
                if (out.getXsio() != null) {
                    bRet = false;
                }
            } else {
                if (out.getOctDriverPath() != null) {
                    bRet = false;
                }
            }
        }
        AlogMarker.tAE("ConfigManager.checkOutputConfig", "0");
        return bRet;
    }

    private static boolean compareOutputConfig(LogOutput out, ModemConf conf) {
        AlogMarker.tAB("ConfigManager.compareOutputConfig", "0");
        boolean bRet = false;
        if (AMTLApplication.getXsioDefined()) {
            if (out.getXsio().equals(conf.getXsio())) {
                bRet = true;
            }
        } else {
            if (out.getOctDriverPath().equals("NONE")) {
                bRet = true;
            } else if (out.getOctDriverPath().equals(conf.getOctDriverPath())) {
                bRet = true;
            }
        }
        AlogMarker.tAE("ConfigManager.compareOutputConfig", "0");
        return bRet;
    }

    public static int updateCurrentIndex(ModemConf curModConf, int currentIndex, String modemName,
            ModemController modemCtrl, ArrayList<LogOutput> configArray) {
        AlogMarker.tAB("ConfigManager.updateCurrentIndex", "0");
        boolean confReset = false;
        int updatedIndex = currentIndex;
        try {
            if (updatedIndex == -2) {
                LogOutput defaultConf = AMTLApplication.getDefaultConf();
                if (defaultConf != null && !checkOutputConfig(defaultConf)
                       && defaultConf.getOct() != null) {
                    if (compareOutputConfig(defaultConf, curModConf)
                            && defaultConf.getOct().equals(curModConf.getOctMode())) {
                        updatedIndex = defaultConf.getIndex();
                    } else {
                        updatedIndex = resetConf(modemCtrl, curModConf);
                        confReset = true;
                    }
                } else {
                    if (configArray != null) {
                        boolean confChanged = false;
                        for (LogOutput o: configArray) {
                            if (o != null && !checkOutputConfig(o)
                                    && o.getMtsOutput() != null && o.getOct() != null) {
                                if (compareOutputConfig(o, curModConf)
                                        && o.getMtsOutput().equals(curModConf.getMtsConf()
                                        .getOutput()) && o.getOct()
                                        .equals(curModConf.getOctMode())) {
                                    updatedIndex = o.getIndex();
                                    confChanged = true;
                                }
                            }
                        }
                        if (!confChanged) {
                            updatedIndex = resetConf(modemCtrl, curModConf);
                            confReset = true;
                        }
                    } else {
                        updatedIndex = resetConf(modemCtrl, curModConf);
                        confReset = true;
                    }
                }
            }
            if (updatedIndex >= 0 || ExpertConfig.isExpertModeEnabled(modemName)) {
                if (curModConf.confTraceEnabled()) {
                    if (curModConf.isMtsRequired() && curModConf.getMtsMode().equals("persistent")
                            && !MtsManager.getMtsState().equals("running")) {
                        MtsManager.startService(curModConf.getMtsMode());
                    } else if (!curModConf.isMtsRequired() && curModConf.getMtsMode()
                            .equals("persistent") && !MtsManager.getMtsState().equals("stopped")) {
                        MtsManager.stopServices();
                    }
                } else {
                    ExpertConfig.setExpertMode(modemName, false);
                    updatedIndex = -1;
                }
            }

            if (updatedIndex == -1 && !ExpertConfig.isExpertModeEnabled(modemName)) {
                if (curModConf.confTraceEnabled() && !confReset) {
                    updatedIndex = resetConf(modemCtrl, curModConf);
                }
                if (!MtsManager.getMtsState().equals("stopped")) {
                    MtsManager.stopServices();
                }
            }
        } catch (ModemControlException ex) {
            Log.e(TAG, MODULE + " : an error occured while stopping logs " + ex);
        }
        AlogMarker.tAE("ConfigManager.updateCurrentIndex", "0");
        return updatedIndex;
    }

    public static void applyAtProxyConfig(ModemConf modConfToApply, boolean atProxyToStart) {
        AlogMarker.tAB("ConfigManager.updateAtProxyConf", "0");

        boolean logAndCtrl = AMTLApplication.getLogAndControl();
        String atProxyMode = AtProxy.getAtProxyMode();
        String usbConfValue = AtProxy.getUsbConfigValue();
        boolean usbLoggingConf = false;
        int index = modConfToApply.getIndex();
        String mtsOutput = modConfToApply.getMtsConf().getOutput();

        String storedAtProxyMode = getStoredSettings().getAtProxyMode();

        if (mtsOutput != null && mtsOutput.contains("ttyGS") && index >= 0) {
            usbLoggingConf = true;
        }

        if (atProxyToStart) {
            if (!atProxyMode.equals(storedAtProxyMode)) {
                Log.d(TAG, MODULE + ": Starting at proxy in mode " + storedAtProxyMode);
                AtProxy.setAtProxyMode(storedAtProxyMode);
                AtProxy.setUsbConfigValue(ONE_ACM);
            } else {
                if (!usbConfValue.equals(ONE_ACM)) {
                    AtProxy.setUsbConfigValue(ONE_ACM);
                }
            }
        } else {
            if (usbLoggingConf) {
                if (logAndCtrl) {
                    if (!atProxyMode.equals(storedAtProxyMode)) {
                        Log.d(TAG, MODULE + ": Starting at proxy in mode " + storedAtProxyMode);
                        AtProxy.setAtProxyMode(storedAtProxyMode);
                        AtProxy.setUsbConfigValue(TWO_ACM);
                    } else {
                        if (!usbConfValue.equals(TWO_ACM)) {
                            AtProxy.setUsbConfigValue(TWO_ACM);
                        }
                    }
                } else {
                    if (!atProxyMode.equals(AT_PROXY_OFF)) {
                        Log.d(TAG, MODULE + ": Stopping at proxy");
                        AtProxy.setAtProxyMode(AT_PROXY_OFF);
                        AtProxy.setUsbConfigValue(ONE_ACM);
                    } else {
                        if (!usbConfValue.equals(ONE_ACM)) {
                            AtProxy.setUsbConfigValue(ONE_ACM);
                        }
                    }
                }
            } else {
                if (!atProxyMode.equals(AT_PROXY_OFF)) {
                    Log.d(TAG, MODULE + ": Stopping at proxy");
                    AtProxy.setAtProxyMode(AT_PROXY_OFF);
                    AtProxy.setUsbConfigValue(NO_ACM);
                } else {
                    if (usbConfValue.contains("acm")) {
                        AtProxy.setUsbConfigValue(NO_ACM);
                    }
                }
            }
        }
        AlogMarker.tAE("ConfigManager.updateAtProxyConf", "0");
    }

    public static boolean updateAtProxyConf(ModemConf curModConf, int updatedIndex,
            boolean expModeOn) {
        AlogMarker.tAB("ConfigManager.updateAtProxyConf", "0");
        boolean logAndCtrl = AMTLApplication.getLogAndControl();
        String atProxyMode = AtProxy.getAtProxyMode();
        String usbConfValue = AtProxy.getUsbConfigValue();
        boolean usbLoggingConf = false;
        boolean atProxyOn = false;
        String mtsOutput = curModConf.getMtsConf().getOutput();

        Log.d(TAG, MODULE + ": at proxy mode = " + AtProxy.getAtProxyMode());
        Log.d(TAG, MODULE + ": usb config value = " + AtProxy.getUsbConfigValue());

        String storedAtProxyMode = getStoredSettings().getAtProxyMode();

        if (mtsOutput != null && mtsOutput.contains("ttyGS") && updatedIndex >= 0) {
            usbLoggingConf = true;
        }

        if (logAndCtrl) {
            if (!atProxyMode.equals(AT_PROXY_OFF)) {
                if (usbLoggingConf) {
                    if (!usbConfValue.equals(TWO_ACM)) {
                        AtProxy.setUsbConfigValue(TWO_ACM);
                    }
                    atProxyOn = false;
                } else {
                    if (!usbConfValue.equals(ONE_ACM)) {
                        AtProxy.setUsbConfigValue(ONE_ACM);
                    }
                    atProxyOn = true;
                }
            } else {
                if (usbLoggingConf) {
                    Log.d(TAG, MODULE + ": Starting at proxy in mode " + storedAtProxyMode);
                    AtProxy.setAtProxyMode(storedAtProxyMode);
                    if (!usbConfValue.equals(TWO_ACM)) {
                        AtProxy.setUsbConfigValue(TWO_ACM);
                    }
                    atProxyOn = false;
                } else {
                    atProxyOn = false;
                }
            }
        } else {
            if (!atProxyMode.equals(AT_PROXY_OFF)) {
                if (usbLoggingConf) {
                    Log.d(TAG, MODULE + ": Stopping at proxy");
                    AtProxy.setAtProxyMode(AT_PROXY_OFF);
                    AtProxy.setUsbConfigValue(ONE_ACM);
                    atProxyOn = false;
                } else {
                    if (!usbConfValue.equals(ONE_ACM)) {
                        AtProxy.setUsbConfigValue(ONE_ACM);
                    }
                    atProxyOn = true;
                }
            } else {
                if (usbLoggingConf) {
                    if (!usbConfValue.equals(ONE_ACM)) {
                        AtProxy.setUsbConfigValue(ONE_ACM);
                    }
                }
                atProxyOn = false;
            }
        }
        AlogMarker.tAE("ConfigManager.updateAtProxyConf", "0");
        return atProxyOn;
    }
}
