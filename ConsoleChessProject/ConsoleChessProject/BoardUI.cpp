#include <wx/wx.h>
#include "BoardUI.hpp"
#include "UIGlobals.hpp"
#include "BoardManager.hpp"
#include "ResourceManager.hpp"
#include "GameManager.hpp"
#include "Point2D.hpp"

static std::unordered_map<Utils::Point2DInt, Cell*> cells;
static const Cell* currentSelected;

static void UpdateInteractablePieces(const ColorTheme& interactableColor)
{
	const Piece* cellPiece = nullptr;
	for (const auto& cell : cells)
	{
		if (!cell.second->HasPiece(cellPiece) ||
			cellPiece == nullptr) continue;

		bool isInteractableColor = cellPiece->color == interactableColor;
		cell.second->UpdateCanClick(isInteractableColor);
	}
}

void CreateBoard(wxWindow* parent)
{
	wxPoint currentPoint;

	const int gridStartX = 0;
	const int gridStartY = 0;

	for (int r = 0; r < BOARD_DIMENSION; r++)
	{
		for (int c = 0; c < BOARD_DIMENSION; c++)
		{
			CellColors cellColors;
			cellColors.innerColor = (r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1) ? DARKER_TAN : DARKER_LIGHT_GREEN;
			cellColors.hoverColor = cellColors.innerColor == DARKER_TAN ? TAN : LIGHT_GREEN;
			currentPoint = wxPoint(gridStartX + c * CELL_SIZE.x, gridStartY + r * CELL_SIZE.y);
			
			cells.emplace(Utils::Point2DInt(r, c), new Cell(parent, currentPoint, cellColors));
			cells.end()->second->AddOnClickCallback([](const Cell* cell) -> void { currentSelected = cell; });
		}
	}

	AddTurnChangeCallback(&UpdateInteractablePieces);
}

static void DisplayPieceMoves()
{
	//TODO: add display piece moves logic
}

bool TryRenderPieceAtPos(const Utils::Point2DInt& pos, const PieceType& pieceType)
{
	if (!IsWithinBounds(pos))
	{
		std::string err = std::format("Tried to render piece {} at pos {} "
			"but it is out of board bounds", ToString(pieceType), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		wxLogError(err.c_str());
		return false;
	}

	auto cellIt= cells.find(pos);
	if (cellIt == cells.end())
	{
		std::string err = std::format("Tried to render piece {} at pos {} but pos is "
			"not found in cached cells", ToString(pieceType), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		wxLogError(err.c_str());
		return false;
	}
	
	wxImage* image= new wxImage();
	bool result = TryLoadPieceImage(pieceType, image);
	if (!result || image == nullptr)
	{
		std::string err = std::format("Tried to render piece {} at pos {} but "
			"could not retrieve piece image: {}", ToString(pieceType), pos.ToString(), result);
		Utils::Log(Utils::LogType::Error, err);
		wxLogError(err.c_str());
		return false;
	}

	//TODO: change from nullptr to actual piece
	cellIt->second->UpdatePiece(nullptr, *image);
	return true;
}

void EndCleanup()
{

}