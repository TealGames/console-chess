#include <wx/wx.h>
#include <string>
#include "Cell.hpp"

Cell::Cell(wxPanel* parent, wxPoint pos, const CellColors& colors) :
	wxButton(parent, wxID_ANY, "", pos, CELL_SIZE), colors(colors)
{
	Bind(wxEVT_ENTER_WINDOW, &Cell::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &Cell::OnExit, this);

	SetBackgroundColour(colors.borderColor);

	wxSize innerSize(CELL_SIZE.x-2*BORDER_WIDTH, CELL_SIZE.y-2*BORDER_WIDTH);
	innerPanel = new wxPanel(this, wxID_ANY, wxPoint(pos.x+BORDER_WIDTH, pos.y+BORDER_WIDTH), innerSize);
	innerPanel->SetBackgroundColour(colors.innerColor);
}

void Cell::OnEnter(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	this->SetBackgroundColour(colors.hoverColor);
}

void Cell::OnExit(wxMouseEvent& evt)
{
	Cell* cell = dynamic_cast<Cell*>(evt.GetEventObject());
	if (!cell) return;

	this->SetBackgroundColour(colors.innerColor);
}
