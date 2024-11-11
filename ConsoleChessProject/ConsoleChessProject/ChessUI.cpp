#include <wx/wx.h>
#include "ChessUI.hpp"
#include "MainFrame.hpp"

wxIMPLEMENT_APP(ChessUI);

bool ChessUI::OnInit()
{
	MainFrame* mainFrame = new MainFrame("C++ GUI");
	mainFrame->SetClientSize(WIDTH, HEIGHT);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}