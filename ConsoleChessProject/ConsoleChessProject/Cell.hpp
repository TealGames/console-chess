#pragma once
#include <wx/wx.h>

const wxSize CELL_SIZE{ 50, 50 };
constexpr int BORDER_WIDTH = 5;

struct CellColors
{
	wxColour innerColor;
	wxColour borderColor;
	wxColour hoverColor;
};

class Cell : public wxButton
{
private:
	wxPanel* innerPanel = nullptr;
	const CellColors colors;

private:
	void OnEnter(wxMouseEvent& evt);
	void OnExit(wxMouseEvent& evt);

public:
	Cell(wxPanel* parent, wxPoint pos, const CellColors& colors);
};

