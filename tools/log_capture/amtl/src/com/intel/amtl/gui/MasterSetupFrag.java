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

package com.intel.amtl.gui;

import android.app.Activity;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.LogOutput;
import com.intel.amtl.models.config.Master;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.modem.controller.ModemController;
import com.intel.amtl.R;

import java.util.ArrayList;


public class MasterSetupFrag extends Fragment
        implements OnClickListener, AdapterView.OnItemSelectedListener {

    private final String TAG = "AMTL";
    private final String MODULE = "MasterSetupFrag";

    private String[] masterValues = {"OFF", "OCT", "MIPI1", "MIPI2"};
    private String[] masterNames = {"bb_sw", "3g_sw", "digrfx", "digrfx2", "lte_l1_sw", "3g_dsp",
            "tdscdma_l1_sw", "sig_mon"};

    // Graphical objects for onClick handling.
    private Spinner spinnerBbSw;
    private Spinner spinner3gSw;
    private Spinner spinnerDigrfx;
    private Spinner spinnerDigrfx2;
    private Spinner spinnerLteL1Sw;
    private Spinner spinner3gDsp;
    private Spinner spinnerTdscdmaL1Sw;
    private Spinner spinnerSigMon;
    private Button bAppMasterConf;

    private ArrayList<Master> masterArray = null;
    private ArrayList<Spinner> spinnerArray = null;

    private boolean buttonChanged = false;
    private boolean firstClick = true;

    private ModemController mdmCtrl;

    // this counts how many spinners are on the UI
    private int spinnerCount = 8;

    // this counts how many spinners have been initialized
    private int spinnerInitializedCount = 0;

    private void setUIEnabled() {
        AlogMarker.tAB("MasterSetupFrag.setUIEnabled", "0");
        this.spinnerBbSw.setEnabled(true);
        this.spinner3gSw.setEnabled(true);
        this.spinnerDigrfx.setEnabled(true);
        this.spinnerDigrfx2.setEnabled(true);
        this.spinnerLteL1Sw.setEnabled(true);
        this.spinner3gDsp.setEnabled(true);
        this.spinnerTdscdmaL1Sw.setEnabled(true);
        this.spinnerSigMon.setEnabled(true);
        this.bAppMasterConf.setEnabled(true);
        this.changeButtonColor(this.buttonChanged);
        AlogMarker.tAE("MasterSetupFrag.setUIEnabled", "0");
    }

    private void setUIDisabled() {
        AlogMarker.tAB("MasterSetupFrag.setUIDisabled", "0");
        this.spinnerBbSw.setEnabled(false);
        this.spinner3gSw.setEnabled(false);
        this.spinnerDigrfx.setEnabled(false);
        this.spinnerDigrfx2.setEnabled(false);
        this.spinnerLteL1Sw.setEnabled(false);
        this.spinner3gDsp.setEnabled(false);
        this.spinnerTdscdmaL1Sw.setEnabled(false);
        this.spinnerSigMon.setEnabled(false);
        this.bAppMasterConf.setEnabled(false);
        this.changeButtonColor(false);
        AlogMarker.tAE("MasterSetupFrag.setUIDisabled", "0");
    }

    private void updateUi() {
        AlogMarker.tAB("MasterSetupFrag.updateUI", "0");
        if (this.masterArray != null) {
            for (Master m: masterArray) {
                Spinner spinToSet = this.spinnerArray.get(this.masterArray.lastIndexOf(m));
                ArrayAdapter aAdapt = (ArrayAdapter) spinToSet.getAdapter();
                spinToSet.setSelection(aAdapt.getPosition(m.getDefaultPort()));
            }
        }
        this.changeButtonColor(this.buttonChanged);
        AlogMarker.tAE("MasterSetupFrag.updateUI", "0");
    }

    private void checkConfig(ModemController modCtrl) throws ModemControlException {
        AlogMarker.tAB("MasterSetupFrag.checkConfig", "0");
        this.masterArray = modCtrl.checkAtXsystraceState(this.masterArray);
        this.updateUi();
        if (modCtrl.queryTraceState() && !AMTLApplication.getIsAliasUsed()) {
            this.setUIEnabled();
        } else {
            this.setUIDisabled();
        }
        AlogMarker.tAE("MasterSetupFrag.checkConfig", "0");
    }

    private void changeButtonColor(boolean changed) {
        AlogMarker.tAB("MasterSetupFrag.changeButtonColor", "0");
        if (changed) {
            this.bAppMasterConf.setBackgroundColor(Color.BLUE);
            this.bAppMasterConf.setTextColor(Color.WHITE);
        } else {
            this.bAppMasterConf.setBackgroundColor(Color.LTGRAY);
            this.bAppMasterConf.setTextColor(Color.BLACK);
        }
        AlogMarker.tAE("MasterSetupFrag.changeButtonColor", "0");
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        AlogMarker.tAB("MasterSetupFrag.onCreate", "0");
        super.onCreate(savedInstanceState);
        this.getActivity().registerReceiver(mMessageReceiver, new IntentFilter("modem-event"));
        AlogMarker.tAE("MasterSetupFrag.onCreate", "0");
    }

    @Override
    public void onDestroy() {
        AlogMarker.tAB("MasterSetupFrag.onDestroy", "0");
        this.firstClick = true;
        this.buttonChanged = false;
        this.spinnerCount = 8;
        this.spinnerInitializedCount = 0;
        this.getActivity().unregisterReceiver(mMessageReceiver);
        super.onDestroy();
        AlogMarker.tAE("MasterSetupFrag.onDestroy", "0");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        AlogMarker.tAB("MasterSetupFrag.onCreateView", "0");

        View view = inflater.inflate(R.layout.mastersetuplayout, container, false);

        bAppMasterConf = (Button) view.findViewById(R.id.applyMasterConfButton);
        spinnerBbSw = (Spinner) view.findViewById(R.id.spinner_bb_sw);
        spinner3gSw = (Spinner) view.findViewById(R.id.spinner_3g_sw);
        spinnerDigrfx = (Spinner) view.findViewById(R.id.spinner_digrfx);
        spinnerDigrfx2 = (Spinner) view.findViewById(R.id.spinner_digrfx2);
        spinnerLteL1Sw = (Spinner) view.findViewById(R.id.spinner_lte_l1_sw);
        spinner3gDsp = (Spinner) view.findViewById(R.id.spinner_3g_dsp);
        spinnerTdscdmaL1Sw = (Spinner) view.findViewById(R.id.spinner_tdscdma_l1_sw);
        spinnerSigMon = (Spinner) view.findViewById(R.id.spinner_sig_mon);

        this.spinnerArray = new ArrayList<Spinner>();
        this.spinnerArray.add(spinnerBbSw);
        this.spinnerArray.add(spinner3gSw);
        this.spinnerArray.add(spinnerDigrfx);
        this.spinnerArray.add(spinnerDigrfx2);
        this.spinnerArray.add(spinnerLteL1Sw);
        this.spinnerArray.add(spinner3gDsp);
        this.spinnerArray.add(spinnerTdscdmaL1Sw);
        this.spinnerArray.add(spinnerSigMon);

        ArrayAdapter<String> masterAdapter = new ArrayAdapter<String>(this.getActivity(),
                android.R.layout.simple_spinner_item, masterValues);
        masterAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        if (this.spinnerArray != null) {
            for (Spinner s: spinnerArray) {
                s.setAdapter(masterAdapter);
            }
        }
        AlogMarker.tAE("MasterSetupFrag.onCreateView", "0");
        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        AlogMarker.tAB("MasterSetupFrag.onViewCreated", "0");
        super.onViewCreated(view, savedInstanceState);

        if (spinnerArray != null) {
            for (Spinner s: spinnerArray) {
                s.setOnItemSelectedListener(this);
            }
        }
        if (this.bAppMasterConf != null) {
            this.bAppMasterConf.setOnClickListener(this);
        }
        AlogMarker.tAE("MasterSetupFrag.onViewCreated", "0");
    }

    @Override
    public void onResume() {
        AlogMarker.tAB("MasterSetupFrag.onResume", "0");
        super.onResume();
        if (!AMTLApplication.getModemChanged()) {
            // update modem status when returning from another fragment
            try {
                this.mdmCtrl = ModemController.getInstance();
                if (this.mdmCtrl.isModemUp()) {
                    this.masterArray = new ArrayList<Master>();
                    for (String s: masterNames) {
                        this.masterArray.add(new Master(s, "", ""));
                    }
                    this.checkConfig(this.mdmCtrl);
                } else {
                    this.setUIDisabled();
                }
            } catch (ModemControlException ex) {
                Log.e(TAG, MODULE + ": cannot send command to the modem");
                this.mdmCtrl = null;
            }
        }
        AlogMarker.tAE("MasterSetupFrag.onResume", "0");
    }

    @Override
    public void onPause() {
        AlogMarker.tAB("MasterSetupFrag.onPause", "0");
        this.mdmCtrl = null;
        this.firstClick = true;
        this.buttonChanged = false;
        this.spinnerCount = 1;
        this.spinnerInitializedCount = 0;
        super.onPause();
        AlogMarker.tAE("MasterSetupFrag.onPause", "0");
    }

    public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
        AlogMarker.tAB("MasterSetupFrag.onItemSelected", "0");
        if (this.firstClick) {
            if (spinnerInitializedCount < spinnerCount) {
                spinnerInitializedCount++;
            } else {
                this.buttonChanged = true;
                changeButtonColor(this.buttonChanged);
            }
        } else {
            this.buttonChanged = true;
            changeButtonColor(this.buttonChanged);
        }
        AlogMarker.tAE("MasterSetupFrag.onItemSelected", "0");
    }

    public void onNothingSelected(AdapterView<?> parent) {
    }

    @Override
    public void onClick(View view) {
        AlogMarker.tAB("MasterSetupFrag.onClick", "0");
        switch (view.getId()) {
            case R.id.applyMasterConfButton:
                this.firstClick = false;
                this.buttonChanged = false;
                this.changeButtonColor(this.buttonChanged);
                this.setChosenMasterValues();
                this.setMasterStringToInt();
                ModemConf sysConf = this.setModemConf();
                try {
                    if (this.mdmCtrl != null) {
                        this.mdmCtrl.sendCommand(sysConf.getXsystrace());
                        Toast toast = Toast.makeText(this.getActivity(),
                                "Masters have been updated", Toast.LENGTH_SHORT);
                        toast.show();
                        this.checkConfig(this.mdmCtrl);
                    }
                } catch (ModemControlException ex) {
                    Log.e(TAG, MODULE + ": fail to send command to the modem " + ex);
                }
                break;
        }
        AlogMarker.tAE("MasterSetupFrag.onClick", "0");
    }

    private ModemConf setModemConf() {
        AlogMarker.tAB("MasterSetupFrag.setModemConf", "0");
        SharedPreferences prefs = this.getActivity().getSharedPreferences("AMTLPrefsData",
                Context.MODE_PRIVATE);
        LogOutput output = new LogOutput();
        output.setIndex(prefs.getInt("index" + AMTLApplication.getCurrentModemName(), -2));
        if (masterArray != null) {
            for (Master m: masterArray) {
                output.addMasterToList(m.getName(), m);
            }
        }
        ModemConf modConf = ModemConf.getInstance(output);
        AlogMarker.tAE("MasterSetupFrag.setModemConf", "0");
        return modConf;
    }

    // Set the master port values according to the current spinner value
    private void setChosenMasterValues() {
        AlogMarker.tAB("MasterSetupFrag.setChosenMasterValues", "0");
        if (spinnerArray != null) {
            for (Spinner s: spinnerArray) {
                masterArray.get(spinnerArray.lastIndexOf(s))
                        .setDefaultPort((String)s.getSelectedItem());
            }
        }
        AlogMarker.tAE("MasterSetupFrag.setChosenMasterValues", "0");
    }

    // Set the master values from string to int
    private void setMasterStringToInt() {
        AlogMarker.tAB("MasterSetupFrag.setMasterStringToInt", "0");
        if (this.masterArray != null) {
            for (Master m: masterArray) {
                int portInt = convertMasterStringToInt(m.getDefaultPort());
                if (portInt != -1) {
                    m.setDefaultPort(String.valueOf(portInt));
                }
            }
        }
        AlogMarker.tAE("MasterSetupFrag.setMasterStringToInt", "0");
    }

    // Convert the master port from strings displayed in the spinners to the corresponding values
    // to send through xsystrace command
    private int convertMasterStringToInt(String masterString) {
        AlogMarker.tAB("MasterSetupFrag.convertMasterStringToInt", "0");
        int masterInt = -1;
        if (masterString.equals("OFF")) {
            masterInt = 0;
        } else if (masterString.equals("OCT")) {
            masterInt = 1;
        } else if (masterString.equals("MIPI1")) {
            masterInt = 2;
        } else if (masterString.equals("MIPI2")) {
            masterInt = 4;
        }
        AlogMarker.tAE("MasterSetupFrag.convertMasterStringToInt", "0");
        return masterInt;
    }

    private BroadcastReceiver mMessageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            AlogMarker.tAB("MasterSetupFrag.BroadcastReceiver.onReceive", "0");
            // Get extra data included in the Intent
            String message = intent.getStringExtra("message");
            if (message != null && message.equals("UP")) {
                try {
                    if (mdmCtrl != null) {
                        checkConfig(mdmCtrl);
                    }
                } catch (ModemControlException ex) {
                    Log.e(TAG, MODULE + ": cannot send command to the modem");
                }
            } else if (message != null && message.equals("DOWN")) {
                setUIDisabled();
            }
            // In case message is null, no action to take. This is an
            // undefined behavior
            AlogMarker.tAE("MasterSetupFrag.BroadcastReceiver.onReceive", "0");
        }
    };
}
