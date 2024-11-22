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

	/// <summary>
	/// Will return true if a piece is rendered in the cell
	/// NOTE: not the same as having it since it may still be null
	/// </summary>
	/// <returns></returns>
	bool IsRenderingPiece() const;

	//TODO: maybe make a custom wrapper type called OutResult<> that can be used with pointers
	//and prohibits nullptr so that out parameters can work as expected

	/// <summary>
	/// Will return true if there is piece data stored in the cell
	/// NOTE: not the same as rendering since it may be rendered without being stored
	/// NOTE: for out parameters to work correctly, the piece** must NOT be nullptr
	/// so that a vaiable address is used to reroute pointer to Piece
	/// It can be set up like this: const Piece* ptr= nullptr; const Piece** ptr2= &ptr;
	/// </summary>
	/// <param name="outFoundPiece"></param>
	/// <returns></returns>
	bool HasPieceStored(const Piece** outFoundPiece = nullptr);

	/// <summary>
	/// Will return true if a piece is stored in the cell OR is rendered
	/// NOTE: for out parameters to work correctly, the piece** must NOT be nullptr
	/// so that a vaiable address is used to reroute pointer to Piece
	/// It can be set up like this: const Piece* ptr= nullptr; const Piece** ptr2= &ptr;
	/// </summary>
	/// <param name="outFoundPiece"></param>
	/// <returns></returns>
	bool HasPiece(const Piece** outFoundPiece = nullptr);

	//TODO: change from pointer to reference
	void UpdatePiece(const Piece* piece, wxImage& image);
	bool TryRemovePiece();

	void AddOnClickCallback(const std::function<void(Cell*)>& callback);
	
	void SetHighlighted(bool doHighlight);
	void ToggleHighlighted();
	void UpdateCanClick(bool isClickable);
};

