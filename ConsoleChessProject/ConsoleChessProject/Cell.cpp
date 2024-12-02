#include <wx/wx.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <functional>
#include "Cell.hpp"
#include "HelperFunctions.hpp"
#include "WXHelperFunctions.hpp"
#include "ResourceManager.hpp"

std::string ToString(const CellState& state)
{
	if (state == CellState::Default) 
		return "Default";
	else if (state == CellState::Disabled) 
		return "Disabled";
	else if (state == CellState::PossibleMoveHighlighted) 
		return "Possible Move Highlighted";
	else if (state == CellState::PreviousMoveHighlighted) 
		return "Previous Move Highlighted";
	else if (state == CellState::Selected) 
		return "Selected";
	else return "NULL";
}

Cell::Cell(wxWindow* parent, wxPoint pos, const ArmyColor& tileColor, const CellColors& colors,
	const std::unordered_map<CellState, wxBitmap*>& stateSprites)
	: wxPanel(parent, wxID_ANY, pos, CELL_SIZE), m_colors(colors), m_TileColor(tileColor),
	m_bitMapDisplay(nullptr), m_isRenderingPiece(false),
	m_isClickable(true), m_IsClickable(m_isClickable),
	m_hasOverlayImage(false), m_HasOverlayImage(m_hasOverlayImage), 
	//_highlightedType(std::nullopt), 
	m_currentState(CellState::Default), m_CurrentState(m_currentState), m_stateSprites(stateSprites),
	m_pieceHere(nullptr), m_onClickCallbacks()
{
	m_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, wxBitmap{}, wxDefaultPosition);
	m_bitMapDisplay->Enable(false);

	m_overlayPanel = new wxStaticBitmap(this, wxID_ANY, wxBitmap{}, wxDefaultPosition);
	//_overlayPanel->Raise();
	m_overlayPanel->Enable(false);

	Bind(wxEVT_ENTER_WINDOW, &Cell::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &Cell::OnExit, this);
	Bind(wxEVT_LEFT_DOWN, &Cell::OnClick, this);

	SetBackgroundColour(colors.InnerColor);
}

void Cell::OnEnter(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	if (IsHighlighted() || IsDisabled()) return;

	m_lastColor = GetBackgroundColour();
	int currentRed = static_cast<int>(m_lastColor.Red());
	int currentGreen = static_cast<int>(m_lastColor.Green());
	int currentBlue = static_cast<int>(m_lastColor.Blue());

	int newRed = std::clamp(static_cast<int>(1.2 * currentRed), 0, 255);
	int newGreen = std::clamp(static_cast<int>(1.2 * currentGreen), 0, 255);
	int newBlue = std::clamp(static_cast<int>(1.2 * currentBlue), 0, 255);
	wxColour newColor = wxColour(newRed, newGreen, newBlue);

	SetBackgroundColour(newColor);
	//If we are highlighted we do not want to return back until the caller says so

	//SetBackgroundColour(_colors.HoverColor);
	Refresh();
	
}

void Cell::OnExit(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	//If we are highlighted we do not want to return back until the caller says so
	if (IsHighlighted() || IsDisabled()) return;

	//SetBackgroundColour(_colors.InnerColor);
	SetBackgroundColour(m_lastColor);
	Refresh();
}

void Cell::OnClick(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	Utils::Log(Utils::LogType::Error, std::format("Clicked on cell with state: {}", ToString(cell->m_currentState)));

	if (IsDisabled()) return;
	for (const auto& callback : m_onClickCallbacks) callback(this);

	//Refresh();
	//wxLogMessage("Click");
}

void Cell::AddOnClickCallback(const std::function<void(Cell*)>& callback)
{
	m_onClickCallbacks.push_back(callback);
}

bool Cell::IsHighlighted() const
{
	return m_currentState == CellState::PossibleMoveHighlighted ||
		m_currentState == CellState::PreviousMoveHighlighted ||
		m_currentState == CellState::Selected;
}

//std::optional<HighlightColorType> Cell::GetHighlightedColorType() const
//{
//	return _highlightedType;
//}

//std::optional<const wxColour*> Cell::TryGetColorForState(const CellVisualState& state)
//{
//	if (state == CellVisualState::Default) return &_colors.InnerColor;
//	else if (state == CellVisualState::Selected) return &_colors.SelectedColor;
//	else if (state == CellVisualState::PossibleMoveHighlighted)
//	{
//		if (HasPiece()) return &_colors.PossibleCaptureColor;
//		return &_colors.PossibleMoveColor;
//	}
//	else if (state == CellVisualState::PreviousMoveHighlighted) return &_colors.PreviousMoveColor;
//	else return std::nullopt;
//}

bool Cell::IsRenderingPiece() const
{
	return m_isRenderingPiece;
}

bool Cell::IsDisabled() const
{
	return !m_isClickable;
}

bool Cell::HasPieceStored(const Piece** outFoundPiece)
{
	if (m_pieceHere!=nullptr && outFoundPiece!=nullptr) *outFoundPiece = m_pieceHere;
	/*Utils::Log(std::format("TURN: HAS PIECE STORED: {} OUT: {}", 
		_pieceHere == nullptr ? "NULL" : _pieceHere->ToString(),
		outFoundPiece==nullptr? "NULL PTR" : *outFoundPiece==nullptr? "NULL" : (*outFoundPiece)->ToString()));*/

	return m_pieceHere != nullptr;
}

bool Cell::HasPiece(const Piece** outFoundPiece)
{
	if (m_pieceHere!=nullptr) Utils::Log(std::format("Has piece check existing data: {}", m_pieceHere->ToString()));
	return HasPieceStored(outFoundPiece) || IsRenderingPiece();
}

void Cell::SetPiece(const Piece* piece, wxImage& image)
{
	//pieceHere = &piece;
	//TODO: assignment does nto work since it is not defined for Piece so peiceHere is always NULL!
	m_pieceHere = piece;
	Utils::Log(std::format("UPDATE PIECE NOW: {}", m_pieceHere==nullptr? "NULL" : m_pieceHere->ToString()));
	
	wxSize startSize(image.GetWidth(), image.GetHeight());
	wxSize targetSize(static_cast<int>(ICON_SIZE_TO_CELL * CELL_SIZE.x), 
					  static_cast<int>(ICON_SIZE_TO_CELL * CELL_SIZE.y));
	/*
	if (_UPDATE_IMAGE_SIZE && startSize!=targetSize)
	{
		float newWidthScale =  static_cast<float>(targetSize.x)/ startSize.x;
		float newHeightScale = static_cast<float>(targetSize.y)/ startSize.y;
		float totalScale = std::max(newWidthScale, newHeightScale);
		wxSize size(image.GetWidth() * totalScale, image.GetHeight() * totalScale);

		Resize(image, size);
		std::string m = std::format("Start: ({}, {})  END: ({}, {}) target: {} {}", std::to_string(startSize.x), std::to_string(startSize.y),
			std::to_string(image.GetWidth()), std::to_string(image.GetHeight()), std::to_string(targetSize.x), std::to_string(targetSize.y));
		wxLogMessage(m.c_str());
	}*/
	
	wxBitmap map = ConvertToBitMap(image, targetSize);
	m_bitMapDisplay->SetBitmap(map);
	//_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, map, this->GetPosition());
	m_bitMapDisplay->Center();
	m_isRenderingPiece = true;

	std::string posCenter = std::to_string(m_bitMapDisplay->GetPosition().x)+std::to_string(m_bitMapDisplay->GetPosition().y);
	wxLogMessage(posCenter.c_str());

	/*_bitMapDisplay->Bind(wxEVT_ENTER_WINDOW, &Cell::SkipMouseEvent, this);
	_bitMapDisplay->Bind(wxEVT_LEAVE_WINDOW, &Cell::SkipMouseEvent, this);
	_bitMapDisplay->Bind(wxEVT_LEFT_DOWN, &Cell::SkipMouseEvent, this);*/

	//This is done to allow events to be ignored by the child so it would reach the parent (the cell)
	//_bitMapDisplay->Enable(false);

	/*wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(_bitMapDisplay, 0, wxCENTER, 0);
	SetSizerAndFit(sizer);*/
	Refresh();
}

bool Cell::TryRemovePiece()
{
	bool renderingOrPieceRemoved = false;
	if (IsRenderingPiece())
	{
		m_bitMapDisplay->SetBitmap({});
		m_isRenderingPiece = false;
		renderingOrPieceRemoved = true;
		Utils::Log(std::format("PIECE CHECK TRY REMOVE PIECE: {}",
			m_pieceHere == nullptr ? "NULL" : m_pieceHere->ToString()));
	}

	if (HasPieceStored(nullptr))
	{
		m_pieceHere = nullptr;
		if (!renderingOrPieceRemoved) 
			renderingOrPieceRemoved = true;
	}
	
	return renderingOrPieceRemoved;
}

void Cell::SetCellColors(const CellColors& colors)
{
	m_colors = colors;
}

void Cell::SetOverlaySprite(const wxBitmap& bitmap)
{
	m_hasOverlayImage = true;
	m_overlayPanel->SetBitmap(bitmap);
	m_overlayPanel->Center();
	Refresh();
}

void Cell::RemoveOverlaySprite()
{
	m_hasOverlayImage = false;
	m_overlayPanel->SetBitmap({});
	Refresh();
}

void Cell::UpdateCanClick(const bool isClickable, const bool updateVisual)
{
	m_isClickable = isClickable;

	Utils::Log(std::format("TURN: Tried to make cell disabled"));
	//if (_isClickable) SetVisualState(CellVisualState::Default, updateVisual);
	//else SetVisualState(CellVisualState::Disabled, updateVisual);
}	

void Cell::SetState(const CellState& state, const bool updateVisual)
{
	if (!updateVisual)
	{
		m_currentState = state;
		return;
	}

	auto stateSpriteIt = m_stateSprites.find(state);
	bool hasStateSprite = stateSpriteIt != m_stateSprites.end();
	
	//return;
	
	//std::optional<const wxColour*> maybeStateColor = TryGetColorForState(state);
	if (m_HasOverlayImage) RemoveOverlaySprite();

	if (state == CellState::Default)
	{
		SetBackgroundColour(m_colors.InnerColor);
	}
	else if (state == CellState::Selected)
	{
		SetBackgroundColour(m_colors.SelectedColor);
	}
	else if (state == CellState::PossibleMoveHighlighted)
	{
		if (HasPiece()) SetBackgroundColour(m_colors.PossibleCaptureColor);
		else if (hasStateSprite) SetOverlaySprite(*(stateSpriteIt->second));
	}
	else if (state == CellState::PreviousMoveHighlighted)
	{
		SetBackgroundColour(m_colors.PreviousMoveColor);
	}
	//Only if the previous state is not previous move highlighted do we disable the overlay
	else if (m_currentState!=CellState::PreviousMoveHighlighted && 
		state == CellState::Disabled)
	{
		
		int grayCoefficient = 0.3;
		wxColour currentColor = GetBackgroundColour();
		int averageRGB = (currentColor.Red() + currentColor.Green() + currentColor.Blue()) / 3;
		/*float newR = currentColor.Red() + grayCoefficient * (averageRGB- currentColor.Red());
		float newG= currentColor.Green() + grayCoefficient * (averageRGB - currentColor.Green());
		float newB = currentColor.Blue() + grayCoefficient * (averageRGB - currentColor.Blue());

		int newRInt = static_cast<int>(newR*255);
		int newGInt = static_cast<int>(newG*255);
		int newBInt = static_cast<int>(newB*255);*/

		/*Utils::Log(std::format("TURN: Tried to make cell disabled with start color: {} new color:{}",
			WXUtils::ToString(currentColor), WXUtils::ToString(wxColour(newRInt, newGInt, newBInt))));
		SetBackgroundColour(wxColour(newRInt, newGInt, newBInt));*/

		Utils::Log(std::format("TURN: Tried to make cell disabled with start color: {} new color:{}",
			WXUtils::ToString(currentColor), WXUtils::ToString(wxColour(averageRGB, averageRGB, averageRGB))));
		SetBackgroundColour(wxColour(averageRGB, averageRGB, averageRGB));
		
		Utils::Log(std::format("TURN State sprite it: {} for state: {}",
			std::to_string(m_stateSprites.find(state) != m_stateSprites.end()), ToString(state)));
		//SetOverlaySprite(*(stateSpriteIt->second));
	}
	/*else
	{
		const std::string err = std::format("Tried to update visual state for cell for a type "
			"that is not defined with its own instructions!");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}*/

	m_currentState = state;
	UpdateCanClick(m_currentState!=CellState::Disabled, true);
	Refresh();
}

void Cell::ToggleState(const CellState& state)
{
	if (m_currentState == CellState::Default) SetState(state, true);
	else SetState(CellState::Default, true);
}

void Cell::ResetStateToDefault()
{
	return SetState(CellState::Default, true);
}

//const wxColour& Cell::GetHighlightColor(const HighlightColorType highlightType) const
//{
//	if (highlightType == HighlightColorType::Selected) return _colors.SelectedColor;
//	else if (highlightType == HighlightColorType::PossibleMove) return _colors.PossibleMoveColor;
//	else if (highlightType == HighlightColorType::PreviousMove) return _colors.PreviousMoveColor;
//	else
//	{
//		const std::string err = std::format("Tried to get highlight color for a type "
//			"that is not defined with its own color!");
//		Utils::Log(Utils::LogType::Error, err);
//		return {};
//	}
//}

//void Cell::Highlight(const HighlightColorType& highlightType)
//{
//	_highlightedType = highlightType;
//	SetBackgroundColour(GetHighlightColor(highlightType));
//	Refresh();
//}
//
//void Cell::Dehighlight()
//{
//	_highlightedType = std::nullopt;
//	SetBackgroundColour(_colors.InnerColor);
//	Refresh();
//}
//
//void Cell::ToggleHighlighted(const HighlightColorType& highlightTypeIfToggle)
//{
//	if (IsHighlighted()) Dehighlight();
//	else Highlight(highlightTypeIfToggle);
//}