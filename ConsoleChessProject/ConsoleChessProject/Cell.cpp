#include <wx/wx.h>
#include <string>
#include <vector>
#include <algorithm>
#include "Cell.hpp"

static std::vector<Cell::CellCallbackType> onClickCallbacks;

Cell::Cell(wxWindow* parent, wxPoint pos, const CellColors& colors)
	: wxPanel(parent, wxID_ANY, pos, CELL_SIZE), _colors(colors), _bitMapDisplay(nullptr),
	_isClickable(true), IsClickable(_isClickable)
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

void Cell::UpdateImage(const wxImage& image)
{
	wxBitmap map(image);
	_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, map);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(_bitMapDisplay, 0, wxALL | wxCENTER, 10);
	SetSizerAndFit(sizer);
	Refresh();
}

void Cell::UpdateCanClick(bool isClickable)
{
	isClickable = isClickable;
}
