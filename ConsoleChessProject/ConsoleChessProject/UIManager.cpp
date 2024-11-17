#include <wx/wx.h>
#include <format>
#include <string>
#include "UIManager.hpp"
#include "GameManager.hpp"
#include "CButton.hpp"
#include "BoardUI.hpp"

static const wxFont TITLE_FONT(30, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

static constexpr int TITLE_Y_OFFSET = 50;
static constexpr int BUTTON_START_Y = 150;
static constexpr int BUTTON_SPACING = 70;

UIManager::UIManager(MainFrame* root) : RootFrame(root), _pages(nullptr)
{
	DrawStatic();
	DrawMainMenu();
	DrawGame();

	TogglePage(Page::MainMenu);
}

void UIManager::DrawStatic()
{
	RootFrame->SetBackgroundColour(wxColour(DARK_GRAY));
	wxStaticText* titleText = new wxStaticText(RootFrame, wxID_ANY, "CHESS", wxPoint(0, TITLE_Y_OFFSET), 
		wxSize(WIDTH, 100), wxALIGN_CENTER_HORIZONTAL);
	titleText->SetForegroundColour(wxColour(248, 248, 248));

	titleText->SetFont(TITLE_FONT);
	_pages = new wxSimplebook(RootFrame, wxID_ANY);

	//Sizer for root panel
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(titleText, 0, wxALIGN_CENTER | wxTOP, 0);
	rootSizer->Add(_pages, 0, wxALIGN_CENTER, 0);
	RootFrame->SetSizer(rootSizer);
}

void UIManager::DrawMainMenu()
{
	wxPanel* mainMenuRoot = new wxPanel(_pages);
	//wxBoxSizer* mainMenuSizer = new wxBoxSizer(wxVERTICAL);

	CButton* newGameButton = new CButton(mainMenuRoot, "New Game");
	CenterX(newGameButton);
	newGameButton->MoveY(BUTTON_START_Y);
	newGameButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt)
		{
			GameManagement::StartNewGame();
			TogglePage(Page::Game);
			CreateBoardCells(cellParent);
			if (!TryRenderAllPieces())
			{
				const std::string err = std::format("Tried to render all pieces but failed!");
				Utils::Log(Utils::LogType::Error, err);
			}
			wxLogMessage("TOGGLE PAGE GAME");
		});
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

void UIManager::DrawGame()
{
	wxPanel* gameRoot = new wxPanel(_pages);
	wxPanel* cellParent = new wxPanel(gameRoot);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(cellParent, 0, wxALIGN_CENTER | wxTOP, 0);
	gameRoot->SetSizer(rootSizer);

	/*CreateBoard(cellParent);
	TryRenderAllPieces();*/

	/*bool canDoIt = TryRenderPieceAtPos({ 3, 5 }, { ColorTheme::Light, PieceType::Knight });
	if (!canDoIt) wxLogMessage("failed to render piece");*/

	//TryRenderPieceAtPos({ 2, 7 }, { ColorTheme::Dark, PieceType::King });
	//TryRenderPieceAtPos({ 3, 4 }, { ColorTheme::Light, PieceType::Pawn });
	//TryRenderPieceAtPos({ 4, 5 }, { ColorTheme::Dark, PieceType::Knight });

	_pages->AddPage(gameRoot, "Game");
}

void UIManager::TogglePage(const Page& togglePage)
{
	int pageIndex = static_cast<int>(togglePage);
	_pages->ChangeSelection(pageIndex);
}