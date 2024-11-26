#include <wx/wx.h>
#include <unordered_map>
#include <vector>
#include "CapturedPiecesUI.hpp"
#include "ResourceManager.hpp"
#include "GameManager.hpp"
#include "GameState.hpp"
#include "Color.hpp"
#include "UIGlobals.hpp"
#include "DirectionalLayout.hpp"
#include "GridLayout.hpp"
#include "WXHelperFunctions.hpp"

static GridLayout* lightPanelPieces;
static GridLayout* darkPanelPieces;

//The panel that encompases a sprite
static const wxSize SPRITE_PANEL_SIZE(30, 30);
static const wxSize SPRITE_SIZE(30, 30);

static void UpdateDisplay(const GameState& state)
{
	if (lightPanelPieces != nullptr) lightPanelPieces->DestroyLayout();
	if (darkPanelPieces!=nullptr) darkPanelPieces->DestroyLayout();

	GridLayout* currentLayout = nullptr;
	for (const auto& capturedPiece : state.CapturedPieces)
	{
		std::optional<wxBitmap> maybeMap = TryGetBitMapFromPiece(PieceTypeInfo{capturedPiece.m_Color, capturedPiece.m_PieceType}, SPRITE_SIZE);
		if (!maybeMap.has_value())
		{
			const std::string err = std::format("Tried to update capture display for pieces "
				"but could not find bitmap for piece: {}", capturedPiece.ToString());
			Utils::Log(Utils::LogType::Error, err);
			return;
		}
		
		bool hasCapturedDark = capturedPiece.m_Color == ColorTheme::Dark;
		currentLayout = hasCapturedDark ? lightPanelPieces : darkPanelPieces;

		wxPanel* piecePanel = new wxPanel(currentLayout, wxID_ANY, wxDefaultPosition, SPRITE_PANEL_SIZE);
		piecePanel->SetBackgroundColour(hasCapturedDark? LIGHT_CELL_COLOR : DARK_CELL_COLOR);
		piecePanel->CenterOnParent();
		currentLayout->AddChild(piecePanel);

		//Remember the light teamn captures dark pieces and vice versa
		wxStaticBitmap* bitmapDisplay = new wxStaticBitmap(piecePanel, wxID_ANY, maybeMap.value());

		bitmapDisplay->Center();
	}
}

void CreateCaptureDisplay(Core::GameManager& manager, wxWindow* parent)
{
	DirectionalLayout* displayRoot = new DirectionalLayout(parent, LayoutType::Vertical, wxDefaultPosition, parent->GetSize());
	const wxSize LAYOUT_SIZE(0.8 * displayRoot->GetSize().x, 0.2 * displayRoot->GetSize().y);

	DirectionalLayout* lightLayout= new DirectionalLayout(displayRoot, LayoutType::Vertical, 
		wxPoint((displayRoot->GetSize().x- LAYOUT_SIZE.x)/2, 0), LAYOUT_SIZE);
	lightLayout->SetBackgroundColour(BACKGROUND_COLOR);

	wxStaticText* lightTitle = new wxStaticText(lightLayout, wxID_ANY, "WHITE CAPTURED", 
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	lightTitle->SetFont(HEADING_FONT);
	//lightTitle->CenterOnParent();
	lightTitle->SetSize(wxSize(lightLayout->GetSize().x, lightTitle->GetBestSize().y));
	lightTitle->SetForegroundColour(MUTED_WHITE);
	//lightTitle->SetBacckgroundColour(LIGHT_GREEN);
	lightLayout->AddChild(lightTitle, 0, SPACING_ALL_SIDES, 10);

	const wxSize PANEL_PIECES_SIZE(0.8 * lightLayout->GetSize().x, 0.6 * lightLayout->GetSize().y);
	const int LAYOUT_SPACING = 20;
	const wxSize spriteGridRowCols = wxSize(PANEL_PIECES_SIZE.y / SPRITE_PANEL_SIZE.y, PANEL_PIECES_SIZE.x/SPRITE_PANEL_SIZE.x);
	Utils::Log(std::format("PIECE CHECK: rwos: {} cols: {}", std::to_string(spriteGridRowCols.x), std::to_string(spriteGridRowCols.y)));
	const wxSize spriteMargins = wxSize(5, 5);

	lightPanelPieces = new GridLayout(lightLayout, spriteGridRowCols, spriteMargins, wxDefaultPosition, PANEL_PIECES_SIZE);
	lightPanelPieces->SetBackgroundColour(LIGHT_GREEN);
	lightLayout->AddChild(lightPanelPieces, 1, SpacingType::Center);
	displayRoot->AddChild(lightLayout, 1, SpacingType::Center | SPACING_ALL_SIDES, LAYOUT_SPACING);

	DirectionalLayout* darkLayout = new DirectionalLayout(displayRoot, LayoutType::Vertical,
		wxPoint((displayRoot->GetSize().x - LAYOUT_SIZE.x) / 2, 0), LAYOUT_SIZE);
	darkLayout->SetBackgroundColour(BACKGROUND_COLOR);

	wxStaticText* darkTitle = new wxStaticText(darkLayout, wxID_ANY, "DARK CAPTURED", 
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	darkTitle->SetFont(HEADING_FONT);
	//darkTitle->CenterOnParent();
	darkTitle->SetSize(wxSize(darkLayout->GetBestSize()));
	darkTitle->SetForegroundColour(LIGHTER_SECONDARY_COLOR_2);
	//darkTitle->SetBackgroundColour(LIGHT_GREEN);
	darkLayout->AddChild(darkTitle, 0, SPACING_ALL_SIDES, 10);

	darkPanelPieces = new GridLayout(darkLayout, spriteGridRowCols, spriteMargins, wxDefaultPosition, PANEL_PIECES_SIZE);
	darkPanelPieces->SetBackgroundColour(LIGHT_GREEN);
	darkLayout->AddChild(darkPanelPieces, 1, SpacingType::Center);
	displayRoot->AddChild(darkLayout, 1, SpacingType::Center | SPACING_ALL_SIDES, LAYOUT_SPACING);


	/*wxBoxSizer* lightPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	lightPanelSizer->Add(lightPanelPieces, 0, wxEXPAND, 0);
	lightPanelPieces->SetSizer(lightPanelSizer);

	

	//lightTitle->SetBackgroundColour(TAN);
	//lightTitle->Center();


	/*
	DirectionalLayout* darkLayout = new DirectionalLayout(displayRoot, LayoutType::Vertical);

	wxStaticText* darkTitle = new wxStaticText(darkLayout, wxID_ANY, "BLACK CAPTURED", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	darkLayout->AddChild(lightTitle, SpacingType::Top, 0);

	darkPanelPieces = new wxPanel(parent, wxID_ANY, wxDefaultPosition, PIECE_DISPLAY_SIZE);
	darkLayout->AddChild(darkPanelPieces, SpacingType::Bottom, 20);
	*/


	
	/*wxBoxSizer* lightPanelSectionSizer = new wxBoxSizer(wxVERTICAL);
	lightPanelSectionSizer->Add(lightTitle, 0, wxEXPAND, 0);
	lightPanelSectionSizer->Add(lightPanelPieces, 0, wxEXPAND, 0);
	lightPanelSection->SetSizer(lightPanelSectionSizer);*/


	//wxPanel* darkPanelSection = new wxPanel(displayRoot);
	//wxStaticText* darkTitle = new wxStaticText(darkPanelSection, wxID_ANY, "BLACK CAPTURED");
	//darkTitle->Center();


	//darkTitle->SetForegroundColour(TAN);

	//wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	//rootSizer->Add(lightLayout, 0, wxTOP | wxEXPAND, 0);
	//rootSizer->Add(darkLayout, 0, wxBOTTOM | wxEXPAND, 50);
	//displayRoot->SetSizer(rootSizer);


	
	//darkPanelPieces = new wxPanel(parent, wxID_ANY, wxDefaultPosition, PIECE_DISPLAY_SIZE);

	

	/*wxBoxSizer* darkPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	darkPanelSizer->Add(darkPanelPieces, 0, wxEXPAND, 0);
	darkPanelPieces->SetSizer(darkPanelSizer);*/

	//Board::AddPieceMoveCallback(&UpdateDisplay);
	manager.AddEventCallback(Core::GameEventType::PieceMoved, &UpdateDisplay);
}
