#pragma once
#include <wx/wx.h>

const wxSize CELL_SIZE{ 50, 50 };
constexpr int BORDER_WIDTH = 5;

struct CellColors
{
	wxColour innerColor;
	wxColour hoverColor;
};

class Cell : public wxPanel
{
private:
	const CellColors _colors;	
	wxStaticBitmap* _bitMapDisplay;
	bool _isClickable;

public:
	bool& IsClickable;

private:
	void OnEnter(wxMouseEvent& evt);
	void OnExit(wxMouseEvent& evt);
	void OnClick(wxMouseEvent& evt);

public:
	Cell(wxWindow* parent, wxPoint pos, const CellColors& colors);

	using CellCallbackType = std::function<void(const Cell*)>;
	void AddOnClickCallback(const CellCallbackType& callback);
	void UpdateImage(const wxImage& image);
	void UpdateCanClick(bool isClickable);
};

