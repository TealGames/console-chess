#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/simplebook.h>
#include "MainFrame.hpp"
#include "CButton.hpp"
#include "Cell.hpp"
#include "BoardUI.hpp"
#include "Color.hpp"

MainFrame::MainFrame(const wxString& title)
	: wxFrame(nullptr, wxID_ANY, title), WindowName(title), UIManager(*this)
{

}
