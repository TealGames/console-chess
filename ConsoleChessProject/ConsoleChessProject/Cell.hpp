#pragma once
#include <wx/wx.h>
#include "Piece.hpp"

const wxSize CELL_SIZE{ 50, 50 };
constexpr float ICON_SIZE_TO_CELL = 0.7;

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
	const Piece* pieceHere;

public:
	bool& IsClickable;

private:
	
	void OnEnter(wxMouseEvent& evt);
	void OnExit(wxMouseEvent& evt);
	void OnClick(wxMouseEvent& evt);

	void SkipMouseEvent(wxMouseEvent& evt);

public:
	Cell(wxWindow* parent, wxPoint pos, const CellColors& colors);

	bool HasPiece(const Piece* outFoundPiece=nullptr);

	//TODO: change from pointer to reference
	void UpdatePiece(const Piece* piece, wxImage& image);
	bool TryRemovePiece();

	using CellCallbackType = std::function<void(const Cell*)>;
	void AddOnClickCallback(const CellCallbackType& callback);
	void UpdateCanClick(bool isClickable);
};

