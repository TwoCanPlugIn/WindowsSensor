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

#include "sensor_plugin_settings.h"

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

// Global Variables (accessed by settings dialog)
// Debug output
bool isVerbose;

// NMEA 0183 Sentence Options
bool isGGA;
bool isGLL;
bool isGSV;
bool isRMC;

// The PC's GPS Sensor Name
wxString sensorName;


// Structure for aggregating satellites used for position fix
typedef struct _satellite_info {
	unsigned int id;
	double elevation;
	double azimuth;
	double snr;
} SatelliteInformation;

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
	void ShowPreferencesDialog(wxWindow* parent);
		
private: 
	
	// Plugin bitmap
	wxBitmap pluginBitmap;

	// Reference to the OpenCPN window handle
	wxWindow *parentWindow;

	// Overridden wxTimer method, used to fetch position from sensor
	void Notify();

	// OpenCPN Configuration Setings
	wxFileConfig *configSettings;

	// NMEA 0183 Checksum
	wxString ComputeChecksum(wxString sentence);

	// Uses Windows Sensor API to find, initialize and fetch data from a location sensor
	bool InitializeSensor(void);
	bool GetData(void);
	void GetSatelliteInfo(const PROPERTYKEY key, std::vector<SatelliteInformation> &sats);

	// Windows Sensor COM interfaces
	ISensorManager *sensorManager;
	ISensor *sensor;

	// The GPS variables
	double latitude;
	double longitude;
	double speedOverGround;
	double courseOverGround;
	double trueHeading;
	double magneticHeading;
	double magneticVariation;
	double altitude;
	double hDOP;
	double pDOP;
	double vDOP;
	double geoidalSeparation;
	double dgpsAge;
	unsigned int dgpsReferenceId;
	unsigned int satellitesInView;
	unsigned int satellitesInUse;
	std::vector<SatelliteInformation> satellites;
	unsigned int fixType;
	unsigned int fixQuality;
	unsigned int selectionMode;
	unsigned int operationMode;
	unsigned int fixStatus;

	// If sensor has been initialized
	bool isRunning;
	
	// Preferences Dialog
	Windows_Sensor_Plugin_Settings *settingsDialog;

	std::vector<char>GpsSelectionMode = {'A', 'D', 'E', 'M','S', 'N' };
	// SENSOR_DATA_TYPE_GPS_SELECTION_MODE
	// 0 = Autonomous.
	// 1 = DGPS.
	// 2 = Estimated(dead reckoned).
	// 3 = Manual input.
	// 4 = Simulator.
	// 5 = Data not valid.

	// Matches NMEA 0183
	// A = Autonomous mode
	// D = Differential mode
	// E = Estimated(dead reckoning) mode
	// M = Manual input mode	// S = Simulator mode	// N = Data not valid
	
};

#endif 

