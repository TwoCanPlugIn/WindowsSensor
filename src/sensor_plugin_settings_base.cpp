///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "sensor_plugin_settings_base.h"

///////////////////////////////////////////////////////////////////////////

Windows_Sensor_Plugin_Settings_Base::Windows_Sensor_Plugin_Settings_Base( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* sizerDialog;
	sizerDialog = new wxBoxSizer( wxVERTICAL );

	panelSettings = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizerPanelSettings;
	sizerPanelSettings = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sizerInterfaces;
	sizerInterfaces = new wxStaticBoxSizer( new wxStaticBox( panelSettings, wxID_ANY, wxT("Windows GPS Sensor") ), wxHORIZONTAL );

	lblSensor = new wxStaticText( sizerInterfaces->GetStaticBox(), wxID_ANY, wxT("GPS Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
	lblSensor->Wrap( -1 );
	sizerInterfaces->Add( lblSensor, 0, wxALL, 5 );


	sizerPanelSettings->Add( sizerInterfaces, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sizerVerbose;
	sizerVerbose = new wxStaticBoxSizer( new wxStaticBox( panelSettings, wxID_ANY, wxT("Debugging Options") ), wxVERTICAL );

	checkVerbose = new wxCheckBox( sizerVerbose->GetStaticBox(), wxID_ANY, wxT("Verbose Logging"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerVerbose->Add( checkVerbose, 0, wxALL, 5 );


	sizerPanelSettings->Add( sizerVerbose, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sizerSentences;
	sizerSentences = new wxStaticBoxSizer( new wxStaticBox( panelSettings, wxID_ANY, wxT("NMEA 0183 Position Sentences") ), wxHORIZONTAL );

	wxString chkListSentenceChoices[] = { wxT("GGA"), wxT("GLL"), wxT("GSV"), wxT("RMC") };
	int chkListSentenceNChoices = sizeof( chkListSentenceChoices ) / sizeof( wxString );
	chkListSentence = new wxCheckListBox( sizerSentences->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, chkListSentenceNChoices, chkListSentenceChoices, 0 );
	sizerSentences->Add( chkListSentence, 1, wxALL|wxEXPAND, 5 );


	sizerPanelSettings->Add( sizerSentences, 1, wxEXPAND, 5 );

	wxBoxSizer* sizerButtons;
	sizerButtons = new wxBoxSizer( wxHORIZONTAL );


	sizerButtons->Add( 0, 0, 1, wxEXPAND, 5 );

	btnOK = new wxButton( panelSettings, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( btnOK, 0, wxALL, 5 );

	btnCancel = new wxButton( panelSettings, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerButtons->Add( btnCancel, 0, wxALL, 5 );


	sizerPanelSettings->Add( sizerButtons, 0, wxEXPAND, 5 );


	panelSettings->SetSizer( sizerPanelSettings );
	panelSettings->Layout();
	sizerPanelSettings->Fit( panelSettings );
	sizerDialog->Add( panelSettings, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( sizerDialog );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	chkListSentence->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( Windows_Sensor_Plugin_Settings_Base::OnCheckSentence ), NULL, this );
	chkListSentence->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( Windows_Sensor_Plugin_Settings_Base::OnRightClick ), NULL, this );
	btnOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Windows_Sensor_Plugin_Settings_Base::OnOK ), NULL, this );
	btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Windows_Sensor_Plugin_Settings_Base::OnCancel ), NULL, this );
}

Windows_Sensor_Plugin_Settings_Base::~Windows_Sensor_Plugin_Settings_Base()
{
	// Disconnect Events
	chkListSentence->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( Windows_Sensor_Plugin_Settings_Base::OnCheckSentence ), NULL, this );
	chkListSentence->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( Windows_Sensor_Plugin_Settings_Base::OnRightClick ), NULL, this );
	btnOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Windows_Sensor_Plugin_Settings_Base::OnOK ), NULL, this );
	btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Windows_Sensor_Plugin_Settings_Base::OnCancel ), NULL, this );

}
