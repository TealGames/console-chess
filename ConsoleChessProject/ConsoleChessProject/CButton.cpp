#include <string>
#include <wx/wx.h>
#include <functional>
#include <vector>
#include "CButton.hpp"
#include "ResourceManager.hpp"
#include "HelperFunctions.hpp"
#include "WXHelperFunctions.hpp"
#include "DirectionalLayout.hpp"
#include "UIGlobals.hpp"

const wxSize CButton::m_BUTTON_SIZE = wxSize{ 200, 60 };

void CButton::Setup()
{
	SetBackgroundColour(LIGHT_GREEN);
	SetFont(m_font);
	SetOwnForegroundColour(MUTED_WHITE);

	Bind(wxEVT_BUTTON, &CButton::OnClick, this);
	Bind(wxEVT_ENTER_WINDOW, &CButton::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &CButton::OnExit, this);
}

CButton::CButton(wxWindow* parent, const wxString& buttonText, const wxPoint& pos,
	const wxSize& size, const wxWindowID& id, const long& style) :
	wxButton(parent, id, buttonText, pos, size, style), m_clickCallbacks{},
	m_font(0.1 * size.x, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
	m_clickSound(TryGetSound(AudioClipType::ButtonClick)), m_hoverSound(TryGetSound(AudioClipType::ButtonHover))
{
	Setup();
}

CButton::CButton(wxWindow* parent, wxImage& image, const wxString& buttonText, const wxPoint& pos,
	const wxSize& size, const wxWindowID& id, const long& style) :
	wxButton(parent, id, "", pos, size, style), m_clickCallbacks{},
	m_font(0.1 * size.x, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
	m_clickSound(TryGetSound(AudioClipType::ButtonClick)), m_hoverSound(TryGetSound(AudioClipType::ButtonHover))
{
	DirectionalLayout* layout = new DirectionalLayout(this, LayoutType::Horizontal, wxDefaultPosition, GetSize());
	layout->Enable(false);

	wxSize textSize = wxSize(0, 0);
	wxStaticText* text = nullptr;
	if (!buttonText.empty())
	{
		text = new wxStaticText(layout, wxID_ANY, buttonText, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_VERTICAL);
		text->SetFont(m_font);
		//text->SetSize(wxSize(text->GetBestSize().x, layout->GetSize().y));

		layout->AddChild(text, 0);
		textSize = text->GetSize();
	}

	wxSize spaceLeft = wxSize(layout->GetSize().x - textSize.x, layout->GetSize().y);
	wxBitmap bitmap = ConvertToBitMap(image, spaceLeft);
	Utils::Log(Utils::LogType::Error, std::format("Creating button with size: {} with text size: {} space left: {} bitmap size: {}", 
		WXUtils::ToString(GetSize()), WXUtils::ToString(textSize), WXUtils::ToString(spaceLeft), WXUtils::ToString(bitmap.GetSize())));

	wxStaticBitmap* mapDisplay = new wxStaticBitmap(layout, wxID_ANY, bitmap, wxDefaultPosition, bitmap.GetSize());
	layout->AddChild(mapDisplay, 0, SpacingType::None, 0, 0);
	if (text!=nullptr) text->SetPosition(wxPoint(wxDefaultPosition.x, (layout->GetSize().y - text->GetSize().y) / 2));

	mapDisplay->Enable(false);
	Setup();
}

void CButton::OnEnter(wxMouseEvent& event)
{
	CButton* button = dynamic_cast<CButton*>(event.GetEventObject());
	if (!button) return;

	button->SetBackgroundColour(DARKER_LIGHT_GREEN);
	button->SetOwnForegroundColour(DARKER_MUTED_WHITE);
	button->Refresh();

	if (m_hoverSound != nullptr) m_hoverSound->TryPlaySound();
}

void CButton::OnExit(wxMouseEvent& event)
{
	CButton* button = dynamic_cast<CButton*>(event.GetEventObject());
	if (!button) return;

	button-> SetBackgroundColour(LIGHT_GREEN);
	button->SetOwnForegroundColour(MUTED_WHITE);
	button->Refresh();
}

void CButton::OnClick(wxCommandEvent& event)
{
	CButton* button = dynamic_cast<CButton*>(event.GetEventObject());
	if (!button) return;

	//Utils::Log(Utils::LogType::Error, "BUTON CLICKED");
	if (m_clickSound != nullptr) m_clickSound->TryPlaySound();
	
	if (m_clickCallbacks.empty()) return;
	for (const auto& callback : m_clickCallbacks) callback(event);
}

void CButton::AddOnClickAction(const ButtonClickCallbackType& callback)
{
	m_clickCallbacks.push_back(callback);
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