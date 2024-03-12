///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Windows_Sensor_Plugin_Settings_Base
///////////////////////////////////////////////////////////////////////////////
class Windows_Sensor_Plugin_Settings_Base : public wxDialog
{
	private:

	protected:
		wxPanel* panelSettings;
		wxStaticText* lblSensor;
		wxCheckBox* checkVerbose;
		wxCheckListBox* chkListSentence;
		wxButton* btnOK;
		wxButton* btnCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnCheckSentence( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRightClick( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		Windows_Sensor_Plugin_Settings_Base( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Windows Sensor Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,300 ), long style = wxDEFAULT_DIALOG_STYLE );

		~Windows_Sensor_Plugin_Settings_Base();

};

