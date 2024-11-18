#include <wx/wx.h>
#include "ChessEngine.hpp"
#include "MainFrame.hpp"

ChessEngine::ChessEngine() : m_Windows(), GameManager()
{
	Initialize();
}

ChessEngine::~ChessEngine()
{
	Shutdown();
}

void ChessEngine::CreateWindow(const std::string windowName)
{
	MainFrame* mainFrame = new MainFrame(GameManager, windowName);
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
	for (MainFrame* window : m_Windows)
	{
		delete window;
	}
	m_Windows.clear();
}
