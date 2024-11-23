#include <wx/wx.h>
#include <unordered_map>
#include <vector>
#include "CapturedPiecesUI.hpp"
#include "ResourceManager.hpp"
#include "BoardManager.hpp"
#include "GameState.hpp"
#include "Color.hpp"
#include "UIGlobals.hpp"
#include "DirectionalLayout.hpp"
#include "WXHelperFunctions.hpp"

static wxPanel* lightPanelPieces;
static wxPanel* darkPanelPieces;

static const wxSize PANEL_SIZE(100, 200);
static const wxSize CAPTURED_SPRITE_SIZE{30, 30};

static void UpdateDisplay(const GameState& state)
{
	if (lightPanelPieces!=nullptr) lightPanelPieces->DestroyChildren();
	if (darkPanelPieces!=nullptr) darkPanelPieces->DestroyChildren();

	for (const auto& capturedPiece : state.CapturedPieces)
	{
		std::optional<wxBitmap> maybeMap = TryGetBitMapFromPiece(PieceTypeInfo{capturedPiece.color, capturedPiece.pieceType}, CAPTURED_SPRITE_SIZE);
		if (!maybeMap.has_value())
		{
			const std::string err = std::format("Tried to update capture display for pieces "
				"but could not find bitmap for piece: {}", capturedPiece.ToString());
			Utils::Log(Utils::LogType::Error, err);
			return;
		}
		
		//Remember the light teamn captures dark pieces and vice versa
		wxStaticBitmap* bitmapDisplay = new wxStaticBitmap(capturedPiece.color==ColorTheme::Dark? 
			lightPanelPieces : darkPanelPieces, wxID_ANY, maybeMap.value());
		bitmapDisplay->Center();
	}
}

void CreateCaptureDisplay(wxWindow* parent)
{
	//TODO: create a horizontal or vertical layout component that basically hides the whole box sizer creation
	//and this compoennt should be a panel that basically manages the children (allowing for not needing to add sizer everytime)
	wxPanel* displayRoot = new wxPanel(parent, wxID_ANY, wxDefaultPosition, parent->GetSize());
	const wxSize layoutSize(0.8 * displayRoot->GetSize().x, 0.2 * displayRoot->GetSize().y);

	DirectionalLayout* lightLayout= new DirectionalLayout(displayRoot, LayoutType::Vertical, 
		wxPoint((displayRoot->GetSize().x-layoutSize.x)/2, 0), layoutSize);
	lightLayout->SetBackgroundColour(LIGHTER_SECONDARY_COLOR_2);

	wxStaticText* lightTitle = new wxStaticText(lightLayout, wxID_ANY, "WHITE CAPTURED");
	lightTitle->CenterOnParent();
	lightTitle->SetSize(wxSize(lightLayout->GetBestSize()));
	lightTitle->SetForegroundColour(MUTED_WHITE);
	lightLayout->AddChild(lightTitle, SpacingType::Top, 0);

	const wxSize panelPiecesSize(0.8 * lightLayout->GetSize().x, 0.6 * lightLayout->GetSize().y);
	lightPanelPieces = new wxPanel(lightLayout, wxID_ANY, wxDefaultPosition, panelPiecesSize);
	lightLayout->AddChild(lightPanelPieces, SpacingType::Center);



	DirectionalLayout* darkLayout = new DirectionalLayout(displayRoot, LayoutType::Vertical,
		wxPoint((displayRoot->GetSize().x - layoutSize.x) / 2, 0), layoutSize);
	lightLayout->SetBackgroundColour(LIGHTER_SECONDARY_COLOR_2);

	wxStaticText* darkTitle = new wxStaticText(darkLayout, wxID_ANY, "DARK CAPTURED");
	darkTitle->CenterOnParent();
	darkTitle->SetSize(wxSize(darkLayout->GetBestSize()));
	darkTitle->SetForegroundColour(BACKGROUND_COLOR);
	darkLayout->AddChild(darkTitle, SpacingType::Top, 50);

	darkPanelPieces = new wxPanel(darkLayout, wxID_ANY, wxDefaultPosition, panelPiecesSize);
	darkLayout->AddChild(darkPanelPieces, SpacingType::Center);*/

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

	Board::AddPieceMoveCallback(&UpdateDisplay);
}
