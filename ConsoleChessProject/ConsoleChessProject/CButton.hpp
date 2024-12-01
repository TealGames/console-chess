#pragma once
#include <string>
#include <wx/wx.h>
#include "UIGlobals.hpp"

class CButton : public wxButton
{
private:
	static const wxSize m_BUTTON_SIZE;
	wxFont m_font;

	void OnEnter(wxMouseEvent& event);
	void OnExit(wxMouseEvent& event);
public:
	CButton(wxWindow* parent, const wxString& buttonText, const wxPoint& pos = wxPoint(0, 0),
		const wxSize& size = m_BUTTON_SIZE, const wxWindowID& id = wxID_ANY, const long& style = wxBORDER_NONE);

	void MoveX(const int newX);
	void MoveY(const int newY);
};

wxPoint GetCenter(const wxSize& size);
void CenterX(CButton* button);