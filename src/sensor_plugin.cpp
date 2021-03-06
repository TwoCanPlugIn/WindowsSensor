// Copyright(C) 2022 by Steven Adler
//
// This file is part of Windows Sensor Plugin for OpenCPN.
//
// NOAA Weather Plugin for OpenCPN is free software: you can redistribute it and/or modify
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


//
// Project: Windows Sensor Plugin
// Description: Quick & Dirty Plugin to use Windows Location Services (sensor api)
// Owner: twocanplugin@hotmail.com
// Date: 10/01/2022
// Version History: 
// 1.0 Initial Release
//

// Note to self
// Good reference: https://docs.microsoft.com/en-us/windows/win32/sensorsapi/portal
// and https://docs.microsoft.com/en-us/windows/win32/sensorsapi/portal

// Future thoughts
// Support other sensors to generate XDR sentence for yaw, roll, pitch

#include "sensor_plugin.h"
#include "sensor_plugin_icons.h"

// Globally accessible variables used by the plugin

// OpenCPN Configuration Setings
wxFileConfig *configSettings;

// The class factories, used to create and destroy instances of the PlugIn
extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
	return new Windows_Sensor_Plugin(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
	delete p;
}

Windows_Sensor_Plugin::Windows_Sensor_Plugin(void *ppimgr) : opencpn_plugin_116(ppimgr), wxTimer(this) {
	// Load the plugin bitmaps/icons 
	initialize_images();
}

Windows_Sensor_Plugin::~Windows_Sensor_Plugin(void) {
}

int Windows_Sensor_Plugin::Init(void) {
	// Maintain a reference to the OpenCPN window
	// Although we don't actually use it
	parentWindow = GetOCPNCanvasWindow();

	// Maintain a reference to the OpenCPN configuration object 
	// Although we don't actually use it
	configSettings = GetOCPNConfigObject();

	// Initialize the Windows Sensor
	isRunning = InitializeSensor();

	// Fetch our position every second
	if (isRunning == true) {
		Start(1000, wxTIMER_CONTINUOUS);
	}

	// Notify OpenCPN what events we want to receive callbacks for
	return (WANTS_CONFIG);
}

// OpenCPN is either closing down, or we have been disabled from the Preferences Dialog
bool Windows_Sensor_Plugin::DeInit(void) {
	// Stop our timer and cleanup
	if (isRunning == true) {
		Stop();
		sensor->Release();
		sensorManager = NULL;
		CoUninitialize();
	}
	isRunning = false;

	return true;
}

// Indicate what version of the OpenCPN Plugin API we support
int Windows_Sensor_Plugin::GetAPIVersionMajor() {
	return OCPN_API_VERSION_MAJOR;
}

int Windows_Sensor_Plugin::GetAPIVersionMinor() {
	return OCPN_API_VERSION_MINOR;
}

// The plugin version numbers. 
int Windows_Sensor_Plugin::GetPlugInVersionMajor() {
	return PLUGIN_VERSION_MAJOR;
}

int Windows_Sensor_Plugin::GetPlugInVersionMinor() {
	return PLUGIN_VERSION_MINOR;
}

// Return descriptions for the Plugin
wxString Windows_Sensor_Plugin::GetCommonName() {
	return _T(PLUGIN_COMMON_NAME);
}

wxString Windows_Sensor_Plugin::GetShortDescription() {
	return _T(PLUGIN_SHORT_DESCRIPTION);
}

wxString Windows_Sensor_Plugin::GetLongDescription() {
	return _T(PLUGIN_LONG_DESCRIPTION);
}

// 32x32 pixel PNG file, use pgn2wx.pl perl script
wxBitmap* Windows_Sensor_Plugin::GetPlugInBitmap() {
		return windowsSensorLogo;
}

bool Windows_Sensor_Plugin::InitializeSensor() {
	sensorManager = NULL;
	HRESULT hr;

	// Unnecessary as COM appears to be initialized by wxWidgets
	// Initialize the COM Object
	///hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	//if (hr != S_OK) {
	//	wxLogMessage(_T("Windows Sensor Plugin, COM Interface failed 0x%08lx"), hr);
	//	return false;
	//}

	// Create an intance of the Sensor COM object
	hr = CoCreateInstance(CLSID_SensorManager, 0, CLSCTX_ALL, __uuidof(ISensorManager), (void**)&sensorManager);

	if ((hr != S_OK) || (sensorManager == NULL)) {
		wxLogMessage(_T("Windows Sensor Plugin, Sensor Manager is not initializated."));
		CoUninitialize();
		return false;
	}
	wxLogMessage(_T("Windows Sensor Plugin, Sensor Manager initializated."));

	ISensorCollection *sensorList = NULL;

	// Specifically use a GPS device
	//hr = sensorManager->GetSensorsByCategory(SENSOR_CATEGORY_LOCATION, &sensorList);
	hr = sensorManager->GetSensorsByCategory(SENSOR_TYPE_LOCATION_GPS, &sensorList);
	if ((hr != S_OK) || (sensorList == NULL)) {
		wxLogMessage(_T("Windows Sensor Plugin, No GPS Sensors found"));
		CoUninitialize();
		return false;
	}

	// BUG BUG Probably unnecessary
	ULONG sensorsCount = 0;
	hr = sensorList->GetCount(&sensorsCount);
	if ((hr != S_OK) || (sensorsCount == 0)) {
		wxLogMessage(_T("Windows Sensor Plugin, GPS Sensor Count is zero"));
		sensorList->Release();
		CoUninitialize();
		return false;
	}

	wxLogMessage(_T("Windows Sensor Plugin, Found %i GPS sensor(s)"), sensorsCount);

	// Iterate through the sensors, althouh we really just use the last one returned
	for (unsigned int i = 0; i < sensorsCount; i++) {
		sensor = NULL;

		hr = sensorList->GetAt(i, &sensor);
		if ((hr != S_OK) || (sensor == NULL)) {
			continue;
		}

		BSTR name = NULL;
		hr = sensor->GetFriendlyName(&name);
		if ((hr != S_OK) || (name == NULL)) {
			continue;
		}

		// Convert the BSTR to a wxString
		wxString sensorName = wxString::FromUTF8(_bstr_t(name));
		wxLogMessage(_T("Windows Sensor Plugin, GPS Sensor: %i, Name %s"), i, sensorName);

		// Not really necessary, but useful for debugging purposes
		SENSOR_ID guid;
		hr = sensor->GetID(&guid);
		if (hr == S_OK) {

			wxLogMessage(_T("Windows Sensor Plugin: %08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"),
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		}

		// Not really necessary, but again, useful for debugging purposes
		SensorState state;
		hr = sensor->GetState(&state);
		if (hr == S_OK) {

			wxString sensorState = wxEmptyString;

			switch (state) {
				case SENSOR_STATE_READY || SENSOR_STATE_MIN:
					sensorState = "Ready";
					break;
				case SENSOR_STATE_NOT_AVAILABLE:
					sensorState = "Not Available";
					break;
				case SENSOR_STATE_INITIALIZING:
					sensorState = "Initializing";
					break;
				case SENSOR_STATE_ERROR:
					sensorState = "Error";
					break;
				case SENSOR_STATE_ACCESS_DENIED:
					sensorState = "Access Denied";
					break;
				case SENSOR_STATE_NO_DATA:
					sensorState = "No Data";
					break;
				default:
					sensorState = "Unknown";
					break;
			}

			wxLogMessage(_T("Windows Sensor Plugin, GPS sensor state: %s"), sensorState);
		}

	}
	return true;
}


bool Windows_Sensor_Plugin::GetData(void) {
	HRESULT hr = 0;
	SensorState state;

	hr = sensor->GetState(&state);
	if (hr != S_OK) {
		return false;
	}

	// No position fix yet....
	if ((state != SENSOR_STATE_READY) || (state != SENSOR_STATE_MIN)) {
		return false;
	}

	ISensorDataReport *sensorData = NULL;
	hr = sensor->GetData(&sensorData);
	
	if ((hr != S_OK) || (sensorData == NULL)) {
		return false;
	}

	// Iterate through the data values
	IPortableDeviceKeyCollection *keyList = NULL;
	hr = sensor->GetSupportedDataFields(&keyList);

	if ((hr != S_OK) || (keyList == NULL)) {
		return false;
	}

	ULONG keysCount = 0;
	keyList->GetCount(&keysCount);
	
	if ((hr != S_OK) || (keysCount == 0)) {
		wxLogMessage(_T("Windows Sensor Plugin, No data values."));
		return false;
	}

	for (unsigned int i = 0; i < keysCount; i++) {
		PROPERTYKEY sensorDataKey;
		keyList->GetAt(i, &sensorDataKey);

		// ignore values that are not of interest to us
		if (sensorDataKey.fmtid != SENSOR_DATA_TYPE_LOCATION_GUID)	{
			continue;
		}

		// Get the value
		PROPVARIANT sensorDataValue;
		sensorData->GetSensorValue(sensorDataKey, &sensorDataValue);

		if (sensorDataKey == SENSOR_DATA_TYPE_LATITUDE_DEGREES) {
			latitude = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_LONGITUDE_DEGREES) {
			longitude = sensorDataValue.dblVal;
		}
		

		else if (sensorDataKey == SENSOR_DATA_TYPE_ALTITUDE_ANTENNA_SEALEVEL_METERS) {
			altitude = sensorDataValue.dblVal;
		}
		
		else if (sensorDataKey == SENSOR_DATA_TYPE_SPEED_KNOTS)	{
			speedOverGround = sensorDataValue.dblVal;
		}
		
		else if (sensorDataKey == SENSOR_DATA_TYPE_HORIZONAL_DILUTION_OF_PRECISION)	{
			hDOP = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_GEOIDAL_SEPARATION) {
			geoidalSeparation = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_DGPS_DATA_AGE) {
			dgpsAge = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_DIFFERENTIAL_REFERENCE_STATION_ID) {
			dgpsReferenceId = sensorDataValue.intVal;
		}
		
		else if (sensorDataKey == SENSOR_DATA_TYPE_MAGNETIC_HEADING_DEGREES) {
			magneticHeading = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_TRUE_HEADING_DEGREES) {
			trueHeading = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_USED_COUNT) {
			numberOfSatellites = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_FIX_TYPE) {
			fixType = sensorDataValue.intVal;
		}
		
		else if (sensorDataKey == SENSOR_DATA_TYPE_FIX_QUALITY) {
			fixQuality = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_NMEA_SENTENCE) {
			BSTR sentence = sensorDataValue.bstrVal;
			wxString nmeaSentence = wxString::FromUTF8(_bstr_t(sentence));
			wxLogMessage(_T("Windows Sensor Plugin, NMEA Sentence: %s"), nmeaSentence);
		}

	}
	keyList->Release();
	return true;
}


// $--GGA, hhmmss.ss, llll.ll, a, yyyyy.yy, a, x, xx, x.x, x.x, M, x.x, M, x.x, xxxx*hh<CR><LF>
//                                             |  |   hdop         geoidal  age refID 
//                                             |  |        Alt
//                                             | sats
//                                           fix Qualty

// BUG BUG we could just use the sentence provided by the sensor !!
void Windows_Sensor_Plugin::Notify() {
	if (GetData()) {

		double latitudeDegrees = trunc(latitude);
		double latitudeMinutes = (latitude - latitudeDegrees) * 60;

		double longitudeDegrees = trunc(longitude);
		double longitudeMinutes = (longitude - longitudeDegrees) * 60;

		wxDateTime tm = wxDateTime::Now();

		// Generate the GGA sentence
		wxString sentence = wxString::Format("$IIGGA,%s,%02.0f%07.4f,%c,%03.0f%07.4f,%c,%d,%d,%.2f,%.1f,M,%.1f,M,%.1f,%d", \
			tm.Format("%H%M%S").ToAscii(), fabs(latitudeDegrees), fabs(latitudeMinutes), latitudeDegrees >= 0 ? 'N' : 'S', \
			fabs(longitudeDegrees), fabs(longitudeMinutes), longitudeDegrees >= 0 ? 'E' : 'W', \
			fixType, numberOfSatellites, (double)hDOP , (double)altitude, \
			(double)geoidalSeparation, dgpsAge, dgpsReferenceId);

		// Calculate & append checksum
		sentence.Trim();
		wxString checksum = ComputeChecksum(sentence);
		sentence.Append(wxT("*"));
		sentence.Append(checksum);
		sentence.Append(wxT("\r\n"));
		// Send to OpenCPN
		PushNMEABuffer(sentence);
		wxLogMessage(_T("Windows Sensor Plugin, Generated sentence: %s"), sentence);
	}
}


// Shamelessly copied from somewhere, another plugin ?
wxString Windows_Sensor_Plugin::ComputeChecksum(wxString sentence) {
	unsigned char calculatedChecksum = 0;
	for (wxString::const_iterator it = sentence.begin() + 1; it != sentence.end(); ++it) {
		calculatedChecksum ^= static_cast<unsigned char> (*it);
	}
	return(wxString::Format(wxT("%02X"), calculatedChecksum));
}
