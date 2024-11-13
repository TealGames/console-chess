#include <wx/wx.h>
#include <string>
#include <vector>
#include <algorithm>
#include "Cell.hpp"
#include "ResourceManager.hpp"

static std::vector<Cell::CellCallbackType> onClickCallbacks;
static bool updateImageSize = true;

Cell::Cell(wxWindow* parent, wxPoint pos, const CellColors& colors)
	: wxPanel(parent, wxID_ANY, pos, CELL_SIZE), _colors(colors), _bitMapDisplay(nullptr),
	_isClickable(true), IsClickable(_isClickable), pieceHere(nullptr)
{
	Bind(wxEVT_ENTER_WINDOW, &Cell::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &Cell::OnExit, this);
	Bind(wxEVT_LEFT_DOWN, &Cell::OnClick, this);

	SetBackgroundColour(colors.innerColor);
}

void Cell::OnEnter(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	SetBackgroundColour(_colors.hoverColor);
	Refresh();
}

void Cell::OnExit(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	SetBackgroundColour(_colors.innerColor);
	Refresh();
}

void Cell::OnClick(wxMouseEvent& evt)
{
	if (!IsClickable) return;

	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	for (const auto& callback : onClickCallbacks) callback(this);

	SetBackgroundColour(_colors.hoverColor);
	Refresh();

	wxLogMessage("Click");
}

void Cell::AddOnClickCallback(const CellCallbackType& callback)
{
	onClickCallbacks.push_back(callback);

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
	
	if (updateImageSize)
	{
		wxSize startSize(image.GetWidth(), image.GetHeight());
		float newWidthScale = ICON_SIZE_TO_CELL * CELL_SIZE.x / startSize.x;
		float newHeightScale = ICON_SIZE_TO_CELL * CELL_SIZE.y / startSize.y;
		float totalScale = std::max(newWidthScale, newHeightScale);
		wxSize size(image.GetWidth() * totalScale, image.GetHeight() * totalScale);

		Resize(image, size);
		std::string m = std::format("Start: ({}, {})  END: ({}, {})", std::to_string(startSize.x), std::to_string(startSize.y),
			std::to_string(image.GetWidth()), std::to_string(image.GetHeight()));
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
