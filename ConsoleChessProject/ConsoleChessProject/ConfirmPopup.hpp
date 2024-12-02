#pragma once
#include <wx/wx.h>
#include <string>
#include <functional>
#include "CButton.hpp"

enum class ConfirmType
{
	YesNo,
	ConfirmDeny,
};

using ConfirmCallbackType = std::function<void()>;
using DenyCallbackType = std::function<void()>;
class ConfirmPopup
{
private:
	//static const wxSize BUTTON_SIZE;

	wxDialog* m_root;
	wxStaticText* m_text;
	CButton* m_confirmButton;
	CButton* m_denyButton;

	ConfirmCallbackType m_confirmCallback;
	DenyCallbackType m_denyCallback;

public:

private:
	void SetButtonLabels(const ConfirmType& confirmType);
	void ExcecuteConfirmAction();
	void ExecuteDenyAction();

public:
	ConfirmPopup(wxWindow* parent);

	void Enable(const std::string& message, const ConfirmType& confirmType, 
		const ConfirmCallbackType& confirmOption, const DenyCallbackType& denyOption);

	void Disable();
};

