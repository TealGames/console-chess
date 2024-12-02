#pragma once
#include <wx/wx.h>
#include <functional>
#include <string>
#include <unordered_map>
#include "CButton.hpp"

using CStepperOptionCallbackType = std::function<void(const std::string&)>;
struct CStepperOption
{
	std::string Option;
	CStepperOptionCallbackType Callback;
};

class CStepper : public wxPanel
{
private:
	static const wxSize m_BUTTON_SIZE;
	const std::vector<CStepperOption> m_options;
	CButton* m_nextButton;
	CButton* m_prevButton;
	wxStaticText* m_text;

	int m_index;
public:

private:
	void DisplayOption(const int& index, bool invokeCallback);
	void NextOption();
	void PrevOption();

public:
	CStepper(wxWindow* parent, const std::vector<CStepperOption>& options,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		const wxWindowID& id = wxID_ANY, const long& style= 0);
};

