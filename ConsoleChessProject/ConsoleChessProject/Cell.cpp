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

std::string ToString(const CellVisualState& state)
{
	if (state == CellVisualState::Default) 
		return "Default";
	else if (state == CellVisualState::Disabled) 
		return "Disabled";
	else if (state == CellVisualState::PossibleMoveHighlighted) 
		return "Possible Move Highlighted";
	else if (state == CellVisualState::PreviousMoveHighlighted) 
		return "Previous Move Highlighted";
	else if (state == CellVisualState::Selected) 
		return "Selected";
	else return "NULL";
}

Cell::Cell(wxWindow* parent, wxPoint pos, const CellColors& colors,
	const std::unordered_map<CellVisualState, wxBitmap*>& stateSprites)
	: wxPanel(parent, wxID_ANY, pos, CELL_SIZE), _colors(colors), _bitMapDisplay(nullptr), 
	_isRenderingPiece(false),
	_isClickable(true), IsClickable(_isClickable),
	_hasOverlayImage(false), HasOverlayImage(_hasOverlayImage), 
	//_highlightedType(std::nullopt), 
	_visualState(CellVisualState::Default), VisualState(_visualState), _stateSprites(stateSprites),
	_pieceHere(nullptr), _onClickCallbacks()
{
	_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, wxBitmap{}, wxDefaultPosition);
	_bitMapDisplay->Enable(false);

	_overlayPanel = new wxStaticBitmap(this, wxID_ANY, wxBitmap{}, wxDefaultPosition);
	_overlayPanel->Enable(false);

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

	_lastColor = GetBackgroundColour();
	int currentRed = static_cast<int>(_lastColor.Red());
	int currentGreen = static_cast<int>(_lastColor.Green());
	int currentBlue = static_cast<int>(_lastColor.Blue());

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
	SetBackgroundColour(_lastColor);
	Refresh();
}

void Cell::OnClick(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	for (const auto& callback : _onClickCallbacks) callback(this);

	//Refresh();
	//wxLogMessage("Click");
}

void Cell::AddOnClickCallback(const std::function<void(Cell*)>& callback)
{
	_onClickCallbacks.push_back(callback);
}

bool Cell::IsHighlighted() const
{
	return _visualState == CellVisualState::PossibleMoveHighlighted ||
		_visualState == CellVisualState::PreviousMoveHighlighted ||
		_visualState == CellVisualState::Selected;
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
	return _isRenderingPiece;
}

bool Cell::IsDisabled() const
{
	return _visualState == CellVisualState::Disabled;
}

bool Cell::HasPieceStored(const Piece** outFoundPiece)
{
	if (_pieceHere!=nullptr && outFoundPiece!=nullptr) *outFoundPiece = _pieceHere;
	/*Utils::Log(std::format("TURN: HAS PIECE STORED: {} OUT: {}", 
		_pieceHere == nullptr ? "NULL" : _pieceHere->ToString(),
		outFoundPiece==nullptr? "NULL PTR" : *outFoundPiece==nullptr? "NULL" : (*outFoundPiece)->ToString()));*/

	return _pieceHere != nullptr;
}

bool Cell::HasPiece(const Piece** outFoundPiece)
{
	if (_pieceHere!=nullptr) Utils::Log(std::format("Has piece check existing data: {}", _pieceHere->ToString()));
	return HasPieceStored(outFoundPiece) || IsRenderingPiece();
}

void Cell::SetPiece(const Piece* piece, wxImage& image)
{
	//pieceHere = &piece;
	//TODO: assignment does nto work since it is not defined for Piece so peiceHere is always NULL!
	_pieceHere = piece;
	Utils::Log(std::format("UPDATE PIECE NOW: {}", _pieceHere==nullptr? "NULL" : _pieceHere->ToString()));
	
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
	
	wxBitmap map = GetBitMapFromSprite(image, targetSize);
	_bitMapDisplay->SetBitmap(map);
	//_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, map, this->GetPosition());
	_bitMapDisplay->Center();
	_isRenderingPiece = true;

	std::string posCenter = std::to_string(_bitMapDisplay->GetPosition().x)+std::to_string(_bitMapDisplay->GetPosition().y);
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
		_bitMapDisplay->SetBitmap({});
		_isRenderingPiece = false;
		renderingOrPieceRemoved = true;
		Utils::Log(std::format("PIECE CHECK TRY REMOVE PIECE: {}",
			_pieceHere == nullptr ? "NULL" : _pieceHere->ToString()));
	}

	if (HasPieceStored(nullptr))
	{
		_pieceHere = nullptr;
		if (!renderingOrPieceRemoved) 
			renderingOrPieceRemoved = true;
	}
	
	return renderingOrPieceRemoved;
}

void Cell::SetOverlaySprite(const wxBitmap& bitmap)
{
	_hasOverlayImage = true;
	_overlayPanel->SetBitmap(bitmap);
	_overlayPanel->Center();
	Refresh();
}

void Cell::RemoveOverlaySprite()
{
	_hasOverlayImage = false;
	_overlayPanel->SetBitmap({});
	Refresh();
}

void Cell::UpdateCanClick(bool isClickable)
{
	_isClickable = isClickable;

	Utils::Log(std::format("TURN: Tried to make cell disabled"));
	if (_isClickable) SetVisualState(CellVisualState::Default);
	else SetVisualState(CellVisualState::Disabled);
}	

void Cell::SetVisualState(const CellVisualState& state)
{
	auto stateSpriteIt = _stateSprites.find(state);
	bool hasStateSprite = stateSpriteIt != _stateSprites.end();
	Utils::Log(std::format("LOADING State sprite it: {} for state: {}", 
		std::to_string(_stateSprites.find(state) != _stateSprites.end()), ToString(state)));
	//return;
	
	//std::optional<const wxColour*> maybeStateColor = TryGetColorForState(state);
	if (HasOverlayImage) RemoveOverlaySprite();

	if (state == CellVisualState::Default)
	{
		SetBackgroundColour(_colors.InnerColor);
	}
	else if (state == CellVisualState::Selected)
	{
		SetBackgroundColour(_colors.SelectedColor);
	}
	else if (state == CellVisualState::PossibleMoveHighlighted)
	{
		if (HasPiece()) SetBackgroundColour(_colors.PossibleCaptureColor);
		else if (hasStateSprite) SetOverlaySprite(*(stateSpriteIt->second));
	}
	else if (state == CellVisualState::PreviousMoveHighlighted)
	{
		SetBackgroundColour(_colors.PreviousMoveColor);
	}
	else if (state == CellVisualState::Disabled)
	{
		int grayCoefficient = 0.3;
		wxColour currentColor = _colors.InnerColor;
		float averageRGB = (currentColor.Red() + currentColor.Green() + currentColor.Blue()) / 3;
		float newR = currentColor.Red() + grayCoefficient * (averageRGB- currentColor.Red());
		float newG= currentColor.Green() + grayCoefficient * (averageRGB - currentColor.Green());
		float newB = currentColor.Blue() + grayCoefficient * (averageRGB - currentColor.Blue());

		int newRInt = static_cast<int>(newR*255);
		int newGInt = static_cast<int>(newG*255);
		int newBInt = static_cast<int>(newB*255);

		Utils::Log(std::format("TURN: Tried to make cell disabled with start color: {} new color:{}",
			WXUtils::ToString(currentColor), WXUtils::ToString(wxColour(newRInt, newGInt, newBInt))));
		SetBackgroundColour(wxColour(newRInt, newGInt, newBInt));
		//SetOverlaySprite(*(stateSpriteIt->second));
	}
	else
	{
		const std::string err = std::format("Tried to update visual state for cell for a type "
			"that is not defined with its own instructions!");
		Utils::Log(Utils::LogType::Error, err);
		return;
	}

	_visualState = state;
	Refresh();
}

void Cell::ToggleVisualState(const CellVisualState& state)
{
	if (_visualState == CellVisualState::Default) SetVisualState(state);
	else SetVisualState(CellVisualState::Default);
}

void Cell::ResetVisualToDefault()
{
	return SetVisualState(CellVisualState::Default);
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