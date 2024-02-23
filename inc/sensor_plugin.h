// Copyright(C) 2021 by Steven Adler
//
// This file is part of Windows Location Sensor plugin for OpenCPN.
//
// Windows Location Sensor plugin for OpenCPN is free software: you can redistribute it 
// and/or modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or  (at your option) 
// any later version.
//
// Windows Location Sensor plugin for OpenCPN is distributed in the hope that it will 
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Windows Location Sensor plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.
//


#ifndef WINDOWS_SENSOR_PLUGIN_H
#define WINDOWS_SENSOR_PLUGIN_H

// Windows COM and Sensor API
#define _WINSOCKAPI_
#include <comutil.h>
#include <sensorsapi.h>
#include <sensors.h>
#pragma comment(lib,"sensorsapi.lib")

// wxWidgets
// Pre compiled headers 
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif
#include <wx/timer.h>
#include <wx/string.h>
#include <wx/fileconf.h>

// Defines version numbers, names etc. for this plugin
#include "version.h"

// OpenCPN include file
#include "ocpn_plugin.h"

// The Windows Sensor plugin
class Windows_Sensor_Plugin : public opencpn_plugin_116, wxTimer {

public:
	// The constructor
	Windows_Sensor_Plugin(void *ppimgr);
	
	// and destructor
	~Windows_Sensor_Plugin(void);

	// Overridden OpenCPN plugin methods
	int Init(void);
	bool DeInit(void);
	int GetAPIVersionMajor();
	int GetAPIVersionMinor();
	int GetPlugInVersionMajor();
	int GetPlugInVersionMinor();
	wxString GetCommonName();
	wxString GetShortDescription();
	wxString GetLongDescription();
	wxBitmap *GetPlugInBitmap();
		
private: 
	
	// Plugin bitmap
	wxBitmap pluginBitmap;

	// Reference to the OpenCPN window handle
	wxWindow *parentWindow;

	// Overridden wxTimer method, used to fetch position from sensor
	void Notify();

	// NMEA 0183 Checksum
	wxString ComputeChecksum(wxString sentence);

	// Uses Windows Sensor API to find, initialize and fetch data from a location sensor
	bool InitializeSensor(void);
	bool GetData(void);

	// The GPS variables
	double latitude;
	double longitude;
	double speedOverGround;
	double courseOverGround;
	double trueHeading;
	double magneticHeading;
	double altitude;
	double hDOP;
	double geoidalSeparation;
	double dgpsAge;
	int dgpsReferenceId;
	int numberOfSatellites;
	int fixType;
	int fixQuality;

	// Windows Sensor COM interfaces
	ISensorManager *sensorManager;
	ISensor *sensor;

	// If sensor has been initialized
	bool isRunning;

	// Debug output
	bool isVerbose;
	
};

#endif 

