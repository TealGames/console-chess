#include <wx/wx.h>
#include <unordered_map>
#include <vector>
#include "GameHUD.hpp"
#include "ResourceManager.hpp"
#include "GameManager.hpp"
#include "GameState.hpp"
#include "Color.hpp"
#include "UIGlobals.hpp"
#include "DirectionalLayout.hpp"
#include "GridLayout.hpp"
#include "WXHelperFunctions.hpp"
#include "HelperFunctions.hpp"

static wxPanel* winningSliderPanel;
static wxStaticText* lightWinPercentText;
static wxStaticText* darkWinPercentText;

static GridLayout* lightPanelPieces;
static GridLayout* darkPanelPieces;

static wxStaticText* lightValueText;
static wxStaticText* darkValueText;

//The panel that encompases a sprite
static const wxSize SPRITE_PANEL_SIZE(30, 30);
static const wxSize SPRITE_SIZE(30, 30);
static const Utils::Vector2D SLIDER_SIZE_OF_PARENT = Utils::Vector2D(0.8, 0.3);

static void SetWinDisplayValue(float percent)
{
	wxSize parentSize = winningSliderPanel->GetParent()->GetSize();

	darkWinPercentText->SetLabel(std::format("{:.0f}%", percent * 100));
	lightWinPercentText->SetLabel(std::format("{:.0f}%", 100 - (percent * 100)));

	winningSliderPanel->SetSize(wxSize(parentSize.GetWidth()* percent, parentSize.GetHeight()));
	winningSliderPanel->SetPosition(wxDefaultPosition);

	Utils::Log(Utils::LogType::Error, std::format("POOP: New size is: {} -> {}",
		std::to_string(parentSize.x),
		std::to_string(winningSliderPanel->GetSize().x)));
}

static void UpdateWinningDisplay(const Core::GameManager& manager, const GameState& state)
{
	std::unordered_map<ArmyColor, float> percents = manager.CalculateWinPercentage(state);
	Utils::Log(std::format("CALC: update win display with percetn: {}", std::to_string(percents.at(ArmyColor::Dark))));
	SetWinDisplayValue(percents.at(ArmyColor::Dark));
}

void CreateWinChanceDisplay(Core::GameManager& manager, wxWindow* parent)
{
	DirectionalLayout* layoutParent = new DirectionalLayout(parent, LayoutType::Horizontal, wxDefaultPosition, parent->GetSize());
	layoutParent->SetBackgroundColour(LIGHT_GREEN);

	const wxSize backgroundSize(SLIDER_SIZE_OF_PARENT.m_X * layoutParent->GetSize().x, SLIDER_SIZE_OF_PARENT.m_Y * layoutParent->GetSize().y);
	const wxSize textSize = wxSize((layoutParent->GetSize().x - backgroundSize.x)/2, layoutParent->GetSize().y);

	darkWinPercentText = new wxStaticText(layoutParent, wxID_ANY, "0%", wxDefaultPosition, 
		textSize, wxALIGN_CENTER_HORIZONTAL);
	darkWinPercentText->SetForegroundColour(LIGHTER_SECONDARY_COLOR);
	//darkWinPercentText->SetBackgroundColour(RED);
	layoutParent->AddChild(darkWinPercentText, 0);

	lightWinPercentText = new wxStaticText(layoutParent, wxID_ANY, "0%", wxDefaultPosition,
		textSize, wxALIGN_CENTER_HORIZONTAL);
	lightWinPercentText->SetForegroundColour(MUTED_WHITE);
	//lightWinPercentText->SetBackgroundColour(RED);
	layoutParent->AddChild(lightWinPercentText, 0);
	
	wxPoint backgroundPos = wxPoint((layoutParent->GetSize().x - backgroundSize.x) / 2, wxDefaultPosition.y);
	wxPanel* background = new wxPanel(layoutParent, wxID_ANY, wxDefaultPosition, backgroundSize);
	background->SetBackgroundColour(MUTED_WHITE);
	layoutParent->AddChild(background, 0, SpacingType::None, 0, 1);
	
	//background->Center();

	
	 
	winningSliderPanel = new wxPanel(background, wxID_ANY, wxDefaultPosition, background->GetSize());
	winningSliderPanel->SetBackgroundColour(LIGHTER_SECONDARY_COLOR);
	//layoutParent->AddChild(winningSliderPanel, 1, SpacingType::Center);
	SetWinDisplayValue(0.5);

	manager.AddEventCallback(Core::GameEventType::SuccessfulTurn, 
		[&manager](const GameState& state) -> void{ UpdateWinningDisplay(manager, state); });
}

static void UpdateCaptureDisplay(const GameState& state)
{
	/*Utils::Log(std::format("DISPLAY: Update display game state: dark: {} light:",
		std::to_string(state.TeamValue.size())));*/

		//std::to_string(state.TeamValue.at(ColorTheme::Dark)), std::to_string(state.TeamValue.at(ColorTheme::Light))));
	lightValueText->SetLabel("PTS: "+std::to_string(state.TeamValue.at(ArmyColor::Light)));
	lightValueText->SetSize(lightValueText->GetBestSize());

	wxPoint lightCenterPos = wxPoint((lightValueText->GetParent()->GetSize().x - 
		lightValueText->GetSize().x)/2, lightValueText->GetPosition().y);
	lightValueText->SetPosition(lightCenterPos);

	darkValueText->SetLabel("PTS: "+std::to_string(state.TeamValue.at(ArmyColor::Dark)));
	darkValueText->SetSize(darkValueText->GetBestSize());

	wxPoint darkCenterPos = wxPoint((lightValueText->GetParent()->GetSize().x - 
		lightValueText->GetSize().x) / 2, lightValueText->GetPosition().y);
	darkValueText->SetPosition(darkCenterPos);

	if (lightPanelPieces != nullptr) lightPanelPieces->DestroyLayout();
	if (darkPanelPieces!=nullptr) darkPanelPieces->DestroyLayout();

	GridLayout* currentLayout = nullptr;
	for (const auto& capturedPiece : state.CapturedPieces)
	{
		std::optional<wxBitmap> maybeMap = TryGetBitMapFromPiece(PieceTypeInfo{capturedPiece->m_Color, capturedPiece->m_PieceType}, SPRITE_SIZE);
		if (!maybeMap.has_value())
		{
			const std::string err = std::format("Tried to update capture display for pieces "
				"but could not find bitmap for piece: {}", capturedPiece->ToString());
			Utils::Log(Utils::LogType::Error, err);
			return;
		}
		
		bool hasCapturedDark = capturedPiece->m_Color == ArmyColor::Dark;
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
	const wxSize LAYOUT_SIZE(0.9 * displayRoot->GetSize().x, 0.2 * displayRoot->GetSize().y);

	DirectionalLayout* lightLayout= new DirectionalLayout(displayRoot, LayoutType::Vertical, 
		wxDefaultPosition, LAYOUT_SIZE);
	lightLayout->SetBackgroundColour(BACKGROUND_COLOR);

	wxStaticText* lightTitle = new wxStaticText(lightLayout, wxID_ANY, "WHITE", 
		wxDefaultPosition, wxSize(lightLayout->GetSize().x, wxDefaultSize.y), wxALIGN_CENTER_HORIZONTAL);
	lightTitle->SetFont(HEADING_FONT);
	//lightTitle->CenterOnParent();
	lightTitle->SetSize(wxSize(lightLayout->GetSize().x, lightTitle->GetBestSize().y));
	lightTitle->SetForegroundColour(MUTED_WHITE);
	//lightTitle->SetBacckgroundColour(LIGHT_GREEN);
	lightLayout->AddChild(lightTitle, 0, SpacingType::Top | SpacingType::Left | SpacingType::Right, 5);

	lightValueText = new wxStaticText(lightLayout, wxID_ANY, "NULL", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	lightValueText->SetFont(CAPTION_ITALIC_FONT);
	//lightValueText->SetSize(wxSize(lightTitle->GetSize().x, lightValueText->GetBestSize().y));
	lightValueText->SetSize(lightValueText->GetBestSize());
	lightValueText->SetForegroundColour(MUTED_WHITE);
	//lightValueText->SetBackgroundColour(LIGHT_GREEN);
	lightLayout->AddChild(lightValueText, 0, SpacingType::Center, 10);


	const wxSize PANEL_PIECES_SIZE(0.8 * lightLayout->GetSize().x, 0.6 * lightLayout->GetSize().y);
	const int LAYOUT_SPACING = 10;
	const wxSize spriteGridRowCols = wxSize(PANEL_PIECES_SIZE.y / SPRITE_PANEL_SIZE.y, PANEL_PIECES_SIZE.x/SPRITE_PANEL_SIZE.x);
	Utils::Log(std::format("PIECE CHECK: rwos: {} cols: {}", std::to_string(spriteGridRowCols.x), std::to_string(spriteGridRowCols.y)));
	const wxSize spriteMargins = wxSize(5, 5);

	lightPanelPieces = new GridLayout(lightLayout, spriteGridRowCols, spriteMargins, wxDefaultPosition, PANEL_PIECES_SIZE);
	lightPanelPieces->SetBackgroundColour(LIGHT_GREEN);
	
	lightLayout->AddChild(lightPanelPieces, 1, SpacingType::Center);
	displayRoot->AddChild(lightLayout, 1, SPACING_ALL_SIDES, LAYOUT_SPACING);


	DirectionalLayout* darkLayout = new DirectionalLayout(displayRoot, LayoutType::Vertical,
		wxDefaultPosition, LAYOUT_SIZE);
	darkLayout->SetBackgroundColour(BACKGROUND_COLOR);

	wxStaticText* darkTitle = new wxStaticText(darkLayout, wxID_ANY, "DARK", 
		wxDefaultPosition, wxSize(lightLayout->GetSize().x, wxDefaultSize.y), wxALIGN_CENTER_HORIZONTAL);
	darkTitle->SetFont(HEADING_FONT);
	//darkTitle->CenterOnParent();
	darkTitle->SetSize(wxSize(darkLayout->GetSize().x, darkTitle->GetBestSize().y));
	darkTitle->SetForegroundColour(NORMAL_GRAY);
	//darkTitle->SetBackgroundColour(LIGHT_GREEN);
	darkLayout->AddChild(darkTitle, 0, SpacingType::Top | SpacingType::Left | SpacingType::Right, 5);

	darkValueText = new wxStaticText(darkLayout, wxID_ANY, "NULL", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	darkValueText->SetFont(CAPTION_ITALIC_FONT);
	darkValueText->SetSize(darkValueText->GetBestSize());
	darkValueText->SetForegroundColour(NORMAL_GRAY);
	//darkValueText->SetBackgroundColour(LIGHT_GREEN);
	darkLayout->AddChild(darkValueText, 0, SpacingType::Center, 10);

	darkPanelPieces = new GridLayout(darkLayout, spriteGridRowCols, spriteMargins, wxDefaultPosition, PANEL_PIECES_SIZE);
	darkPanelPieces->SetBackgroundColour(LIGHT_GREEN);
	darkLayout->AddChild(darkPanelPieces, 1, SpacingType::Center);
	displayRoot->AddChild(darkLayout, 1, SPACING_ALL_SIDES, LAYOUT_SPACING);

	manager.AddEventCallback(Core::GameEventType::SuccessfulTurn, &UpdateCaptureDisplay);
	manager.AddEventCallback(Core::GameEventType::StartGame, &UpdateCaptureDisplay);
}
