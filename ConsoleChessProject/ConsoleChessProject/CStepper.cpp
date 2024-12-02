#include <wx/wx.h>
#include <functional>
#include <string>
#include <unordered_map>
#include "CStepper.hpp"
#include "CButton.hpp"
#include "DirectionalLayout.hpp"

const wxSize CStepper::m_BUTTON_SIZE = wxSize(20, 5);

CStepper::CStepper(wxWindow* parent, const std::vector<CStepperOption>& options,
	const wxPoint& pos, const wxSize& size, const wxWindowID& id, const long& style)
	: wxPanel(parent, id, pos, size, style), m_options(options), m_index(0)
{
	DirectionalLayout* layout = new DirectionalLayout(this, LayoutType::Horizontal, wxDefaultPosition, size);
	layout->SetBackgroundColour(TAN);
	m_prevButton = new CButton(layout, "PREV", wxDefaultPosition, m_BUTTON_SIZE);
	m_prevButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {PrevOption(); });

	m_nextButton = new CButton(layout, "NEXT", wxDefaultPosition, m_BUTTON_SIZE);
	m_nextButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {NextOption(); });

	m_text = new wxStaticText(layout, wxID_ANY, "NULL");

	layout->AddChild(m_prevButton, 0, SpacingType::Right, 10);
	layout->AddChild(m_text, 1);
	layout->AddChild(m_nextButton, 0, SpacingType::Left, 10);

	DisplayOption(0, true);
}

void CStepper::DisplayOption(const int& index, bool invokeCallback)
{
	if (index < 0 || index >= m_options.size()) return;
	const CStepperOption& option = m_options.at(index);

	m_text->SetLabel(option.Option);
	if (invokeCallback && option.Callback!=nullptr) 
		option.Callback(option.Option);
}

void CStepper::NextOption()
{
	m_index = m_index + 1 % m_options.size();
	DisplayOption(m_index, true);
}

void CStepper::PrevOption()
{
	m_index = m_index - 1 % m_options.size();
	DisplayOption(m_index, true);
}



