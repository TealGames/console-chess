#include <wx/wx.h>
#include "App.hpp"
#include "MainFrame.hpp"
#include "ChessEngine.hpp"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	ChessEngine engine;
	return true;
}