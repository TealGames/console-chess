#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/simplebook.h>
#include "MainFrame.hpp"
#include "CButton.hpp"
#include "Cell.hpp"
#include "BoardUI.hpp"

static const wxFont TITLE_FONT(30, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

static constexpr int TITLE_Y_OFFSET = 50;
static constexpr int BUTTON_START_Y = 150;
static constexpr int BUTTON_SPACING = 70;

MainFrame::MainFrame(const wxString& title)
	: wxFrame(nullptr, wxID_ANY, title)
{
	wxStandardPaths& paths = wxStandardPaths::Get();
	wxLogMessage(paths.GetResourcesDir());
	DrawStatic();
	DrawMainMenu();
	DrawGame();

	TogglePage(Page::MainMenu);
}

void MainFrame::DrawStatic()
{
	root = new wxPanel(this);
	root->SetBackgroundColour(wxColour(DARK_GRAY));
	wxStaticText* titleText = new wxStaticText(root, wxID_ANY, "CHESS", wxPoint(0, TITLE_Y_OFFSET), wxSize(WIDTH, 100), wxALIGN_CENTER_HORIZONTAL);
	titleText->SetForegroundColour(wxColour(248, 248, 248));

	titleText->SetFont(TITLE_FONT);
	pages = new wxSimplebook(root, wxID_ANY);

	//Sizer for root panel
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(titleText, 0, wxALIGN_CENTER | wxTOP, 0);
	rootSizer->Add(pages, 0, wxALIGN_CENTER, 0);
	root->SetSizer(rootSizer);
}

void MainFrame::DrawMainMenu()
{
	wxPanel* mainMenuRoot = new wxPanel(pages);
	//wxBoxSizer* mainMenuSizer = new wxBoxSizer(wxVERTICAL);

	CButton* newGameButton = new CButton(mainMenuRoot, "New Game");
	CenterX(newGameButton);
	newGameButton->MoveY(BUTTON_START_Y);
	newGameButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt)
		{
			TogglePage(Page::Game);
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
	pages->AddPage(mainMenuRoot, "MainMenu");
}

void MainFrame::DrawGame()
{
	wxPanel* gameRoot = new wxPanel(pages);
	wxPanel* cellParent = new wxPanel(gameRoot);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(cellParent, 0, wxALIGN_CENTER | wxTOP, 0);
	gameRoot->SetSizer(rootSizer);
	CreateBoard(cellParent);

	bool canDoIt= TryRenderPieceAtPos({ 3, 5 }, PieceType::Queen);
	if (!canDoIt) wxLogMessage("failed to render piece");
	
	pages->AddPage(gameRoot, "Game");
}

void MainFrame::TogglePage(const Page& togglePage)
{
	int pageIndex = static_cast<int>(togglePage);
	/*for (size_t i = 0; i < pages->GetPageCount(); i++)
	{
		if (i == pageIndex)
		{
			pages->GetPage(pageIndex)->Enable();
			wxLogMessage("Enabling page: %d", pageIndex);
		}
		else pages->GetPage(pageIndex)->Disable();
	}*/
	pages->ChangeSelection(pageIndex);
	//pages->Layout();
	//root->Layout();
	//wxLogMessage("The value of someInt is: %d", pages->GetSelection());
}
