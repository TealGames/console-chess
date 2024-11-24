#include <wx/wx.h>
#include <format>
#include <vector>
#include <set>
#include <optional>
#include <unordered_map>
#include "BoardUI.hpp"
#include "UIGlobals.hpp"
#include "BoardManager.hpp"
#include "ResourceManager.hpp"
//#include "GameManager.hpp"
#include "Point2D.hpp"
#include "Piece.hpp"
#include "Cell.hpp"

static std::unordered_map<Utils::Point2DInt, Cell*> cells;
static Cell* lastSelected;
static std::vector<Cell*> previousMoveCells;
static std::vector<Cell*> currentCellMoves;
static const std::unordered_map<ColorTheme, CellColors> cellColorData =
{
	{ColorTheme::Light, CellColors{LIGHT_CELL_COLOR, LIGHT_CELL_HOVER_COLOR, LIGHT_CELL_SELECTED_COLOR, 
								   LIGHT_CELL_MOVE_COLOR, LIGHT_CELL_PREVIOUS_MOVE_COLOR}},
	{ColorTheme::Dark, CellColors{DARK_CELL_COLOR, DARK_CELL_HOVER_COLOR, DARK_CELL_SELECTED_COLOR,
								   DARK_CELL_MOVE_COLOR, DARK_CELL_PREVIOUS_MOVE_COLOR}}
};

static std::optional<wxBitmap> movePositionIcon;

Cell* TryGetCellAtPosition(const Utils::Point2DInt point)
{
	auto it = cells.find(point);
	if (it == cells.end()) return nullptr;
	else return it->second;
}

std::optional<Utils::Point2DInt> TryGetPositionOfCell(const Cell& cell)
{
	for (const auto& cellPos : cells)
	{
		if (cellPos.second != nullptr && cellPos.second == &cell)
		{
			return cellPos.first;
		}
	}
	return std::nullopt;
}

void UpdateInteractablePieces(const ColorTheme& interactableColor)
{
	const Piece* cellPiece = nullptr;
	for (const auto& cell : cells)
	{
		if (!cell.second->HasPiece(&cellPiece) ||
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
			bool isDarkCell = (r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1);
			auto cellColorDataIt = cellColorData.find(isDarkCell? ColorTheme::Dark : ColorTheme::Light);
			if (cellColorDataIt == cellColorData.end())
			{
				const std::string err = std::format("Tried to get color data for tile color but was not found");
				Utils::Log(Utils::LogType::Error, err);
				return;
			}

			currentPoint = wxPoint(gridStartX + c * CELL_SIZE.x, gridStartY + r * CELL_SIZE.y);
			
			auto emplaced = cells.emplace(Utils::Point2DInt(r, c), new Cell(parent, currentPoint, cellColorDataIt->second));
			if (!emplaced.second)
			{
				const std::string error = std::format("Tried to place cell at row {} col {}"
					"but failed to emplace data into cell vector.emplace()", std::to_string(r), std::to_string(c));
				Utils::Log(Utils::LogType::Error, error);
				return;
			}
		}
	}
	
	//AddTurnChangeCallback(&UpdateInteractablePieces);

	//TODO: this should be done elsewhere not in the UI!
	//CreateDefaultBoard();
}

static bool IsStoredAsPreviousMove(const Cell* cellCheck)
{
	if (previousMoveCells.empty()) return false;
	for (const auto& cell : previousMoveCells)
	{
		if (cell== cellCheck) return true;
	}
	return false;
}

static void DeselectHighlightedCells()
{
	if (currentCellMoves.empty()) return;

	for (const auto& cell : currentCellMoves)
	{
		if (cell->IsHighlighted() && !IsStoredAsPreviousMove(cell))
		{
			cell->Dehighlight();
		}
		if (cell->HasOverlayImage) cell->RemoveOverlaySprite();
	}
	currentCellMoves.clear();
}

static wxBitmap& GetMoveIcon()
{
	if (!movePositionIcon.has_value())
	{
		movePositionIcon = TryGetBitMapForIcon(SpriteSymbolType::MoveSpot, CELL_SIZE);
	}
	Utils::Log(std::format("LOADING: has icon: {}", std::to_string(movePositionIcon.has_value())));
	return movePositionIcon.value();
}

static void PrintCells()
{
	std::vector<Utils::Point2DInt> cellPositionsWith;
	for (const auto& element : cells)
	{
		if (element.second->HasOverlayImage) cellPositionsWith.push_back(element.first);
	}

	std::vector<Utils::Point2DInt> cellMoves;
	for (const auto& cell : currentCellMoves)
	{
		cellMoves.push_back(TryGetPositionOfCell(*cell).value());
	}
	const std::string str = std::format("LOADING: positions with overlay: {}. Current move pos: {}", 
		Utils::ToStringIterable<std::vector<Utils::Point2DInt>, Utils::Point2DInt>(cellPositionsWith),
		Utils::ToStringIterable<std::vector<Utils::Point2DInt>, Utils::Point2DInt>(cellMoves));
	Utils::Log(str);
}

void BindCellEventsForGameState(GameState& state)
{
	for (const auto& cell : cells)
	{
		cell.second->AddOnClickCallback([&state, &cell](Cell* clickedCell) -> void
		{
#pragma region Move Updating
			if (!currentCellMoves.empty())
			{
				for (const auto& cell : currentCellMoves)
				{
					Utils::Log(Utils::LogType::Error, std::format("SELECTED {} MOVES: {} CLICKED: {}",
						std::to_string(currentCellMoves.size()), Utils::ToStringIterable<std::vector<Cell*>, Cell*>(currentCellMoves),
						std::to_string(cell == clickedCell)));
					 
					if (!cell->HasOverlayImage && cell != clickedCell || lastSelected == nullptr) continue;

					std::optional<Utils::Point2DInt> startPos = TryGetPositionOfCell(*lastSelected);
					if (startPos == std::nullopt)
					{
						const std::string error = std::format("Tried find start pos for move to cell "
							"but failed to be retrieved!");
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					std::optional<Utils::Point2DInt> endPos = TryGetPositionOfCell(*clickedCell);
					if (endPos == std::nullopt)
					{
						const std::string error = std::format("Tried find end pos for move to cell "
							"but failed to be retrieved!");
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					Board::MoveResult result = Board::TryMove(state, startPos.value(), endPos.value());
					if (!result.IsValidMove)
					{
						const std::string error = std::format("Tried update the the move of piece state from pos "
							"{} -> {} but failed! Info: {}", startPos.value().ToString(),
							endPos.value().ToString(), result.Info);
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					if (!TryRenderUpdateCells(state, std::vector<Utils::Point2DInt>{ startPos.value(), endPos.value() }))
					{
						const std::string error = std::format("Tried update the rendering for cells "
							"{} -> {} but failed!", startPos.value().ToString(), endPos.value().ToString());
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					if (!previousMoveCells.empty())
					{
						for (const auto& cell : previousMoveCells) cell->Dehighlight();
						previousMoveCells.clear();
					}

					DeselectHighlightedCells();
					lastSelected->Highlight(HighlightColorType::PreviousMove);
					clickedCell->Highlight(HighlightColorType::PreviousMove);

					//We need to store the cell that was last selected (the start pos of move)
					//so we can disable it on next move since it is not stored definitively anywhere else
					previousMoveCells.push_back(lastSelected);
					previousMoveCells.push_back(clickedCell);
					lastSelected = clickedCell;
					return;
				}
			}
			DeselectHighlightedCells();
#pragma endregion

			//We don't want to do any highlighting if it is a cell with no pieces
			if (!clickedCell->HasPiece())
			{
				lastSelected = clickedCell;
				return;
			}

			//TODO: for each cell try to create states to check like highlighted, selected, previous moves shown, etc
			//to obfuscate what changes actually occur from state

#pragma region Toggling Highlight
			bool sameCellClickedAgain = lastSelected == clickedCell;

			//We want to preserve previous old move highlighted status
			//so we must check for what type is highlighted and what to set as new
			if (sameCellClickedAgain)
			{
				if (IsStoredAsPreviousMove(lastSelected))
				{
					if (lastSelected->GetHighlightedColorType() == HighlightColorType::PreviousMove)
						lastSelected->Highlight(HighlightColorType::Selected);
					else lastSelected->Highlight(HighlightColorType::PreviousMove);
				}
				else lastSelected->ToggleHighlighted(HighlightColorType::Selected);
			}
			else
			{
				if (lastSelected != nullptr)
				{
					if (IsStoredAsPreviousMove(lastSelected)) lastSelected->Highlight(HighlightColorType::PreviousMove);
					else lastSelected->Dehighlight();
				}

				clickedCell->Highlight(HighlightColorType::Selected);
			}
#pragma endregion

#pragma region Possible Move Highlighting
			if (clickedCell->IsHighlighted())
			{
				std::vector<MoveInfo> possibleMoves = Board::GetPossibleMovesForPieceAt(state, cell.first);
				//if (possibleMoves.empty()) wxLogMessage("Poop");
				Utils::Log(Utils::LogType::Error, std::format("POSSIBLE {} MOVES: {}",
					std::to_string(possibleMoves.size()), Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(possibleMoves)));

				Cell* cellAtPosition = nullptr;
				for (const auto& move : possibleMoves)
				{
					for (const auto& piecesMoved : move.PiecesMoved)
					{
						cellAtPosition = TryGetCellAtPosition(piecesMoved.NewPos);
						if (cellAtPosition == nullptr) continue;

						//cellAtPosition->Highlight(HighlightColorType::PossibleMove);
						cellAtPosition->SetOverlaySprite(GetMoveIcon());
						currentCellMoves.push_back(cellAtPosition);
					}
				}
				PrintCells();
			}

			//We make sure to rehighlight them as previous moves
			//in case one might have been selected as a possible move before
			else
			{
				for (const auto& cell : previousMoveCells) 
					cell->Highlight(HighlightColorType::PreviousMove);
			}

			lastSelected = clickedCell;
#pragma endregion
		});
	}
}

static void DisplayPieceMoves()
{
	//TODO: add display piece moves logic
}

bool TryRenderPieceAtPos(const Utils::Point2DInt& pos, const Piece* pieceInfo)
{
	//TryCacheAllSprites();
	if (!Board::IsWithinBounds(pos))
	{
		std::string err = std::format("Tried to render piece {} at pos {} "
			"but it is out of board bounds", pieceInfo->ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}

	auto cellIt= cells.find(pos);
	if (cellIt == cells.end())
	{
		std::string err = std::format("Tried to render piece {} at pos {} but pos is "
			"not found in cached cells", pieceInfo->ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}
	
	/*auto imagePairIt = pieceSprites.find(PieceTypeInfo{pieceInfo->color, pieceInfo->pieceType});
	if (imagePairIt == pieceSprites.end())
	{
		std::string err = std::format("Tried to render piece {} at pos {} but its sprite "
			"is not cached", pieceInfo->ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}*/
	auto maybeSprite = TryGetSpriteFromPiece(PieceTypeInfo{ pieceInfo->color, pieceInfo->pieceType });
	if (!maybeSprite.has_value())
	{
		std::string err = std::format("Tried to render piece {} at pos {} but its sprite "
			"is not found", pieceInfo->ToString(), pos.ToString());
		Utils::Log(Utils::LogType::Error, err);
		return false;
	}


	//TODO: change from nullptr to actual piece
	Utils::Log(std::format("PIECE CHECK Need to update render sprite for piece {} at pos: {}", pieceInfo->ToString(), pos.ToString()));
	cellIt->second->UpdatePiece(pieceInfo, maybeSprite.value());
	return true;
}

bool TryRenderAllPieces(const GameState& state)
{
	const std::unordered_map<Utils::Point2DInt, Piece>& pieces = state.PiecePositions;
	const std::string message = std::format("A total of pieces: {}", std::to_string(pieces.size()));
	wxLogMessage(message.c_str());
	for (const auto& pieceData : pieces)
	{
		//PieceTypeInfo pieceInfo = {pieceData.second.color, pieceData.second.pieceType};
		Utils::Log(std::format("Try render all pieces piece: {} {}", pieceData.first.ToString(), pieceData.second.ToString()));
		if (!TryRenderPieceAtPos(pieceData.first, &pieceData.second))
		{
			const std::string err = std::format("Tried to render all pieces but failed to do it for piece {} at {}", 
				pieceData.second.ToString(), pieceData.first.ToString());
			Utils::Log(Utils::LogType::Error, err);
			return false;
		}
	}
	return true;
}

bool TryRenderUpdateCells(const GameState& state, std::vector<Utils::Point2DInt> positions)
{
	for (const auto& updatePos : positions)
	{
		Utils::Log(std::format("PIECE CHECK Update render pos: {}", updatePos.ToString()));
		auto cellPairIt = cells.find(updatePos);
		if (cellPairIt == cells.end())
		{
			std::string err = std::format("Tried to update render for cell at pos {} "
				"but position is not found in cached cells", updatePos.ToString());
			Utils::Log(Utils::LogType::Error, err);
			return false;
		}

		auto piecePairIt = state.PiecePositions.find(updatePos);
		Utils::Log(std::format("Piece positions: {}", Utils::ToStringIterable(state.PiecePositions)));
		
		const Piece* updatedPiecePtr = &(piecePairIt->second);
		if (piecePairIt != state.PiecePositions.end())
			Utils::Log(std::format("Piece At new pos: {} is {}", updatePos.ToString(), updatedPiecePtr->ToString()));

		Utils::Log(std::format("Try render update cells for pos: {} current: {} old has: {} new has: {}", 
			Utils::ToStringIterable<std::vector<Utils::Point2DInt>, Utils::Point2DInt>(positions),
			updatePos.ToString(), std::to_string(cellPairIt->second->HasPiece(nullptr)),
			std::to_string(piecePairIt != state.PiecePositions.end())));

		const Piece* cellOldDataPtr = nullptr;
		const Piece** cellOldDataPtrPtr = &cellOldDataPtr;
		bool cellAlreadyHasPiece = cellPairIt->second->HasPiece(cellOldDataPtrPtr);

		Utils::Log(std::format("PIECE CHECK update render cells for pos {}: {}",
			updatePos.ToString(), cellOldDataPtrPtr == nullptr ? "NULL PTR" : *cellOldDataPtrPtr == nullptr ? "NULL" : (*cellOldDataPtrPtr)->ToString()));

		//If the new data does not have piece here, we remove sprite
		if (piecePairIt == state.PiecePositions.end() && cellAlreadyHasPiece)
		{
			Utils::Log(std::format("PIECE CHECK removing piece at pos {}", updatePos.ToString()));
			cellPairIt->second->TryRemovePiece();
		}

		//If new data has new piece here we update sprite
		else if ((piecePairIt != state.PiecePositions.end() && !cellAlreadyHasPiece) || 
			(cellAlreadyHasPiece && cellOldDataPtrPtr != nullptr && *cellOldDataPtrPtr != updatedPiecePtr))
		{
			Utils::Log(std::format("PIECE CHECK udpate or add rendering piece at pos {}", updatePos.ToString()));
			TryRenderPieceAtPos(updatePos, updatedPiecePtr);
		}

	}
	return true;
}


void EndCleanup()
{

}