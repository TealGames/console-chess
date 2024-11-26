#include <wx/wx.h>
#include "App.hpp"
#include "MainFrame.hpp"
#include "ChessEngine.hpp"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	m_Engine.Initialize();
	return true;
}

int App::OnExit()
{
	m_Engine.Shutdown();
	return 0;
}