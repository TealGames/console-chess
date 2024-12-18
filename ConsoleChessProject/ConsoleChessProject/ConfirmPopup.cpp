#include <wx/wx.h>
#include <wx/overlay.h>
#include <string>
#include <format>
#include <functional>
#include "HelperFunctions.hpp"
#include "ConfirmPopup.hpp"
#include "CButton.hpp"
#include "Globals.hpp"
#include "UIGlobals.hpp"
#include "DirectionalLayout.hpp"
#include "Point2D.hpp"

static const float PANEL_SIZE_FACTOR = 0.5;
static const float TEXT_SIZE_FACTOR_Y = 0.6;
static const Utils::Point2D BUTTON_SIZE_FACTOR = Utils::Point2D(0.4, 0.5);

ConfirmPopup::ConfirmPopup(wxWindow* parent) : 
	m_root(nullptr), m_text(nullptr),
	m_confirmButton(nullptr), m_denyButton(nullptr),
	m_confirmCallback(nullptr), m_denyCallback(nullptr)
{
	/*m_root= new wxPanel(parent, wxID_ANY, wxDefaultPosition, 
		wxSize(PANEL_SIZE_FACTOR * WIDTH, PANEL_SIZE_FACTOR * HEIGHT));*/

	m_root = new wxDialog(parent, wxID_ANY, "Confirmation", wxDefaultPosition,
		wxSize(PANEL_SIZE_FACTOR * WIDTH, PANEL_SIZE_FACTOR * HEIGHT));
	m_root->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);

	const wxSize textSize = wxSize(m_root->GetSize().x, TEXT_SIZE_FACTOR_Y * m_root->GetSize().y);
	DirectionalLayout* verticalLayout = new DirectionalLayout(m_root, 
		LayoutType::Vertical, wxDefaultPosition, textSize);
	verticalLayout->SetBackgroundColour(BRIGHT_YELLOW);
	
	Utils::Log(Utils::LogType::Error, std::format("Text size: {}", WXUtils::ToString(textSize)));
	m_text = new wxStaticText(verticalLayout, wxID_ANY, "NULL", wxDefaultPosition, verticalLayout->GetSize(), wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	m_text->Wrap(0.8 * verticalLayout->GetSize().x);
	m_text->SetFont(HEADING_FONT);
	m_text->SetForegroundColour(MUTED_WHITE);
	//m_text->SetSize(textSize);

	m_text->SetBackgroundColour(BRIGHT_YELLOW);
	verticalLayout->AddChild(m_text, 0, SPACING_ALL_SIDES, 10);
	//m_text->CenterOnParent();
	Utils::Log(Utils::LogType::Error, std::format("Text size: {}", WXUtils::ToString(textSize)));
	//verticalLayout->AddChild(m_text, 0, SPACING_ALL_SIDES, 5);

	/*wxPanel* buttons = new wxPanel(m_root, wxID_ANY, wxDefaultPosition,
		wxSize(m_root->GetSize().x, (1 - TEXT_SIZE_FACTOR_Y) * m_root->GetSize().y));*/
	//verticalLayout->AddChild(buttons, 0);

	DirectionalLayout* horizontalLayout = new DirectionalLayout(m_root, LayoutType::Horizontal,
		wxDefaultPosition, wxSize(m_root->GetSize().x, (1-TEXT_SIZE_FACTOR_Y) * m_root->GetSize().y));
	horizontalLayout->SetBackgroundColour(RED);

	const wxSize buttonSize = wxSize(BUTTON_SIZE_FACTOR.m_X * horizontalLayout->GetSize().x, BUTTON_SIZE_FACTOR.m_Y * horizontalLayout->GetSize().y);
	m_confirmButton = new CButton(horizontalLayout, "NULL", wxDefaultPosition, buttonSize);
	m_denyButton = new CButton(horizontalLayout, "NULL", wxDefaultPosition, buttonSize);
	horizontalLayout->AddChild(m_confirmButton, 0, SPACING_ALL_SIDES, 10);
	horizontalLayout->AddChild(m_denyButton, 0, SPACING_ALL_SIDES, 10);
	
	horizontalLayout->SetPosition(wxPoint(wxDefaultPosition.x, wxDefaultPosition.y + verticalLayout->GetSize().y));
	//verticalLayout->CenterOnParent();

	m_confirmButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {ExcecuteConfirmAction(); });
	m_denyButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {ExecuteDenyAction(); });
}

void ConfirmPopup::SetButtonLabels(const ConfirmType& confirmType)
{
	if (m_confirmButton == nullptr || m_denyButton == nullptr)
	{
		const std::string err = std::format("Tried to set button labels "
			"to a confirm type but either the confirm or deny (or both) buttons are NULL!");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}

	if (confirmType == ConfirmType::ConfirmDeny)
	{
		m_confirmButton->SetLabel("Confirm");
		m_denyButton->SetLabel("Deny");
	}
	else if (confirmType == ConfirmType::YesNo)
	{
		m_confirmButton->SetLabel("Yes");
		m_denyButton->SetLabel("No");
	}
	else
	{
		const std::string err = std::format("Tried to set button labels "
			"to a confirm type with no actions defined");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}
}

void ConfirmPopup::ExcecuteConfirmAction()
{
	if (m_confirmCallback != nullptr) m_confirmCallback();
	Disable();
}

void ConfirmPopup::ExecuteDenyAction()
{
	if (m_denyCallback != nullptr) m_denyCallback();
	Disable();
}

void ConfirmPopup::Enable(const std::string& message, const ConfirmType& confirmType,
	const ConfirmCallbackType& confirmOption, const DenyCallbackType& denyOption)
{
	SetButtonLabels(confirmType);
	m_text->SetLabel(message);
	m_confirmCallback = confirmOption;
	m_denyCallback = denyOption;

	//Utils::Log(Utils::LogType::Error, "ENabling popup");
	/*m_root->Show();
	m_root->Raise();
	m_root->Refresh();*/
	m_root->CenterOnScreen();
	m_root->ShowModal();

	m_root->Refresh();
	/*wxMenu* menu = new wxMenu("HELLO");
	menu->AppendCheckItem(0, "MY NAME");*/

}

void ConfirmPopup::Disable()
{
	m_confirmCallback = nullptr;
	m_denyCallback = nullptr;

	/*m_root->Hide();
	m_root->Refresh();
	m_root->Layout();*/
	m_root->EndModal(wxID_CANCEL);
}
