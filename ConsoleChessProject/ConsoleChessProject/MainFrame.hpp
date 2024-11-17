#pragma once
#include <vector>
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "UIGlobals.hpp"
#include "UIManager.hpp"

class MainFrame : public wxFrame
{
private:
	
public:
	const std::string WindowName;
	const UIManager& UIManager;

private:

public:
	MainFrame(const wxString& title);
};

