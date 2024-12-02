#pragma once
#include <string>
#include <wx/wx.h>
#include <functional>
#include <vector>
#include "UIGlobals.hpp"
#include "ResourceManager.hpp"

using ButtonClickCallbackType = std::function<void(wxCommandEvent&)>;
class CButton : public wxButton
{
private:
	static const wxSize m_BUTTON_SIZE;
	wxFont m_font;
	const AudioClip* m_hoverSound;
	const AudioClip* m_clickSound;

	std::vector<ButtonClickCallbackType> m_clickCallbacks;

	void Setup();
	void OnEnter(wxMouseEvent& event);
	void OnExit(wxMouseEvent& event);
	void OnClick(wxCommandEvent& event);

public:
	CButton(wxWindow* parent, const wxString& buttonText, const wxPoint& pos = wxPoint(0, 0),
		const wxSize& size = m_BUTTON_SIZE, const wxWindowID& id = wxID_ANY, const long& style = wxBORDER_NONE);

	CButton(wxWindow* parent, wxImage& image, const wxString& buttonText, const wxPoint& pos = wxPoint(0, 0),
		const wxSize& size = m_BUTTON_SIZE, const wxWindowID& id = wxID_ANY, const long& style = wxBORDER_NONE);

	void MoveX(const int newX);
	void MoveY(const int newY);
	void AddOnClickAction(const ButtonClickCallbackType& callback);
};

wxPoint GetCenter(const wxSize& size);
void CenterX(CButton* button);