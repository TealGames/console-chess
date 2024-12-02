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

static const float PANEL_SIZE_FACTOR = 0.5;
static const int TEXT_SIZE_FACTOR_Y = 0.8;
static const wxSize BUTTON_SIZE_FACTOR = wxSize(0.5, 0.8);

ConfirmPopup::ConfirmPopup(wxWindow* parent) : 
	m_root(nullptr), m_text(nullptr),
	m_confirmButton(nullptr), m_denyButton(nullptr),
	m_confirmCallback(nullptr), m_denyCallback(nullptr)
{
	/*m_root= new wxPanel(parent, wxID_ANY, wxDefaultPosition, 
		wxSize(PANEL_SIZE_FACTOR * WIDTH, PANEL_SIZE_FACTOR * HEIGHT));*/

	m_root = new wxDialog(parent, wxID_ANY, "Confirmation", wxDefaultPosition,
		wxSize(PANEL_SIZE_FACTOR * WIDTH, PANEL_SIZE_FACTOR * HEIGHT));
	m_root->SetBackgroundColour(DARKER_LIGHT_GREEN);

	DirectionalLayout* verticalLayout = new DirectionalLayout(m_root, 
		LayoutType::Vertical, wxDefaultPosition, m_root->GetSize());
	//verticalLayout->SetBackgroundColour(RED);
	
	const wxSize textSize = wxSize(verticalLayout->GetSize().x, TEXT_SIZE_FACTOR_Y* verticalLayout->GetSize().y);
	m_text = new wxStaticText(verticalLayout, wxID_ANY, "NULL", wxDefaultPosition, textSize, wxALIGN_CENTER_HORIZONTAL);
	m_text->SetFont(HEADING_FONT);
	m_text->SetBackgroundColour(RED);

	DirectionalLayout* horizontalLayout = new DirectionalLayout(verticalLayout, LayoutType::Horizontal, wxDefaultPosition,
		wxSize(verticalLayout->GetSize().x, (1 - TEXT_SIZE_FACTOR_Y) * verticalLayout->GetSize().y));

	m_confirmButton = new CButton(horizontalLayout, "NULL", wxDefaultPosition,
		wxSize(BUTTON_SIZE_FACTOR.x * horizontalLayout->GetSize().x, BUTTON_SIZE_FACTOR.y * horizontalLayout->GetSize().x));
	m_denyButton = new CButton(horizontalLayout, "NULL", wxDefaultPosition,
		wxSize(BUTTON_SIZE_FACTOR.x * horizontalLayout->GetSize().x, BUTTON_SIZE_FACTOR.y * horizontalLayout->GetSize().x));
	horizontalLayout->AddChild(m_confirmButton, 0, SPACING_ALL_SIDES, 10);
	horizontalLayout->AddChild(m_denyButton, 0, SPACING_ALL_SIDES, 10);

	verticalLayout->AddChild(m_text);
	verticalLayout->AddChild(horizontalLayout);

	m_confirmButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {ExcecuteConfirmAction(); });
	m_denyButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {ExecuteDenyAction(); });
}

void ConfirmPopup::SetButtonLabels(const ConfirmType& confirmType)
{
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

	wxMenu* menu = new wxMenu("HELLO");
	menu->AppendCheckItem(0, "MY NAME");

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
