#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/simplebook.h>
#include <wx/popupwin.h>
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
#include "ThemeControllerUI.hpp"
#include "ConfirmPopup.hpp"

static const std::string GAME_STATE_ID = "main_state";

static constexpr int TITLE_Y_OFFSET = 50;
static constexpr int BUTTON_START_Y = 150;
static constexpr int BUTTON_SPACING = 70;

//static constexpr int LEFT_SIDE_PANEL_WIDTH = 250;

MainFrame::MainFrame(Core::GameManager& gameManager, const wxString& title)
	: wxFrame(nullptr, wxID_ANY, title), WindowName(title), m_manager(gameManager), 
	m_currentState(nullptr), m_popup(this)
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
	m_pages = new wxSimplebook(this, wxID_ANY, wxPoint(WIDTH/2, HEIGHT/2), wxSize(WIDTH, HEIGHT));
	m_pages->SetBackgroundColour(BRIGHT_YELLOW);
	//_pages->Center();

	//Sizer for root panel
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(titleText, 0, wxCENTER | wxTOP, 0);
	rootSizer->Add(m_pages, 0, wxCENTER, 0);
	this->SetSizer(rootSizer);
}

void MainFrame::DrawMainMenu()
{
	wxPanel* mainMenuRoot = new wxPanel(m_pages);
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
	m_pages->AddPage(mainMenuRoot, "MainMenu");

	//Utils::Log(std::format("DRAW MAIN MENU on main frame. Current game states: {}", std::to_string(_manager.TotalGameStatesCount())));
}

void MainFrame::DrawGame()
{
	DirectionalLayout* pageRoot = new DirectionalLayout(m_pages, LayoutType::Vertical, wxDefaultPosition, m_pages->GetSize());
	pageRoot->SetBackgroundColour(TAN);

	const float gameRootYFactor = 0.9;
	const wxSize gameRootSize = wxSize(pageRoot->GetSize().GetWidth(), gameRootYFactor * pageRoot->GetSize().GetHeight());
	DirectionalLayout* gameRoot = new DirectionalLayout(pageRoot, LayoutType::Horizontal,
		wxPoint(wxDefaultPosition.x, wxDefaultPosition.y - gameRootSize.y), gameRootSize);
	gameRoot->SetBackgroundColour(LIGHT_GREEN);

	wxPanel* winningChancePanelParent = new wxPanel(pageRoot, wxID_ANY, wxDefaultPosition,
		wxSize(pageRoot->GetSize().x, 0.05*pageRoot->GetSize().GetHeight()));
	CreateWinChanceDisplay(m_manager, winningChancePanelParent);
	pageRoot->AddChild(winningChancePanelParent, 0);
	//winningChancePanelParent->SetBackgroundColour(LIGHT_DEEP_BLUE);

	const wxSize cellAreaSize = 1.1 * BOARD_SIZE;
	const wxSize rightPanelSize = wxSize(0.5 * (WIDTH - BOARD_SIZE.x), cellAreaSize.y);
	const wxSize leftPanelSize = wxSize(0.35*(WIDTH- BOARD_SIZE.x), cellAreaSize.y);
	float margins = (WIDTH - cellAreaSize.x - rightPanelSize.x - leftPanelSize.x) / 2;
	//_cellParent->Center();

	//wxPanel* leftSidePanel = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, wxSize(0.5 * LEFT_SIDE_PANEL_WIDTH, cellAreaSize.y));
	wxPanel* leftSidePanel = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, leftPanelSize);
	leftSidePanel->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);

	DirectionalLayout* leftLayout = new DirectionalLayout(leftSidePanel, LayoutType::Vertical, wxDefaultPosition, leftSidePanel->GetSize());
	std::optional<wxImage> quitIcon = TryGetImageForIcon(SpriteSymbolType::QuitIcon);
	CButton* quitButton = new CButton(leftLayout, quitIcon.value(), "Quit", wxDefaultPosition, wxSize(leftSidePanel->GetSize().x, 30));

	//TODO: when popups like this appear all other buttons should ideally be made uninteractable
	quitButton->AddOnClickAction([this](wxCommandEvent& evt) -> void 
		{
			//Utils::Log(Utils::LogType::Error, "CLICK QUIT");
			m_popup.Enable("Hello", ConfirmType::ConfirmDeny, nullptr, nullptr); 

			/*wxPopupWindow* popup = new wxPopupWindow(this, wxPU_CONTAINS_CONTROLS);
			popup->SetSize(200, 150);
			popup->Show(true);*/
		});

	leftLayout->AddChild(quitButton, 0, SPACING_ALL_SIDES, 10);
	/*CreateThemeController(leftSidePanel);*/

	m_cellParent = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, cellAreaSize);
	CreateBoardCells(m_cellParent);
	m_cellParent->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);
	
	wxPanel* rightSidePanel = new wxPanel(gameRoot, wxID_ANY, wxDefaultPosition, rightPanelSize);
	rightSidePanel->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);
	CreateCaptureDisplay(m_manager, rightSidePanel);
	

	//sidePanel->SetSize(SIDE_PANEL_SIZE);
	//sidePanel->SetPosition(wxPoint(500, 0));

	/*wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(_cellParent, 0, wxCENTER | wxTOP, 0);
	rootSizer->Add(sidePanel, 0, wxCENTER | wxLEFT, 20);
	gameRoot->SetSizer(rootSizer);*/

	pageRoot->AddChild(gameRoot, 0);
	gameRoot->AddChild(leftSidePanel, 0, SpacingType::Right, margins);
	//gameRoot->AddChild(_cellParent, 0, SpacingType::Left, 130);
	gameRoot->AddChild(m_cellParent, 0);
	gameRoot->AddChild(rightSidePanel, 0, SpacingType::Left, margins);


	//gameRoot->CenterOnParent();

	m_pages->AddPage(pageRoot, "Game");
}

void MainFrame::TogglePage(const Page& togglePage)
{
	int pageIndex = static_cast<int>(togglePage);
	m_pages->ChangeSelection(pageIndex);
}

bool MainFrame::TryUpdateBoard()
{
	if (m_currentState==nullptr)
	{
		const std::string err = std::format("Tried to update board display in main frame "
			"but there is no current game state!");
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}

	const std::string message = std::format("A total of pieces UPDATE BOARSD: {}", std::to_string(m_currentState->InPlayPieces.size()));
	Utils::Log(message);
	return TryRenderAllPieces(m_manager, *m_currentState);
}

void MainFrame::StartGame()
{
	wxLogMessage("Pointer value ADDRESS GAME MANAGER LATER: %p", &m_manager);
	Utils::Log(std::format("Start game on main frame. Current game states: {}", std::to_string(m_manager.TotalGameStatesCount())));
	m_currentState = &(m_manager.StartNewGame(GAME_STATE_ID));
	
	if (m_currentState==nullptr)
	{
		const std::string err = std::format("Tried to start game in main frame"
			" but there is no current game state!");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}
	//std::string str = "State of Game: "+_currentState.value().ToString();
	//Utils::Log(Utils::LogType::Warning, str);
	BindCellEventsForGameState(m_manager, GAME_STATE_ID);
	TogglePage(Page::Game);
	const std::string message = std::format("A total of pieces START GAME: {}", std::to_string(m_currentState->InPlayPieces.size()));
	
	if (!TryUpdateBoard())
	{
		const std::string err = std::format("Tried to render all pieces but failed!");
		Utils::Log(Utils::LogType::Error, err);
	}
	UpdateInteractablePieces(m_currentState->CurrentPlayer);
	
	
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

