#pragma once
#include <string>
#include <wx/wx.h>
#include "UIGlobals.hpp"

const wxSize BUTTON_SIZE{ 200, 60 };
const wxFont BUTTON_FONT(18, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

class CButton : public wxButton
{
private:
	void OnEnter(wxMouseEvent& event);
	void OnExit(wxMouseEvent& event);
public:
	CButton(wxWindow* parent, const wxString& buttonText, const wxPoint& pos = wxPoint(0, 0),
		const wxSize& size = BUTTON_SIZE, const wxWindowID& id = wxID_ANY, const long& style = wxBORDER_NONE);

	void MoveX(const int newX);
	void MoveY(const int newY);
};

wxPoint GetCenter(const wxSize& size);
void CenterX(CButton* button);