#include <wx/wx.h>
#include "ChessEngine.hpp"
#include "MainFrame.hpp"

ChessEngine::ChessEngine() : windows()
{
	Initialize();
}

ChessEngine::~ChessEngine()
{
	Shutdown();
}

void ChessEngine::CreateWindow(const std::string windowName)
{
	MainFrame* mainFrame = new MainFrame(windowName);
	mainFrame->SetClientSize(WIDTH, HEIGHT);
	mainFrame->Center();
	mainFrame->Show();
}

void ChessEngine::Initialize()
{
	CreateWindow("Chess Game");
}

void ChessEngine::Shutdown()
{
	for (MainFrame* window : windows)
	{
		delete window;
	}
	windows.clear();
}
