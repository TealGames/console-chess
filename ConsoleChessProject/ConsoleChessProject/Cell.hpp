#pragma once
#include <wx/wx.h>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map> 
#include <functional>
#include "Piece.hpp"

const wxSize CELL_SIZE{ 50, 50 };
constexpr float ICON_SIZE_TO_CELL = 0.7;

//TODO: change cell system to use states rather than knowing about the implementation
//of different types of highlighting and symbol setting
enum class CellState
{
	Default,
	Selected,
	PossibleMoveHighlighted,
	PreviousMoveHighlighted,
	Disabled,
};

std::string ToString(const CellState& state);

//enum class HighlightColorType
//{
//	Selected,
//	PossibleMove,
//	PreviousMove,
//};

struct CellColors
{
	wxColour InnerColor;
	wxColour HoverColor;
	wxColour SelectedColor;
	wxColour PossibleMoveColor;
	wxColour PossibleCaptureColor;
	wxColour PreviousMoveColor;
};

//class Cell;
//using CellClickCallbackType = std::function<void(Cell*)>;
class Cell : public wxPanel
{
private:
	static constexpr bool m_UPDATE_IMAGE_SIZE = true;
	
	std::vector<std::function<void(Cell*)>> m_onClickCallbacks;
	bool m_isClickable;
	bool m_hasOverlayImage;
	bool m_isRenderingPiece;

	CellColors m_colors;	
	wxColour m_lastColor;
	
	const std::unordered_map<CellState, wxBitmap*> m_stateSprites;
	CellState m_currentState;

	//std::optional<HighlightColorType> _highlightedType;
	const Piece* m_pieceHere;

	wxStaticBitmap* m_bitMapDisplay = nullptr;
	wxStaticBitmap* m_overlayPanel = nullptr;

public:
	const bool& m_IsClickable;
	const bool& m_HasOverlayImage;
	const CellState& m_CurrentState;
	const ArmyColor m_TileColor;

private:
	//const wxColour& GetHighlightColor(const HighlightColorType highlightType) const;
	//std::optional<const wxColour*> TryGetColorForState(const CellVisualState& state);
	void SetOverlaySprite(const wxBitmap& bitmap);
	void RemoveOverlaySprite();

	void OnEnter(wxMouseEvent& evt);
	void OnExit(wxMouseEvent& evt);
	void OnClick(wxMouseEvent& evt);

	//void SkipMouseEvent(wxMouseEvent& evt);

public:
	Cell(wxWindow* parent, wxPoint pos, const ArmyColor& tileColor, const CellColors& colors, 
		const std::unordered_map<CellState, wxBitmap*>& stateSprites);

	bool IsHighlighted() const;
	//std::optional<HighlightColorType> GetHighlightedColorType() const;

	/// <summary>
	/// Will return true if a piece is rendered in the cell
	/// NOTE: not the same as having it since it may still be null
	/// </summary>
	/// <returns></returns>
	bool IsRenderingPiece() const;

	/// <summary>
	/// Return true if the state of this cell is disabled
	/// </summary>
	/// <returns></returns>
	bool IsDisabled() const;

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
	void SetPiece(const Piece* piece, wxImage& image);
	bool TryRemovePiece();

	void SetCellColors(const CellColors& colors);

	void UpdateCanClick(const bool isClickable, const bool updateVisual);
	void AddOnClickCallback(const std::function<void(Cell*)>& callback);

	void SetState(const CellState& state, const bool updateVisual);
	void ToggleState(const CellState& state);
	void ResetStateToDefault();
	
	
	/// <summary>
	/// Will highlight the cell with a color based on the type.
	/// </summary>
	/// <param name="highlightType"></param>
	/// <param name="actionCondition"></param>
	//void Highlight(const HighlightColorType& highlightType);
	//void Dehighlight();

	//void ToggleHighlighted(const HighlightColorType& highlightTypeIfToggle);
};

