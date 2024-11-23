#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/simplebook.h>
#include <functional>
#include "MainFrame.hpp"
#include "CapturedPiecesUI.hpp"
#include "CButton.hpp"
#include "Cell.hpp"
#include "BoardUI.hpp"
#include "Color.hpp"
#include "GameManager.hpp"
#include "GameState.hpp"

static const wxFont TITLE_FONT(30, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

static constexpr int TITLE_Y_OFFSET = 50;
static constexpr int BUTTON_START_Y = 150;
static constexpr int BUTTON_SPACING = 70;

static const wxSize SIDE_PANEL_SIZE(0.2*WIDTH, 0.8*HEIGHT);

MainFrame::MainFrame(GameManagement::GameManager& gameManager, const wxString& title)
	: wxFrame(nullptr, wxID_ANY, title), WindowName(title), manager(gameManager), _currentState(std::nullopt)
{
	DrawStatic();
	DrawMainMenu();
	DrawGame();

	TogglePage(Page::MainMenu);
}

void MainFrame::DrawStatic()
{
	this->SetBackgroundColour(wxColour(BACKGROUND_COLOR));
	wxStaticText* titleText = new wxStaticText(this, wxID_ANY, "CHESS", wxPoint(0, TITLE_Y_OFFSET),
		wxSize(WIDTH, 100), wxALIGN_CENTER_HORIZONTAL);
	titleText->SetForegroundColour(wxColour(248, 248, 248));

	titleText->SetFont(TITLE_FONT);
	_pages = new wxSimplebook(this, wxID_ANY, wxPoint(WIDTH/2, HEIGHT/2), wxSize(WIDTH, HEIGHT));
	//_pages->Center();

	//Sizer for root panel
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(titleText, 0, wxCENTER | wxTOP, 0);
	rootSizer->Add(_pages, 0, wxCENTER, 0);
	this->SetSizer(rootSizer);
}

void MainFrame::DrawMainMenu()
{
	wxPanel* mainMenuRoot = new wxPanel(_pages);
	//wxBoxSizer* mainMenuSizer = new wxBoxSizer(wxVERTICAL);

	CButton* newGameButton = new CButton(mainMenuRoot, "New Game");
	CenterX(newGameButton);
	newGameButton->MoveY(BUTTON_START_Y);
	newGameButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) -> void {StartGame(); });

	//mainMenuSizer->Add(newGameButton);

	CButton* rulesButton = new CButton(mainMenuRoot, "Rules");
	CenterX(rulesButton);
	rulesButton->MoveY(BUTTON_START_Y + BUTTON_SPACING);
	//mainMenuSizer->Add(rulesButton);

	CButton* settingsButton = new CButton(mainMenuRoot, "Settings");
	CenterX(settingsButton);
	settingsButton->MoveY(BUTTON_START_Y + 2 * BUTTON_SPACING);
	//mainMenuSizer->Add(mainMenuRoot);

	wxLogMessage("Draw Main Menu");
	//mainMenuRoot->SetSizer(mainMenuSizer);
	_pages->AddPage(mainMenuRoot, "MainMenu");
}

void MainFrame::DrawGame()
{
	wxPanel* gameRoot = new wxPanel(_pages);
	
	_cellParent = new wxPanel(gameRoot);
	CreateBoardCells(_cellParent);
	
	wxPanel* sidePanel = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, SIDE_PANEL_SIZE);
	sidePanel->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);
	//wxBoxSizer* sidePanelSizer = new wxBoxSizer(wxVERTICAL);
	//sidePanelSizer->Add(sidePanel, 0, wxTOP, 0);
	//sidePanelSizer->AddStretchSpacer(1);
	//sidePanelSizer->AddSpacer(50);
	//sidePanel->SetSizer(sidePanelSizer);
	CreateCaptureDisplay(sidePanel);

	//sidePanel->SetSize(SIDE_PANEL_SIZE);
	//sidePanel->SetPosition(wxPoint(500, 0));

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(_cellParent, 0, wxCENTER | wxTOP, 0);
	rootSizer->Add(sidePanel, 0, wxCENTER | wxLEFT, 20);
	gameRoot->SetSizer(rootSizer);

	_pages->AddPage(gameRoot, "Game");
}

void MainFrame::TogglePage(const Page& togglePage)
{
	int pageIndex = static_cast<int>(togglePage);
	_pages->ChangeSelection(pageIndex);
}

bool MainFrame::TryUpdateBoard()
{
	if (!_currentState.has_value())
	{
		const std::string err = std::format("Tried to update board display in main frame "
			"but there is no current game state!");
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}
	return TryRenderAllPieces(_currentState.value());
}

void MainFrame::StartGame()
{
	_currentState = manager.StartNewGame();
	if (!_currentState.has_value())
	{
		const std::string err = std::format("Tried to start game in main frame"
			" but there is no current game state!");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}
	//std::string str = "State of Game: "+_currentState.value().ToString();
	//Utils::Log(Utils::LogType::Warning, str);
	BindCellEventsForGameState(_currentState.value());
	TogglePage(Page::Game);

	//return;

	if (!TryUpdateBoard())
	{
		const std::string err = std::format("Tried to render all pieces but failed!");
		Utils::Log(Utils::LogType::Error, err);
	}
	
	//TODO: function listeners adding crashes app!
	/*std::function<void(ColorTheme)> turnChangeFunc = [this](const ColorTheme color) -> void 
		{
			TryUpdateBoard();
			UpdateInteractablePieces(color);
		};

	manager.TurnChangeEvent.AddListener(turnChangeFunc);
	manager.GameEndEvent.AddListener([this, &turnChangeFunc](const GameManagement::EndGameInfo info)-> void
		{
			manager.TurnChangeEvent.TryRemoveListener(turnChangeFunc);
		});*/
}

