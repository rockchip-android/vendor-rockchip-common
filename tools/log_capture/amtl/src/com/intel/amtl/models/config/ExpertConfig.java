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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.models.config;

import android.app.Activity;
import android.os.SystemProperties;
import android.util.Log;

import com.intel.amtl.config_parser.ConfigParser;
import com.intel.amtl.gui.UIHelper;
import com.intel.amtl.log.AlogMarker;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import org.xmlpull.v1.XmlPullParserException;

public class ExpertConfig {

    private final String TAG = "AMTL";
    private final String MODULE = "ExpertConfig";
    private static final String EXPERT_PROPERTY = "persist.service.amtl.exp";

    private String confPath = "/etc/telephony/amtl_default_expert.cfg";
    private static boolean configSet = false;
    private Activity activity = null;
    private File chosenFile = null;
    private ConfigParser modemConfParser = null;
    private ModemConf selectedConf = null;

    public ExpertConfig() {
        AlogMarker.tAB("ExpertConfig.ExpertConfig", "0");
        this.modemConfParser = new ConfigParser();
        AlogMarker.tAE("ExpertConfig.ExpertConfig", "0");
    }

    public ExpertConfig(Activity activity) {
        AlogMarker.tAB("ExpertConfig.ExpertConfig", "0");
        this.modemConfParser = new ConfigParser();
        this.activity = activity;
        AlogMarker.tAE("ExpertConfig.ExpertConfig", "0");
    }

    public String getPath() {
        AlogMarker.tAB("ExpertConfig.getPath", "0");
        AlogMarker.tAE("ExpertConfig.getPath", "0");
        return this.confPath;
    }

    public void setPath(String path) {
        AlogMarker.tAB("ExpertConfig.setPath", "0");
        this.confPath = path;
        AlogMarker.tAE("ExpertConfig.setPath", "0");
    }

    public static boolean isConfigSet() {
        AlogMarker.tAB("ExpertConfig.isConfigSet", "0");
        AlogMarker.tAE("ExpertConfig.isConfigSet", "0");
        return configSet;
    }

    public static void setConfigSet(boolean confSet) {
        AlogMarker.tAB("ExpertConfig.setConfigSet", "0");
        configSet = confSet;
        AlogMarker.tAE("ExpertConfig.setConfigSet", "0");
    }

    public ModemConf getExpertConf() {
        AlogMarker.tAB("ExpertConfig.getExpertConf", "0");
        AlogMarker.tAE("ExpertConfig.getExpertConf", "0");
        return this.selectedConf;
    }

    public void setExpertConf(ModemConf conf) {
        AlogMarker.tAB("ExpertConfig.setExpertConf", "0");
        this.selectedConf = conf;
        AlogMarker.tAE("ExpertConfig.setExpertConf", "0");
    }

    // retrieve the property value to know if expert mode is set
    public static boolean isExpertModeEnabled(String modemName) {
        AlogMarker.tAB("ExpertConfig.isExpertModeEnabled", "0");
        boolean ret = false;
        String expertMode = SystemProperties.get(EXPERT_PROPERTY + modemName, null);
        if (expertMode != null) {
            if (expertMode.equals("1")) {
                ret = true;
            }
        }
        AlogMarker.tAE("ExpertConfig.isExpertModeEnabled", "0");
        return ret;
    }

    public static void setExpertMode(String modemName, boolean enabled) {
        AlogMarker.tAB("ExpertConfig.setExpertMode", "0");
        if (enabled) {
            SystemProperties.set(EXPERT_PROPERTY + modemName, "1");
        } else {
            SystemProperties.set(EXPERT_PROPERTY + modemName, "0");
        }
        AlogMarker.tAE("ExpertConfig.setExpertMode", "0");
    }

    public String displayConf() {
        AlogMarker.tAB("ExpertConfig.displayConf", "0");
        String message = "";
        if (this.selectedConf != null) {
            message = selectedConf.getXsio();
            message += "\r\n";
            message += selectedConf.getXsystrace();
            message += "\r\n";
            message += selectedConf.getTrace();
        }
        AlogMarker.tAE("ExpertConfig.displayConf", "0");
        return message;
    }

    public boolean validateFile() {
        AlogMarker.tAB("ExpertConfig.validateFile", "0");
        Boolean bRet = false;
        this.chosenFile = this.checkConfPath(this.confPath);
        if (this.chosenFile != null && !this.chosenFile.isDirectory()) {
            this.selectedConf = this.applyConf(this.chosenFile);
            bRet = true;
        }
        AlogMarker.tAE("ExpertConfig.validateFile", "0");
        return bRet;
    }

    private File checkConfPath(String path) {
        AlogMarker.tAB("ExpertConfig.checkConfPath", "0");
        File ret = null;
        File file = new File(path);
        if (file.exists()) {
            ret = file;
            Log.d(TAG, MODULE + ": file: " + path + " has been chosen");
        } else {
            if (this.activity != null) {
                UIHelper.okDialog(activity, "Error", "file: " + path + " has not been found");
            }
            Log.e(TAG, MODULE + ": file: " + path + " has not been found");
        }
        AlogMarker.tAE("ExpertConfig.checkConfPath", "0");
        return ret;
    }

    private ModemConf applyConf(File conf) {
        AlogMarker.tAB("ExpertConfig.applyConf", "0");
        ModemConf ret = null;
        FileInputStream fin = null;
        if (conf != null) {
            try {
                fin = new FileInputStream(conf);
                if (fin != null) {
                    ret = modemConfParser.parseShortConfig(fin);
                    this.setConfigSet(true);
                }
            } catch (FileNotFoundException ex) {
                Log.e(TAG, MODULE + ": file " + conf + " has not been found");
                if (this.activity != null) {
                    UIHelper.okDialog(this.activity, "Error", ex.toString());
                }
            } catch (XmlPullParserException ex) {
                Log.e(TAG, MODULE + ": an issue occured during parsing " + ex);
                if (this.activity != null) {
                    UIHelper.okDialog(this.activity, "Error", ex.toString());
                }
            } catch (IOException ex) {
                Log.e(TAG, MODULE + ": an issue occured during parsing " + ex);
                if (this.activity != null) {
                    UIHelper.okDialog(this.activity, "Error", ex.toString());
                }
            } finally {
                if (fin != null) {
                    try {
                        fin.close();
                    } catch (IOException ex) {
                        Log.e(TAG, MODULE + ": Error during close " + ex);
                    }
                }
            }
        }
        AlogMarker.tAE("ExpertConfig.applyConf", "0");
        return ret;
    }
}
