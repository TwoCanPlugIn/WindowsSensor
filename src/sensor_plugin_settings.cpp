// Copyright(C) 2022 by Steven Adler
//
// This file is part of Windows Sensor Plugin for OpenCPN.
//
// Windows Sensor Plugin for OpenCPN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Windows Sensor Plugin for OpenCPN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Windows Sensor Plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//
// Project: Windows Sensor Plugin
// Description: Quick & Dirty Plugin to use Windows Location Services (sensor api)
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2022
// Version History: 
// 1.0 Initial Release
// 1.0.1 23/02/2024 Enable/Disable Debug logging, Fix GGA sentence when FixType not DGPS
// 1.1.0 01/03/2024 Generate different NMEA 0183 Sentences

// Note to self
// Good reference: https://docs.microsoft.com/en-us/windows/win32/sensorsapi/portal
// and https://docs.microsoft.com/en-us/windows/win32/sensorsapi/portal

// Future thoughts
// Support other sensors to generate XDR sentence for yaw, roll, pitch

#include "sensor_plugin_settings.h"

// Constructor and destructor implementation
Windows_Sensor_Plugin_Settings::Windows_Sensor_Plugin_Settings(wxWindow* parent) : Windows_Sensor_Plugin_Settings_Base(parent) {

	// Ensure the dialog is sized correctly	
	wxSize newSize = this->GetSize();
	Fit();

	// Populate the values of the dialog
	lblSensor->SetLabel(wxString::Format("GPS Sensor: %s", sensorName));
	// Note the order of the check list box elements
	// GGA
	chkListSentence->Check(0, isGGA);
	// GLL
	chkListSentence->Check(1, isGLL);
	// GSV
	chkListSentence->Check(2, isGSV);
	//RMC
	chkListSentence->Check(3, isRMC);
}

Windows_Sensor_Plugin_Settings::~Windows_Sensor_Plugin_Settings() {
	// Nothing to do in the destructor
}

void Windows_Sensor_Plugin_Settings::OnCheckSentence(wxCommandEvent& event) {
	// Nothing needs be done at this moment if the user check/unchecks an item
}

void Windows_Sensor_Plugin_Settings::OnRightClick(wxMouseEvent& event) { 
	// Allow the user to use right click to toggle all of the check list elements
	isToggled = !isToggled;
	for (unsigned int i = 0; i < chkListSentence->GetCount(); i++) {
		chkListSentence->Check(i, isToggled);
	}
}

void Windows_Sensor_Plugin_Settings::OnOK(wxCommandEvent& event) {
	// Note the order of the check list elements
	isGGA = chkListSentence->IsChecked(0);
	isGLL = chkListSentence->IsChecked(1);
	isGSV = chkListSentence->IsChecked(2);
	isRMC = chkListSentence->IsChecked(3);
	EndModal(wxID_OK);
}

void Windows_Sensor_Plugin_Settings::OnCancel(wxCommandEvent& event) {
	// Nothing to save
	EndModal(wxID_CANCEL);
}