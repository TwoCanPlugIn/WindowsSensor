#ifndef WINDOWS_SENSOR_PLUGIN_SETTINGS_H
#define WINDOWS_SENSOR_PLUGIN_SETTINGS_H

// wxWidgets Precompiled Headers
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif

// The dialog base class from which we are derived
// Note wxFormBuilder used to generate UI
#include "sensor_plugin_settings_base.h"

// Global Values
extern bool isVerbose;
extern bool isGGA;
extern bool isGLL;
extern bool isGSV;
extern bool isRMC;
extern wxString sensorName;


class Windows_Sensor_Plugin_Settings : public Windows_Sensor_Plugin_Settings_Base {
	
public:
	Windows_Sensor_Plugin_Settings(wxWindow* parent);
	~Windows_Sensor_Plugin_Settings();
	
protected:
	//overridden methods from the base class
	void OnCheckSentence(wxCommandEvent& event);
	void OnRightClick(wxMouseEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	
private:
	bool isToggled;
};

#endif
