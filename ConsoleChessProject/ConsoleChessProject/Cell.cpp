#include <wx/wx.h>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "Cell.hpp"
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

bool Cell::HasPiece(const Piece* outFoundPiece)
{
	outFoundPiece = pieceHere;
	return pieceHere != nullptr;
}

void Cell::UpdatePiece(const Piece* piece, wxImage& image)
{
	//pieceHere = &piece;
	pieceHere = piece;
	
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
	if (!HasPiece()) return false;
	pieceHere = nullptr;
	return true;
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