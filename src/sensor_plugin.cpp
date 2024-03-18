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

#include "sensor_plugin.h"
#include "sensor_plugin_icons.h"

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
	// Settings determine what NMEA 183 sentences to generate
	configSettings = GetOCPNConfigObject();

	if (configSettings) {
		configSettings->SetPath(_T("/PlugIns/WindowsSensor"));
		configSettings->Read(_T("Verbose"), &isVerbose, 0);
		configSettings->Read(_T("GLL"), &isGLL, 1);
		configSettings->Read(_T("GGA"), &isGGA, 1);
		configSettings->Read(_T("GSV"), &isGSV, 1);
		configSettings->Read(_T("RMC"), &isRMC, 1);
	}

	// Initialize the Windows Sensor
	isRunning = InitializeSensor();

	// Fetch our position every second
	if (isRunning == true) {
		Start(1000, wxTIMER_CONTINUOUS);
	}

	// Notify OpenCPN what events we want to receive callbacks for
	return (WANTS_CONFIG | WANTS_PREFERENCES);
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

void Windows_Sensor_Plugin::ShowPreferencesDialog(wxWindow* parent) {

	settingsDialog = new Windows_Sensor_Plugin_Settings(parent);

	if (settingsDialog->ShowModal() == wxID_OK) {
		// Save the settings
		if (configSettings) {
			configSettings->SetPath(_T("/PlugIns/WindowsSensor"));
			configSettings->Write(_T("Verbose"), isVerbose);
			configSettings->Write(_T("GLL"), isGLL);
			configSettings->Write(_T("GGA"), isGGA);
			configSettings->Write(_T("GSV"), isGSV);
			configSettings->Write(_T("RMC"), isRMC);
		}
	}
		
	delete settingsDialog;
	settingsDialog = nullptr;
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

	// Iterate through the sensors, although we really just use the last one returned
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
		sensorName = wxString::FromUTF8(_bstr_t(name));
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

	ULONG keyCount = 0;
	keyList->GetCount(&keyCount);
	
	if ((hr != S_OK) || (keyCount == 0)) {
		wxLogMessage(_T("Windows Sensor Plugin, No data values."));
		return false;
	}

	for (unsigned int i = 0; i < keyCount; i++) {
		PROPERTYKEY sensorDataKey;
		keyList->GetAt(i, &sensorDataKey);

		// ignore values that are not of interest to us
		if (sensorDataKey.fmtid != SENSOR_DATA_TYPE_LOCATION_GUID)	{
			continue;
		}

		// Get the value
		PROPVARIANT sensorDataValue;
		PropVariantInit(&sensorDataValue);
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

		else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_USED_COUNT) {
			satellitesInUse = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_HORIZONAL_DILUTION_OF_PRECISION)	{
			hDOP = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_VERTICAL_DILUTION_OF_PRECISION) {
			vDOP = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_POSITION_DILUTION_OF_PRECISION) {
			pDOP = sensorDataValue.dblVal;
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
		
		else if (sensorDataKey == SENSOR_DATA_TYPE_TRUE_HEADING_DEGREES) {
			trueHeading = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_MAGNETIC_HEADING_DEGREES) {
			magneticHeading = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_MAGNETIC_VARIATION) {
			magneticVariation = sensorDataValue.dblVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_IN_VIEW) {
			satellitesInView = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_FIX_TYPE) {
			fixType = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_FIX_QUALITY) {
			fixQuality = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_GPS_SELECTION_MODE) {
			selectionMode = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_GPS_OPERATION_MODE) {
			operationMode = sensorDataValue.intVal;
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_GPS_STATUS) {
			fixStatus = sensorDataValue.intVal;
		}

		// This seems only to be filled in if the sensor consumes NMEA 0183 sentence
		// It also appears to be the last sentence received, so not of any real use
		else if (sensorDataKey == SENSOR_DATA_TYPE_NMEA_SENTENCE) {
			BSTR sentence = sensorDataValue.bstrVal;
			wxString nmeaSentence = wxString::FromUTF8(_bstr_t(sentence));
			if (isVerbose) {
				wxLogMessage(_T("Windows Sensor Plugin, NMEA Sentence: %s"), nmeaSentence);
			}
		}

		// The following are vector types
		// Refer to https://learn.microsoft.com/en-us/windows/win32/sensorsapi/retrieving-vector-types
		
		/*else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_AZIMUTH) {
			if ((VT_UI1 | VT_VECTOR) == V_VT(&sensorDataValue)) {
				double *dblValue = (double *)sensorDataValue.caub.pElems;
				for (unsigned int j = 0; j < satellitesInView; j++) {
					satellites.at(j).azimuth = *dblValue;
					dblValue++;
				}
			}
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_ELEVATION) {
			if ((VT_UI1 | VT_VECTOR) == V_VT(&sensorDataValue)) {
				double *dblValue = (double *)sensorDataValue.caub.pElems;
				for (unsigned int j = 0; j < satellitesInView; j++) {
					satellites.at(j).elevation = *dblValue;
					dblValue++;
				}
			}
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_STN_RATIO) {
			if ((VT_UI1 | VT_VECTOR) == V_VT(&sensorDataValue)) {
				double *dblValue = (double *)sensorDataValue.caub.pElems;
				for (unsigned int j = 0; j < satellitesInView; j++) {
					satellites.at(j).snr = *dblValue;
					dblValue++;
				}
			}
		}

		else if (sensorDataKey == SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_ID) {
			if ((VT_UI1 | VT_VECTOR) == V_VT(&sensorDataValue)) {
				int *intValue = (int *)sensorDataValue.caub.pElems;
				for (unsigned int j = 0; j < satellitesInView; j++) {
					satellites.at(j).id = *intValue;
					intValue++;
				}
			}
		}*/

		PropVariantClear(&sensorDataValue);
	}
	keyList->Release();
	return true;
}

// Obtain each satellite's id, azimuth, elevation, signal to noise ratio etc.
// Used to generate NMEA 0183 GSV sentences
void Windows_Sensor_Plugin::GetSatelliteInfo(const PROPERTYKEY key, std::vector<SatelliteInformation> &sats) {
	PROPVARIANT propertyValue;
	PropVariantInit(&propertyValue);
	ISensorDataReport *sensorData = NULL;
	HRESULT hr;
	hr = sensor->GetData(&sensorData);
	if (FAILED(hr) || !sensorData) {
		wprintf(L"Failed to get location sensor data\n");
		return;
	}
	hr = sensorData->GetSensorValue(key, &propertyValue);
	if (SUCCEEDED(hr)) {
		if ((VT_UI1 | VT_VECTOR) == V_VT(&propertyValue)) {

			// double variable to store SNR, Elevation, Azimuth
			double *element = (double *)propertyValue.caub.pElems;
			// integer variable to store Id
			unsigned int *id = (unsigned int *)propertyValue.caub.pElems;

			for (unsigned int i = 0; i < satellitesInView; i++) {
				if (IsEqualPropertyKey(key, SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_ID)) {
					sats.at(i).id = (unsigned int)*id;
					if (isVerbose) {
						wxLogMessage(_T("Windows Sensor Plugin, Satellite Id (%d): %d"), i, *id);
					}
				}
				if (IsEqualPropertyKey(key, SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_AZIMUTH)) {
					sats.at(i).azimuth = (int)*element;
					if (isVerbose) {
						wxLogMessage(_T("Windows Sensor Plugin, Satellite Azimuth (%d): %f"), i, *element);
					}
				}
				else if (IsEqualPropertyKey(key, SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_ELEVATION)) {
					sats.at(i).elevation = (int)*element;
					if (isVerbose) {
						wxLogMessage(_T("Windows Sensor Plugin, Satellite Elevation (%d): %f"), i, *element);
					}
				}
				else if (IsEqualPropertyKey(key, SENSOR_DATA_TYPE_SATELLITES_IN_VIEW_STN_RATIO)) {
					sats.at(i).snr = (int)*element;
					if (isVerbose) {
						wxLogMessage(_T("Windows Sensor Plugin, Satellite SNR (%d): %f"), i, *element);
					}
				}
				element++;
				id++;
			}
		}
	}
	else {
		wprintf(L"Failed to get property value\n");
	}
	PropVariantClear(&propertyValue);
}



// $--GGA, hhmmss.ss, llll.ll, a, yyyyy.yy, a, x, xx, x.x, x.x, M, x.x, M, x.x, xxxx*hh<CR><LF>
//                                             |  |   hdop         geoidal  age refID 
//                                             |  |        Alt
//                                             | sats
//                                           fix Qualty

// Generate the required NMEA 0183 sentences
void Windows_Sensor_Plugin::Notify() {
	if (GetData()) {

		double latitudeDegrees = trunc(latitude);
		double latitudeMinutes = (latitude - latitudeDegrees) * 60;

		double longitudeDegrees = trunc(longitude);
		double longitudeMinutes = (longitude - longitudeDegrees) * 60;

		wxDateTime tm = wxDateTime::Now();

		if (isGGA) {
			// $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
			wxString sentence;
			
			// Differential GPS has differential GPS Age and Reference Station Id values
			if (fixType == 2) {
				sentence = wxString::Format("$IIGGA,%s,%02.0f%07.4f,%c,%03.0f%07.4f,%c,%d,%d,%.2f,%.1f,M,%.1f,M,%.1f,%d", \
					tm.Format("%H%M%S").ToAscii(), fabs(latitudeDegrees), fabs(latitudeMinutes), latitudeDegrees >= 0 ? 'N' : 'S', \
					fabs(longitudeDegrees), fabs(longitudeMinutes), longitudeDegrees >= 0 ? 'E' : 'W', \
					fixType, satellitesInView, (double)hDOP, (double)altitude, \
					(double)geoidalSeparation, dgpsAge, dgpsReferenceId);
			}
			// Other Fix Types differential GPS Age and Reference Station Id values are NULL
			// BUG BUG Fix Type = 0 means no fix !
			else {
				sentence = wxString::Format("$IIGGA,%s,%02.0f%07.4f,%c,%03.0f%07.4f,%c,%d,%d,%.2f,%.1f,M,%.1f,M,,", \
					tm.Format("%H%M%S").ToAscii(), fabs(latitudeDegrees), fabs(latitudeMinutes), latitudeDegrees >= 0 ? 'N' : 'S', \
					fabs(longitudeDegrees), fabs(longitudeMinutes), longitudeDegrees >= 0 ? 'E' : 'W', \
					fixType, satellitesInView, (double)hDOP, (double)altitude, \
					(double)geoidalSeparation);
			}

			// Calculate & append checksum
			sentence.Trim();
			wxString checksum = ComputeChecksum(sentence);
			sentence.Append(wxT("*"));
			sentence.Append(checksum);
			sentence.Append(wxT("\r\n"));
			// Send to OpenCPN
			PushNMEABuffer(sentence);
			if (isVerbose) {
				wxLogMessage(_T("Windows Sensor Plugin, Generated sentence: %s"), sentence);
			}
		}
		if (isGLL) {
			// $--GLL,llll.ll,a,yyyyy.yy,a,hhmmss.ss,A,a*hh<CR><LF>
			wxString sentence;

			sentence = wxString::Format("$IIGLL,%02d%07.4f,%c,%03d%07.4f,%c,%s,%c,%c", abs(latitudeDegrees), fabs(latitudeMinutes), latitude >= 0 ? 'N' : 'S', \
				abs(longitudeDegrees), fabs(longitudeMinutes), longitude >= 0 ? 'E' : 'W', tm.Format("%H%M%S.00", wxDateTime::UTC).ToAscii(), 
				fixStatus == 1 ? 'A' : 'V', GpsSelectionMode.at(selectionMode));
			
			// Calculate & append checksum
			sentence.Trim();
			wxString checksum = ComputeChecksum(sentence);
			sentence.Append(wxT("*"));
			sentence.Append(checksum);
			sentence.Append(wxT("\r\n"));
			// Send to OpenCPN
			PushNMEABuffer(sentence);
			if (isVerbose) {
				wxLogMessage(_T("Windows Sensor Plugin, Generated sentence: %s"), sentence);
			}
		}
		if (isGSV) {
			// $--GSV,x,x,x,x,x,x,x,...*hh<CR><LF>
			//        | | | | | | |
			//        | | | | | | snr
			//        | | | | | azimuth
			//        | | | | elevation
			//        | | | satellite id
			// sentences| satellites in view
			//          sentence number
			wxString sentence;
			int totalSentences;
			totalSentences = trunc(satellitesInUse / 4) + ((satellitesInUse % 4) == 0 ? 0 : 1);
			int sentenceNumber;
			sentenceNumber = 1;

			for (int i = 0; i < satellitesInUse; i++) {
				sentence += wxString::Format("%02d,%02d,%03d,%02d", satellites.at(i).id,
					satellites.at(i).elevation, satellites.at(i).azimuth, satellites.at(i).snr);
				if ((((i + 1) % 4) == 0) || (((((i + 1) % 4) != 0)) && (i == (satellitesInUse - 1)))) {
					sentence.Prepend(wxString::Format("$GPGSV,%d,%d,%d,", totalSentences, sentenceNumber, satellitesInUse));
					// Calculate checksum
					sentence.Trim();
					wxString checksum = ComputeChecksum(sentence);
					sentence.Append(wxT("*"));
					sentence.Append(checksum);
					sentence.Append(wxT("\r\n"));
					// Send to OpenCPN
					PushNMEABuffer(sentence);
					if (isVerbose) {
						wxLogMessage(_T("Windows Sensor Plugin, Generated sentence: %s"), sentence);
					}
					sentence.Empty();
					sentenceNumber++;
				}
			}
		}

		if (isRMC) {
			// $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxxxx,x.x,a,a*hh<CR><LF>
			wxString sentence;

			sentence = wxString::Format("$IIRMC,%s,%c,%02.0f%07.4f,%c,%03.0f%07.4f,%c,%.2f,%.2f,%s,%.2f,%c,%c,", \
				tm.Format("%H%M%S").ToAscii(), fixStatus == 1 ? 'A' : 'V', fabs(latitudeDegrees), fabs(latitudeMinutes), latitudeDegrees >= 0 ? 'N' : 'S', \
				fabs(longitudeDegrees), fabs(longitudeMinutes), longitudeDegrees >= 0 ? 'E' : 'W', \
				speedOverGround , trueHeading, tm.Format("%d%m%y").ToAscii(), \
				fabs(magneticVariation),  magneticVariation >= 0 ? 'E' : 'W', GpsSelectionMode.at(selectionMode));


			// Calculate & append checksum
			sentence.Trim();
			wxString checksum = ComputeChecksum(sentence);
			sentence.Append(wxT("*"));
			sentence.Append(checksum);
			sentence.Append(wxT("\r\n"));
			// Send to OpenCPN
			PushNMEABuffer(sentence);
			if (isVerbose) {
				wxLogMessage(_T("Windows Sensor Plugin, Generated sentence: %s"), sentence);
			}
		}
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
