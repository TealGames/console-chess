#include <wx/wx.h>
#include "BoardUI.hpp"
#include "UIGlobals.hpp"
#include "BoardManager.hpp"
#include "ResourceManager.hpp"
#include "GameManager.hpp"
#include "Point2D.hpp"
#include "Piece.hpp"

static std::unordered_map<Utils::Point2DInt, Cell*> cells;
static std::unordered_map<PieceTypeInfo, wxImage> pieceSprites;
static const Cell* currentSelected;

void UpdateInteractablePieces(const ColorTheme& interactableColor)
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

void CreateBoardCells(wxWindow* parent)
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
			//wxLogMessage("DREW CELLS");
		}
	}
	
	//AddTurnChangeCallback(&UpdateInteractablePieces);

	//TODO: this should be done elsewhere not in the UI!
	//CreateDefaultBoard();
}

static void DisplayPieceMoves()
{
	//TODO: add display piece moves logic
}

static bool TryCacheAllSprites()
{
	if (!pieceSprites.empty()) return false;
	if (!TryLoadAllPieceImages(&pieceSprites))
	{
		const std::string error = std::format("Tried to cache all piece sprites "
			"but failed to load all from resource manager");
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}
	return true;
}

bool TryRenderPieceAtPos(const Utils::Point2DInt& pos, const PieceTypeInfo& pieceInfo)
{
	TryCacheAllSprites();
	if (!Board::IsWithinBounds(pos))
	{
		std::string err = std::format("Tried to render piece {} at pos {} "
			"but it is out of board bounds", pieceInfo.ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}

	auto cellIt= cells.find(pos);
	if (cellIt == cells.end())
	{
		std::string err = std::format("Tried to render piece {} at pos {} but pos is "
			"not found in cached cells", pieceInfo.ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}
	
	auto imagePairIt = pieceSprites.find(pieceInfo);
	if (imagePairIt == pieceSprites.end())
	{
		std::string err = std::format("Tried to render piece {} at pos {} but its sprite "
			"is not cached", pieceInfo.ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}

	//TODO: change from nullptr to actual piece
	cellIt->second->UpdatePiece(nullptr, imagePairIt->second);
	return true;
}

bool TryRenderAllPieces(const GameState& state)
{
	const std::unordered_map<Utils::Point2DInt, Piece>& pieces = state.PiecePositions;
	const std::string message = std::format("A total of pieces: {}", std::to_string(pieces.size()));
	wxLogMessage(message.c_str());
	for (const auto& pieceData : pieces)
	{
		PieceTypeInfo pieceInfo = {pieceData.second.color, pieceData.second.pieceType};
		if (!TryRenderPieceAtPos(pieceData.first, pieceInfo))
		{
			const std::string err = std::format("Tried to render all pieces but failed to do it for piece {} at {}", 
				pieceData.second.ToString(), pieceData.first.ToString());
			Utils::Log(Utils::LogType::Error, err);
			return false;
		}
	}
	return true;
}


void EndCleanup()
{

}