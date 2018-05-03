/* Android AMTL
 *
 * Copyright (C) Intel 2014
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
 * Author: Damien Charpentier <damienx.charpentier@intel.com>
 * Author: Morgane Butscher <morganex.butscher@intel.com>
 */

package com.intel.amtl;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.LogOutput;
import com.intel.amtl.R;

import java.util.ArrayList;

public class AMTLApplication extends Application {
    private static Context ctx;
    private static boolean isCloseTtyEnable = true;
    private static boolean isPauseState = true;
    private static boolean hasModemChanged = false;
    private static LogOutput defaultConf = null;
    private static String modemInterface;
    private static int modConnectionId;
    private static boolean traceLegacy = false;
    private static String serviceToStart;
    private static boolean isAliasUsed = false;
    private static String atProxyTxt = "AT PROXY";
    private static boolean logAndControl = false;
    private static boolean modemRestart = true;
    private static boolean notifyDebug = false;
    private static boolean pRouteInfo = true;
    private static boolean coredumpGeneration = true;
    private static boolean isXsioDefined = false;
    private static ArrayList<String> modemNameList = new ArrayList<String>();
    private static AlogMarker m = new AlogMarker();
    private static Activity sActivity = null;

    public static Activity getActivity() {
        m.tAB("AMTLApplication.getActivity", "0");
        m.tAE("AMTLApplication.getActivity", "0");
        return sActivity;
    }

    public static void setActivity(Activity activity) {
        m.tAB("AMTLApplication.setActivity", "0");
        m.tAE("AMTLApplication.setActivity", "0");
        sActivity = activity;
    }

    public static Context getContext() {
        m.tAB("AMTLApplication.getContext", "0");
        m.tAE("AMTLApplication.getContext", "0");
        return ctx;
    }

    public static void setContext(Context context) {
        m.tAB("AMTLApplication.setContext", "0");
        ctx = context;
        m.tAE("AMTLApplication.setContext", "0");
    }

    public static boolean getCloseTtyEnable() {
        m.tAB("AMTLApplication.getCloseTtyEnanble", "0");
        m.tAE("AMTLApplication.getCloseTtyEnanble", "0");
        return isCloseTtyEnable;
    }

    public static void setCloseTtyEnable(boolean isEnable) {
        m.tAB("AMTLApplication.setCloseTtyEnanble", "0");
        isCloseTtyEnable = isEnable;
        m.tAE("AMTLApplication.setCloseTtyEnanble", "0");
    }

    public static boolean getPauseState() {
        m.tAB("AMTLApplication.getPauseState", "0");
        m.tAE("AMTLApplication.getPauseState", "0");
        return isPauseState;
    }

    public static void setPauseState(boolean bState) {
        m.tAB("AMTLApplication.setPauseState", "0");
        isPauseState = bState;
        m.tAE("AMTLApplication.setPauseState", "0");
    }

    public static boolean getModemChanged() {
        m.tAB("AMTLApplication.getModemChanged", "0");
        m.tAE("AMTLApplication.getModemChanged", "0");
        return hasModemChanged;
    }

    public static void setModemChanged(boolean change) {
        m.tAB("AMTLApplication.setModemChanged", "0");
        hasModemChanged = change;
        m.tAE("AMTLApplication.setModemChanged", "0");
    }

    public static LogOutput getDefaultConf() {
        m.tAB("AMTLApplication.getDefaultConf", "0");
        m.tAE("AMTLApplication.getDefaultConf", "0");
        return defaultConf;
    }

    public static void setDefaultConf(LogOutput defConf) {
        m.tAB("AMTLApplication.setDefaultConf", "0");
        defaultConf = defConf;
        m.tAE("AMTLApplication.setDefaultConf", "0");
    }

    public static void setModemConnectionId(String id) {
        m.tAB("AMTLApplication.setModemConnectionId", "0");
        modConnectionId = Integer.parseInt(id);
        m.tAE("AMTLApplication.setModemConnectionId", "0");
    }

    public static int getModemConnectionId() {
        m.tAB("AMTLApplication.getModemConnectionId", "0");
        m.tAE("AMTLApplication.getModemConnectionId", "0");
        return modConnectionId;
    }

    public static void setModemInterface(String modInterface) {
        m.tAB("AMTLApplication.setModemInterface", "0");
        modemInterface = modInterface;
        m.tAE("AMTLApplication.setModemInterface", "0");
    }

    public static String getModemInterface() {
        m.tAB("AMTLApplication.getModemInterface", "0");
        m.tAE("AMTLApplication.getModemInterface", "0");
        return modemInterface;
    }

    public static void setTraceLegacy(String legacy) {
        m.tAB("AMTLApplication.setTraceLegacy", "0");
        traceLegacy = (legacy.equals("true")) ? true : false;
        m.tAE("AMTLApplication.setTraceLegacy", "0");
    }

    public static boolean getTraceLegacy() {
        m.tAB("AMTLApplication.getTraceLegacy", "0");
        m.tAE("AMTLApplication.getTraceLegacy", "0");
        return traceLegacy;
    }

    public static void setServiceToStart(String service) {
        m.tAB("AMTLApplication.setServiceToStart", "0");
        serviceToStart = service;
        m.tAE("AMTLApplication.setServiceToStart", "0");
    }

    public static String getServiceToStart() {
        m.tAB("AMTLApplication.getServiceToStart", "0");
        m.tAE("AMTLApplication.getServiceToStart", "0");
        return serviceToStart;
    }

    public static void setAtProxyBtnTxt(String atProxy) {
        m.tAB("AMTLApplication.setAtProxyBtnTxt", "0");
        if (atProxy != null) {
            atProxyTxt = atProxy;
        }
        m.tAE("AMTLApplication.setAtProxyBtnTxt", "0");
    }

    public static String getAtProxyBtnTxt() {
        m.tAB("AMTLApplication.getAtProxyBtnTxt", "0");
        m.tAE("AMTLApplication.getAtProxyBtnTxt", "0");
        return atProxyTxt;
    }

    public static void setLogAndControl(String logAndCtrl) {
        m.tAB("AMTLApplication.setLogAndControl", "0");
        logAndControl = (logAndCtrl.equals("true")) ? true : false;
        m.tAE("AMTLApplication.setLogAndControl", "0");
    }

    public static boolean getLogAndControl() {
        m.tAB("AMTLApplication.getLogAndControl", "0");
        m.tAE("AMTLApplication.getLogAndControl", "0");
        return logAndControl;
    }

    public static void setModemRestart(String mdmRestart) {
        m.tAB("AMTLApplication.setModemRestart", "0");
        modemRestart = (mdmRestart.equals("true")) ? true : false;
        m.tAE("AMTLApplication.setModemRestart", "0");
    }

    public static boolean getModemRestart() {
        m.tAB("AMTLApplication.getModemRestart", "0");
        m.tAE("AMTLApplication.getModemRestart", "0");
        return modemRestart;
    }

    public static void setNotifyDebug(String notifyDbg) {
        m.tAB("AMTLApplication.setNotifyDebug", "0");
        notifyDebug = (notifyDbg.equals("true")) ? true : false;
        m.tAE("AMTLApplication.setNotifyDebug", "0");
    }

    public static boolean getNotifyDebug() {
        m.tAB("AMTLApplication.getNotifyDebug", "0");
        m.tAE("AMTLApplication.getNotifyDebug", "0");
        return notifyDebug;
    }

    public static void setPRouteInfo(String pRtInfo) {
        m.tAB("AMTLApplication.getPRouteInfo", "0");
        pRouteInfo = (pRtInfo.equals("true")) ? true : false;
        m.tAE("AMTLApplication.getPRouteInfo", "0");
    }

    public static boolean getPRouteInfo() {
        m.tAB("AMTLApplication.setPRouteInfo", "0");
        m.tAE("AMTLApplication.setPRouteInfo", "0");
        return pRouteInfo;
    }

    public static void setCoredumpGeneration(String cdGeneration) {
        m.tAB("AMTLApplication.setCoredumpGeneration", "0");
        coredumpGeneration = (cdGeneration.equals("true")) ? true : false;
        m.tAE("AMTLApplication.setCoredumpGeneration", "0");
    }

    public static boolean getCoredumpGeneration() {
        m.tAB("AMTLApplication.getCoredumpGeneration", "0");
        m.tAE("AMTLApplication.getCoredumpGeneration", "0");
        return coredumpGeneration;
    }

    public static void setModemNameList(ArrayList<String> modemNames) {
        m.tAB("AMTLApplication.setModemNameList", "0");
        modemNameList = modemNames;
        m.tAE("AMTLApplication.setModemNameList", "0");
    }

    public static ArrayList<String> getModemNameList() {
        m.tAB("AMTLApplication.getModemNameList", "0");
        m.tAE("AMTLApplication.getModemNameList", "0");
        return modemNameList;
    }

    public static void setIsAliasUsed(boolean alias) {
        m.tAB("AMTLApplication.setBpLoggingPath", "0");
        isAliasUsed = alias;
        m.tAE("AMTLApplication.setBpLoggingPath", "0");
    }

    public static boolean getIsAliasUsed() {
        m.tAB("AMTLApplication.getBpLoggingPath", "0");
        m.tAE("AMTLApplication.getBpLoggingPath", "0");
        return isAliasUsed;
    }

    public static void setXsioDefined(boolean xsio) {
        m.tAB("AMTLApplication.setXsioDefined", "0");
        isXsioDefined = xsio;
        m.tAE("AMTLApplication.setXsioDefined", "0");
    }

    public static boolean getXsioDefined() {
        m.tAB("AMTLApplication.getXsioDefined", "0");
        m.tAE("AMTLApplication.getXsioDefined", "0");
        return isXsioDefined;
    }

    public static String getCurrentModemName() {
        AlogMarker.tAB("AMTLApplication.getCurrentModemName", "0");
        ArrayList<String> modemNames = new ArrayList<String>();
        modemNames = AMTLApplication.getModemNameList();
        String curModemIndex = PreferenceManager
                .getDefaultSharedPreferences(AMTLApplication.getContext())
                .getString(AMTLApplication.getContext()
                .getString(R.string.settings_modem_name_key), "0");
        int readModemIndex = Integer.parseInt(curModemIndex);
        String modName = modemNames.get(readModemIndex);
        AlogMarker.tAE("AMTLApplication.getCurrentModemName", "0");
        return modName;
    }
}
