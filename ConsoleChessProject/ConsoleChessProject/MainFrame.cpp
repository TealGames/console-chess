#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/simplebook.h>
#include <functional>
#include <format>
#include <string>
#include "MainFrame.hpp"
#include "GameHUD.hpp"
#include "CButton.hpp"
#include "Cell.hpp"
#include "BoardUI.hpp"
#include "Color.hpp"
#include "GameManager.hpp"
#include "BoardManager.hpp"
#include "GameState.hpp"
#include "UIGlobals.hpp"
#include "DirectionalLayout.hpp"

static const std::string GAME_STATE_ID = "main_state";

static constexpr int TITLE_Y_OFFSET = 50;
static constexpr int BUTTON_START_Y = 150;
static constexpr int BUTTON_SPACING = 70;

static constexpr int SIDE_PANEL_WIDTH = 200;

MainFrame::MainFrame(Core::GameManager& gameManager, const wxString& title)
	: wxFrame(nullptr, wxID_ANY, title), WindowName(title), _manager(gameManager), _currentState(nullptr)
{
	DrawStatic();
	DrawMainMenu();
	DrawGame();

	TogglePage(Page::MainMenu);
	//std::cout << &_manager;
	/*wxLogMessage("Pointer value ADDRESS GAME MANAGER: %p", &_manager);
	Utils::Log(std::format("TOGGLE PAGE on main frame. Current game states: {}",
		std::to_string(_manager.TotalGameStatesCount())));*/
}

void MainFrame::DrawStatic()
{
	this->SetBackgroundColour(wxColour(BACKGROUND_COLOR));
	wxStaticText* titleText = new wxStaticText(this, wxID_ANY, "CHESS", wxPoint(0, TITLE_Y_OFFSET),
		wxSize(WIDTH, 50), wxALIGN_CENTER_HORIZONTAL);
	titleText->SetForegroundColour(wxColour(248, 248, 248));

	titleText->SetFont(TITLE_FONT);
	_pages = new wxSimplebook(this, wxID_ANY, wxPoint(WIDTH/2, HEIGHT/2), wxSize(WIDTH, HEIGHT));
	_pages->SetBackgroundColour(BRIGHT_YELLOW);
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

	//Utils::Log(std::format("DRAW MAIN MENU on main frame. Current game states: {}", std::to_string(_manager.TotalGameStatesCount())));
}

void MainFrame::DrawGame()
{
	DirectionalLayout* pageRoot = new DirectionalLayout(_pages, LayoutType::Vertical, wxDefaultPosition, _pages->GetSize());
	pageRoot->SetBackgroundColour(TAN);

	const float gameRootYFactor = 0.9;
	const wxSize gameRootSize = wxSize(pageRoot->GetSize().GetWidth(), gameRootYFactor * pageRoot->GetSize().GetHeight());
	DirectionalLayout* gameRoot = new DirectionalLayout(pageRoot, LayoutType::Horizontal,
		wxPoint(wxDefaultPosition.x, wxDefaultPosition.y - gameRootSize.y), gameRootSize);
	gameRoot->SetBackgroundColour(LIGHT_GREEN);

	wxPanel* winningChancePanelParent = new wxPanel(pageRoot, wxID_ANY, wxDefaultPosition,
		wxSize(pageRoot->GetSize().x, 0.05*pageRoot->GetSize().GetHeight()));
	CreateWinChanceDisplay(_manager, winningChancePanelParent);
	pageRoot->AddChild(winningChancePanelParent, 0);
	//winningChancePanelParent->SetBackgroundColour(LIGHT_DEEP_BLUE);

	
	wxSize cellAreaSize = 1.2 * BOARD_SIZE;
	_cellParent = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, cellAreaSize);
	CreateBoardCells(_cellParent);	
	_cellParent->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);
	//_cellParent->Center();
	
	wxPanel* sidePanel = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, wxSize(SIDE_PANEL_WIDTH, cellAreaSize.y));
	sidePanel->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);
	CreateCaptureDisplay(_manager, sidePanel);

	//sidePanel->SetSize(SIDE_PANEL_SIZE);
	//sidePanel->SetPosition(wxPoint(500, 0));

	/*wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(_cellParent, 0, wxCENTER | wxTOP, 0);
	rootSizer->Add(sidePanel, 0, wxCENTER | wxLEFT, 20);
	gameRoot->SetSizer(rootSizer);*/

	pageRoot->AddChild(gameRoot, 0);
	gameRoot->AddChild(_cellParent, 0, SpacingType::Left, 100);
	gameRoot->AddChild(sidePanel, 0, SpacingType::Left | SpacingType::Right , 20);
	

	//gameRoot->CenterOnParent();

	_pages->AddPage(pageRoot, "Game");
}

void MainFrame::TogglePage(const Page& togglePage)
{
	int pageIndex = static_cast<int>(togglePage);
	_pages->ChangeSelection(pageIndex);
}

bool MainFrame::TryUpdateBoard()
{
	if (_currentState==nullptr)
	{
		const std::string err = std::format("Tried to update board display in main frame "
			"but there is no current game state!");
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}

	const std::string message = std::format("A total of pieces UPDATE BOARSD: {}", std::to_string(_currentState->InPlayPieces.size()));
	Utils::Log(message);
	return TryRenderAllPieces(_manager, *_currentState);
}

void MainFrame::StartGame()
{
	wxLogMessage("Pointer value ADDRESS GAME MANAGER LATER: %p", &_manager);
	Utils::Log(std::format("Start game on main frame. Current game states: {}", std::to_string(_manager.TotalGameStatesCount())));
	_currentState = &(_manager.StartNewGame(GAME_STATE_ID));
	
	if (_currentState==nullptr)
	{
		const std::string err = std::format("Tried to start game in main frame"
			" but there is no current game state!");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}
	//std::string str = "State of Game: "+_currentState.value().ToString();
	//Utils::Log(Utils::LogType::Warning, str);
	BindCellEventsForGameState(_manager, GAME_STATE_ID);
	TogglePage(Page::Game);
	const std::string message = std::format("A total of pieces START GAME: {}", std::to_string(_currentState->InPlayPieces.size()));
	
	if (!TryUpdateBoard())
	{
		const std::string err = std::format("Tried to render all pieces but failed!");
		Utils::Log(Utils::LogType::Error, err);
	}
	UpdateInteractablePieces(_currentState->CurrentPlayer);
	
	
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

