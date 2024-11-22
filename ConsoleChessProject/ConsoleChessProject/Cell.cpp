#include <wx/wx.h>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "Cell.hpp"
#include "HelperFunctions.hpp"
#include "ResourceManager.hpp"

Cell::Cell(wxWindow* parent, wxPoint pos, const CellColors& colors)
	: wxPanel(parent, wxID_ANY, pos, CELL_SIZE), _colors(colors), _bitMapDisplay(nullptr),
	_isClickable(true), IsClickable(_isClickable), _isHighlighted(false), IsHighlighted(_isHighlighted),
	pieceHere(nullptr), _onClickCallbacks()
{
	Bind(wxEVT_ENTER_WINDOW, &Cell::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &Cell::OnExit, this);
	Bind(wxEVT_LEFT_DOWN, &Cell::OnClick, this);

	SetBackgroundColour(colors.InnerColor);
}

void Cell::OnEnter(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	//If we are highlighted we do not want to return back until the caller says so
	if (IsHighlighted) return;

	SetBackgroundColour(_colors.HoverColor);
	Refresh();
}

void Cell::OnExit(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	//If we are highlighted we do not want to return back until the caller says so
	if (IsHighlighted) return;

	SetBackgroundColour(_colors.InnerColor);
	Refresh();
}

void Cell::OnClick(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	for (const auto& callback : _onClickCallbacks) callback(this);

	Refresh();
	//wxLogMessage("Click");
}

void Cell::AddOnClickCallback(const std::function<void(Cell*)>& callback)
{
	_onClickCallbacks.push_back(callback);
}

bool Cell::IsRenderingPiece() const
{
	return _bitMapDisplay!=nullptr;
}

bool Cell::HasPieceStored(const Piece** outFoundPiece)
{
	outFoundPiece = &pieceHere;
	Utils::Log(std::format("HAS PIECE STORED: {} OUT: {}", 
		pieceHere == nullptr ? "NULL" : pieceHere->ToString(),
		outFoundPiece==nullptr? "NULL PTR" : *outFoundPiece==nullptr? "NULL" : (*outFoundPiece)->ToString()));

	return pieceHere != nullptr;
}

bool Cell::HasPiece(const Piece** outFoundPiece)
{
	if (pieceHere!=nullptr) Utils::Log(std::format("Has piece check existing data: {}", pieceHere->ToString()));
	return HasPieceStored(outFoundPiece) || IsRenderingPiece();
}

void Cell::UpdatePiece(const Piece* piece, wxImage& image)
{
	//pieceHere = &piece;
	//TODO: assignment does nto work since it is not defined for Piece so peiceHere is always NULL!
	pieceHere = piece;
	Utils::Log(std::format("UPDATE PIECE NOW: {}", pieceHere==nullptr? "NULL" : pieceHere->ToString()));
	
	wxSize startSize(image.GetWidth(), image.GetHeight());
	wxSize targetSize(static_cast<int>(ICON_SIZE_TO_CELL * CELL_SIZE.x), 
					  static_cast<int>(ICON_SIZE_TO_CELL * CELL_SIZE.y));
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
	}
	
	wxBitmap map(image);
	_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, map, this->GetPosition());
	_bitMapDisplay->Center();

	std::string posCenter = std::to_string(_bitMapDisplay->GetPosition().x)+std::to_string(_bitMapDisplay->GetPosition().y);
	wxLogMessage(posCenter.c_str());

	/*_bitMapDisplay->Bind(wxEVT_ENTER_WINDOW, &Cell::SkipMouseEvent, this);
	_bitMapDisplay->Bind(wxEVT_LEAVE_WINDOW, &Cell::SkipMouseEvent, this);
	_bitMapDisplay->Bind(wxEVT_LEFT_DOWN, &Cell::SkipMouseEvent, this);*/

	//This is done to allow events to be ignored by the child so it would reach the parent (the cell)
	_bitMapDisplay->Enable(false);

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
		delete _bitMapDisplay;
		_bitMapDisplay = nullptr;
		renderingOrPieceRemoved = true;
		Utils::Log(std::format("PIECE CHECK TRY REMOVE PIECE: {}",
			pieceHere == nullptr ? "NULL" : pieceHere->ToString()));
	}

	if (HasPieceStored(nullptr))
	{
		pieceHere = nullptr;
		if (!renderingOrPieceRemoved) 
			renderingOrPieceRemoved = true;
	}
	
	return renderingOrPieceRemoved;
}

void Cell::UpdateCanClick(bool isClickable)
{
	isClickable = isClickable;
}

void Cell::SetHighlighted(bool doHighlight)
{
	_isHighlighted = doHighlight;
	/*if (doHighlight) wxLogMessage("Highlgihted");
	else wxLogMessage("Not highlighted!");*/

	if (doHighlight) SetBackgroundColour(_colors.HighlightedColor);
	else SetBackgroundColour(_colors.InnerColor);
	Refresh();
}

void Cell::ToggleHighlighted()
{
	SetHighlighted(!_isHighlighted);
}