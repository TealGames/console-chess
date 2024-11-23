#include <string>
#include <wx/wx.h>
#include "CButton.hpp"

CButton::CButton(wxWindow* parent, const wxString& buttonText, const wxPoint& pos,
	const wxSize& size, const wxWindowID& id, const long& style) :
	wxButton(parent, id, buttonText, pos, size, style)
{
	SetBackgroundColour(LIGHT_GREEN);
	SetFont(BUTTON_FONT);
	SetOwnForegroundColour(MUTED_WHITE);

	Bind(wxEVT_ENTER_WINDOW, &CButton::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &CButton::OnExit, this);
}

void CButton::OnEnter(wxMouseEvent& event)
{
	CButton* button = dynamic_cast<CButton*>(event.GetEventObject());
	if (!button) return;

	button->SetBackgroundColour(DARKER_LIGHT_GREEN);
	button->SetOwnForegroundColour(DARKER_MUTED_WHITE);
	button->Refresh();
}

void CButton::OnExit(wxMouseEvent& event)
{
	CButton* button = dynamic_cast<CButton*>(event.GetEventObject());
	if (!button) return;

	button-> SetBackgroundColour(LIGHT_GREEN);
	button->SetOwnForegroundColour(MUTED_WHITE);
	button->Refresh();
}

void CButton::MoveX(const int newX)
{
	wxButton::Move(newX, GetPosition().y);
}

void CButton::MoveY(const int newY)
{
	wxButton::Move(GetPosition().x, newY);
}

wxPoint GetCenter(const wxSize& size)
{
	int centerX = WIDTH / 2 - size.x / 2;
	int centerY = HEIGHT / 2 - size.y/2;
	return { centerX, centerY };
}

void CenterX(CButton* button)
{
	button->Move(GetCenter(button->GetSize()));
}