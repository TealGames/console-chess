#pragma once
#include <wx/wx.h>
#include <vector>
#include <functional>
#include "Piece.hpp"

const wxSize CELL_SIZE{ 50, 50 };
constexpr float ICON_SIZE_TO_CELL = 0.7;

struct CellColors
{
	wxColour InnerColor;
	wxColour HoverColor;
	wxColour HighlightedColor;
};

//class Cell;
//using CellClickCallbackType = std::function<void(Cell*)>;
class Cell : public wxPanel
{
private:
	std::vector<std::function<void(Cell*)>> _onClickCallbacks;

	const CellColors _colors;	
	wxStaticBitmap* _bitMapDisplay;
	bool _isClickable;

	bool _isHighlighted;

	const Piece* pieceHere;

	static constexpr bool _UPDATE_IMAGE_SIZE = true;

public:
	const bool& IsClickable;
	const bool& IsHighlighted;

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

	void AddOnClickCallback(const std::function<void(Cell*)>& callback);
	
	void SetHighlighted(bool doHighlight);
	void ToggleHighlighted();
	void UpdateCanClick(bool isClickable);
};

