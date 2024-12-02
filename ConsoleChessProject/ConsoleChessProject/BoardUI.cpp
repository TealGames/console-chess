#include <wx/wx.h>
#include <format>
#include <vector>
#include <set>
#include <optional>
#include <unordered_map>
#include "BoardUI.hpp"
#include "UIGlobals.hpp"
#include "ResourceManager.hpp"
#include "GameManager.hpp"
#include "Point2D.hpp"
#include "Piece.hpp"
#include "Cell.hpp"
#include "PieceMoveResult.hpp"
#include "ThemeControllerUI.hpp"

static std::unordered_map<Utils::Point2DInt, Cell*> cells;
static Cell* lastSelected;
static std::vector<Cell*> previousMoveCells;
static std::vector<Cell*> currentCellMoves;
//static const std::unordered_map<ArmyColor, CellColors> cellColorData =
//{
//	{ArmyColor::Light, CellColors{LIGHT_CELL_COLOR, LIGHT_CELL_HOVER_COLOR, LIGHT_CELL_SELECTED_COLOR, 
//								   LIGHT_CELL_MOVE_COLOR, LIGHT_CELL_CAPTURE_MOVE_COLOR, LIGHT_CELL_PREVIOUS_MOVE_COLOR}},
//	{ArmyColor::Dark, CellColors{DARK_CELL_COLOR, DARK_CELL_HOVER_COLOR, DARK_CELL_SELECTED_COLOR,
//								   DARK_CELL_MOVE_COLOR, DARK_CELL_CAPTURE_MOVE_COLOR, DARK_CELL_PREVIOUS_MOVE_COLOR}}
//};

static std::unordered_map<SpriteSymbolType, wxBitmap> spriteBitmaps;

static wxBitmap& GetSpriteIcon(const SpriteSymbolType symbolType)
{
	auto spriteMapIt = spriteBitmaps.find(symbolType);
	if (!spriteBitmaps.empty() || spriteMapIt ==spriteBitmaps.end())
	{
		std::optional<wxBitmap> maybeBitmap= TryGetBitMapForIcon(SpriteSymbolType::MoveSpot, CELL_SIZE);
		spriteBitmaps.emplace(symbolType, maybeBitmap.has_value() ? maybeBitmap.value() : wxBitmap());
	}
	//Utils::Log(std::format("LOADING: has icon: {}", std::to_string(movePositionIcon.has_value())));
	return spriteMapIt->second;
}

static CellColors GetCellColorsForTheme(const Theme& theme, const ArmyColor& color)
{
	return {
		theme.CellDefaultColors.at(color),
		theme.CellHoverColors.at(color),
		theme.CellSelectedColors.at(color), 
		theme.CellMoveColors.at(color),
		theme.CellCaptureMoveColors.at(color),
		theme.CellPreviousMoveColors.at(color)
	};
}

static bool IsStoredAsPreviousMove(const Cell* cellCheck)
{
	if (previousMoveCells.empty()) return false;
	for (const auto& cell : previousMoveCells)
	{
		if (cell == cellCheck) return true;
	}
	return false;
}

static void ResetCellVisuals()
{
	if (currentCellMoves.empty()) return;

	for (const auto& cell : currentCellMoves)
	{
		/*if (cell->IsHighlighted() && !IsStoredAsPreviousMove(cell))
		{
			cell->Dehighlight();
		}
		if (cell->HasOverlayImage) cell->RemoveOverlaySprite();*/
		if (cell->m_CurrentState != CellState::Default &&
			!IsStoredAsPreviousMove(cell)) cell->ResetStateToDefault();
	}
	currentCellMoves.clear();
}

static void PrintCells()
{
	std::string str = "";
	//std::unordered_map<Cell*, CellVisualState> states;
	for (const auto cell : cells)
	{
		str += std::format("[{}: {}] ", cell.first.ToString(), ToString(cell.second->m_CurrentState));
	}
	Utils::Log(Utils::LogType::Error, std::format("State of all cells: {}", str));
	/*
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
	*/
}

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

void UpdateInteractablePieces(const ArmyColor& interactableColor)
{
	const Piece* cellPiece = nullptr;
	for (const auto& cell : cells)
	{
		bool hasPiece = cell.second->HasPiece(&cellPiece);
		//Utils::Log(std::format("TURN: updating interactable piece at pos {}: {} piece ptr: {}",
		//	cell.first.ToString(), std::to_string(hasPiece), std::to_string(cellPiece!=nullptr)));
		if (!hasPiece || cellPiece == nullptr) continue;

		bool isInteractableColor = cellPiece->m_Color == interactableColor;
		bool updateVisual = !IsStoredAsPreviousMove(cell.second);
		//cell.second->UpdateCanClick(isInteractableColor, updateVisual);
		if (isInteractableColor) cell.second->SetState(CellState::Default, true);
		else
		{
			cell.second->SetState(CellState::Disabled, true);
			Utils::Log(Utils::LogType::Error, std::format("Setting cell at {} to disabled. is: {}", 
				cell.first.ToString(), std::to_string(cell.second->IsDisabled())));
		}
	}
}

bool IsDarkCell(const Utils::Point2DInt& pos)
{
	return (pos.x % 2 == 0 && pos.y % 2 == 0) || (pos.x % 2 == 1 && pos.y % 2 == 1);
}

void CreateBoardCells(wxWindow* parent)
{
	wxPoint currentPoint;
	
	//wxPanel* cellParent = new wxPanel(parent, wxID_ANY, wxDefaultPosition, parentSize);
	//cellParent->SetBackgroundColour(RED);

	const Theme* theme = TryGetCurrentTheme();
	if (theme == nullptr)
	{
		const std::string error = std::format("Tried to create board cells "
			"but the current theme was not found");
		Utils::Log(Utils::LogType::Error, error);
		return;
	}
	AddThemeUpdatedCallback([](const Theme& newTheme)-> void {
		for (const auto& cell : cells)
		{
			cell.second->SetCellColors(GetCellColorsForTheme(newTheme, cell.second->m_TileColor));
		}});

	const int gridStartX = (parent->GetSize().GetWidth()- BOARD_SIZE.x) / 2;
	const int gridStartY = (parent->GetSize().GetHeight()- BOARD_SIZE.y) / 2;
	ArmyColor tileColor = ArmyColor::Light;

	for (int r = 0; r < BOARD_DIMENSION; r++)
	{
		for (int c = 0; c < BOARD_DIMENSION; c++)
		{
			tileColor = IsDarkCell(Utils::Point2DInt(r, c)) ? ArmyColor::Dark : ArmyColor::Light;
			//bool isDarkCell = (r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1);
			/*auto cellColorDataIt = cellColorData.find(isDarkCell? ArmyColor::Dark : ArmyColor::Light);
			if (cellColorDataIt == cellColorData.end())
			{
				const std::string err = std::format("Tried to get color data for tile color but was not found");
				Utils::Log(Utils::LogType::Error, err);
				return;
			}*/

			currentPoint = wxPoint(gridStartX + c * CELL_SIZE.x, gridStartY + r * CELL_SIZE.y);
			
			auto emplaced = cells.emplace(Utils::Point2DInt(r, c), new Cell(parent, currentPoint, tileColor,
				GetCellColorsForTheme(*theme, tileColor),
				{{CellState::PossibleMoveHighlighted, &GetSpriteIcon(SpriteSymbolType::MoveSpot)},
				 {CellState::Disabled, &GetSpriteIcon(SpriteSymbolType::DisabledOverlay)}}));

			if (!emplaced.second)
			{
				const std::string error = std::format("Tried to place cell at row {} col {}"
					"but failed to emplace data into cell vector.emplace()", std::to_string(r), std::to_string(c));
				Utils::Log(Utils::LogType::Error, error);
				return;
			}
		}
	}

	//cellParent->SetPosition(wxPoint(parent->GetSize().GetWidth() / 2, parent->GetSize().GetHeight() / 2));
	
	//AddTurnChangeCallback(&UpdateInteractablePieces);

	//TODO: this should be done elsewhere not in the UI!
	//CreateDefaultBoard();
}

void BindCellEventsForGameState(Core::GameManager& manager, const std::string& gameStateId)
{
	for (const auto& cell : cells)
	{
		cell.second->AddOnClickCallback([&gameStateId, &cell, &manager](Cell* clickedCell) -> void
		{
			if (clickedCell == nullptr || clickedCell->IsDisabled()) return;

#pragma region Move Updating
			if (!currentCellMoves.empty())
			{
				for (const auto& cell : currentCellMoves)
				{
					/*Utils::Log(Utils::LogType::Error, std::format("SELECTED {} MOVES: {} CLICKED: {}",
						std::to_string(currentCellMoves.size()), Utils::ToStringIterable<std::vector<Cell*>, Cell*>(currentCellMoves),
						std::to_string(cell == clickedCell)));*/

					if (cell != clickedCell || lastSelected == nullptr) continue;

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

					PieceMoveResult result = manager.TryMoveForState(gameStateId, startPos.value(), endPos.value());
					if (!result.IsValidMove)
					{
						const std::string error = std::format("Tried update the the move of piece state from pos "
							"{} -> {} but failed! Info: {}", startPos.value().ToString(),
							endPos.value().ToString(), result.Info);
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					//TryGetSound(AudioClipType::Move)->PlaySoundWindows();
					TryGetSound(AudioClipType::Move)->TryPlaySound();
					const GameState* maybeGameState = manager.TryGetGameState(gameStateId);
					if (maybeGameState == nullptr)
					{
						const std::string error = std::format("Tried update the the move of piece state from pos "
							"{} -> {} but faile because game state for id: {} was not found", startPos.value().ToString(),
							endPos.value().ToString(), gameStateId);
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					if (!TryRenderUpdateCells(manager, *maybeGameState, std::vector<Utils::Point2DInt>{ startPos.value(), endPos.value() }))
					{
						const std::string error = std::format("Tried update the rendering for cells "
							"{} -> {} but failed!", startPos.value().ToString(), endPos.value().ToString());
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}

					if (!previousMoveCells.empty())
					{
						//for (const auto& cell : previousMoveCells) cell->Dehighlight();
						for (const auto& cell : previousMoveCells) cell->ResetStateToDefault();
						previousMoveCells.clear();
					}

					ResetCellVisuals();
					//lastSelected->Highlight(HighlightColorType::PreviousMove);
					//clickedCell->Highlight(HighlightColorType::PreviousMove);
					lastSelected->SetState(CellState::PreviousMoveHighlighted, true);
					clickedCell->SetState(CellState::PreviousMoveHighlighted, true);

					//We need to store the cell that was last selected (the start pos of move)
					//so we can disable it on next move since it is not stored definitively anywhere else
					previousMoveCells.push_back(lastSelected);
					previousMoveCells.push_back(clickedCell);
					lastSelected = clickedCell;

					std::optional<ArmyColor> newColor = manager.TryAdvanceTurn(gameStateId);
					Utils::Log(std::format("TURN: Trying to advance turn for state: {}, has new color: {}", gameStateId, std::to_string(newColor.has_value())));
					if (!newColor.has_value())
					{
						const std::string error = std::format("Tried advantce the turn in board UI when moving from"
							"{} -> {} but failed since GameManager call returned a null new team!",
							startPos.value().ToString(), endPos.value().ToString());
						Utils::Log(Utils::LogType::Error, error);
						continue;
					}
					UpdateInteractablePieces(newColor.value());

					PrintCells();
					return;
				}

				//If we got to this point it means player lickced on cell not from available moves
				if (lastSelected != nullptr) lastSelected->SetState(CellState::Default, true);
			}
			ResetCellVisuals();
			PrintCells();
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
			bool sameCellClickedAgain = lastSelected != nullptr && lastSelected == clickedCell;
			//We want to preserve previous old move highlighted status
			//so we must check for what type is highlighted and what to set as new
			if (sameCellClickedAgain)
			{
				if (IsStoredAsPreviousMove(lastSelected) && !lastSelected->IsDisabled())
				{
					if (lastSelected->m_CurrentState == CellState::PreviousMoveHighlighted)
						lastSelected->SetState(CellState::Selected, true);
					else lastSelected->SetState(CellState::PreviousMoveHighlighted, true);
				}
				else lastSelected->ToggleState(CellState::Selected);
			}
			else
			{
				if (lastSelected != nullptr && !lastSelected->IsDisabled())
				{
					if (IsStoredAsPreviousMove(lastSelected)) lastSelected->SetState(CellState::PreviousMoveHighlighted, true);
					else lastSelected->ResetStateToDefault();
				}
				clickedCell->SetState(CellState::Selected, true);
			}
#pragma endregion

#pragma region Possible Move Highlighting
			bool isClickedCellSelected = clickedCell->m_CurrentState == CellState::Selected;

			//Double clicking on a cell with possible moves will toggle it off
			if (sameCellClickedAgain && isClickedCellSelected && !currentCellMoves.empty()) ResetCellVisuals();
			else if (isClickedCellSelected)
			{
				TryGetSound(AudioClipType::PieceSelect)->TryPlaySound();
				std::vector<MoveInfo> possibleMoves = manager.TryGetPossibleMovesForPieceAt(gameStateId, cell.first);
				/*Utils::Log(Utils::LogType::Error, std::format("TURN POSSIBLE {} MOVES: {}",
					std::to_string(possibleMoves.size()), Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(possibleMoves)));*/

				Cell* cellAtPosition = nullptr;
				for (const auto& move : possibleMoves)
				{
					for (const auto& piecesMoved : move.PiecesMoved)
					{
						cellAtPosition = TryGetCellAtPosition(piecesMoved.NewPos);
						if (cellAtPosition == nullptr) continue;

						bool isCaptureCell = cellAtPosition->IsDisabled();
						cellAtPosition->SetState(CellState::PossibleMoveHighlighted, true);
						currentCellMoves.push_back(cellAtPosition);
					}
				}
				PrintCells();
			}

			lastSelected = clickedCell;
		});
	}
}

static void DisplayPieceMoves()
{
	//TODO: add display piece moves logic
}

bool TryRenderPieceAtPos(const Core::GameManager& manager, const Utils::Point2DInt& pos, const Piece* pieceInfo)
{
	//TryCacheAllSprites();
	if (!manager.IsPositionWithinBounds(pos))
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
	auto maybeSprite = TryGetImageFromPiece(PieceTypeInfo{ pieceInfo->m_Color, pieceInfo->m_PieceType });
	if (!maybeSprite.has_value())
	{
		std::string err = std::format("Tried to render piece {} is null: {} at pos {} piece color: {} but its sprite "
			"is not found", pieceInfo->ToString(), std::to_string(pieceInfo == nullptr), pos.ToString(),
			pieceInfo != nullptr ? ToString(pieceInfo->m_Color) : "NULL");
		Utils::Log(Utils::LogType::Error, err);
		return false;
	};

	//TODO: change from nullptr to actual piece
	Utils::Log(std::format("PIECE CHECK Need to update render sprite for piece {} at pos: {}", pieceInfo->ToString(), pos.ToString()));
	cellIt->second->SetPiece(pieceInfo, maybeSprite.value());
	return true;
}

bool TryRenderAllPieces(const Core::GameManager& manager, const GameState& state)
{
	const std::unordered_map<Utils::Point2DInt, Piece*>& pieces = state.InPlayPieces;
	const std::string message = std::format("A total of pieces: {}", std::to_string(state.InPlayPieces.size()));
	wxLogMessage(message.c_str());
	for (const auto& pieceData : pieces)
	{
		//PieceTypeInfo pieceInfo = {pieceData.second.color, pieceData.second.pieceType};
		Utils::Log(std::format("Try render all pieces piece: {} {}", pieceData.first.ToString(), pieceData.second->ToString()));
		if (!TryRenderPieceAtPos(manager, pieceData.first, pieceData.second))
		{
			const std::string err = std::format("Tried to render all pieces but failed to do it for piece {} at {}", 
				pieceData.second->ToString(), pieceData.first.ToString());
			Utils::Log(Utils::LogType::Error, err);
			return false;
		}
	}
	return true;
}

bool TryRenderUpdateCells(const Core::GameManager& manager, const GameState& gameState, std::vector<Utils::Point2DInt> positions)
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

		/*const GameState* gameState = manager.TryGetGameState(gameStateID);
		if (gameState == nullptr)
		{
			std::string err = std::format("Tried to update render for cell at pos {} "
				"but game state was not found for ID: {}", updatePos.ToString(), gameStateID);
			Utils::Log(Utils::LogType::Error, err);
			return false;
		}*/

		auto piecePairIt = gameState.InPlayPieces.find(updatePos);
		Utils::Log(std::format("Piece positions: {}", Utils::ToStringIterable(gameState.InPlayPieces)));
		
		const Piece* updatedPiecePtr = piecePairIt->second;
		if (piecePairIt != gameState.InPlayPieces.end())
			Utils::Log(std::format("Piece At new pos: {} is {}", updatePos.ToString(), updatedPiecePtr->ToString()));

		Utils::Log(std::format("Try render update cells for pos: {} current: {} old has: {} new has: {}", 
			Utils::ToStringIterable<std::vector<Utils::Point2DInt>, Utils::Point2DInt>(positions),
			updatePos.ToString(), std::to_string(cellPairIt->second->HasPiece(nullptr)),
			std::to_string(piecePairIt != gameState.InPlayPieces.end())));

		const Piece* cellOldDataPtr = nullptr;
		const Piece** cellOldDataPtrPtr = &cellOldDataPtr;
		bool cellAlreadyHasPiece = cellPairIt->second->HasPiece(cellOldDataPtrPtr);

		Utils::Log(std::format("PIECE CHECK update render cells for pos {}: {}",
			updatePos.ToString(), cellOldDataPtrPtr == nullptr ? "NULL PTR" : *cellOldDataPtrPtr == nullptr ? "NULL" : (*cellOldDataPtrPtr)->ToString()));

		//If the new data does not have piece here, we remove sprite
		if (piecePairIt == gameState.InPlayPieces.end() && cellAlreadyHasPiece)
		{
			Utils::Log(std::format("PIECE CHECK removing piece at pos {}", updatePos.ToString()));
			cellPairIt->second->TryRemovePiece();
		}

		//If new data has new piece here we update sprite
		else if ((piecePairIt != gameState.InPlayPieces.end() && !cellAlreadyHasPiece) ||
			(cellAlreadyHasPiece && cellOldDataPtrPtr != nullptr && *cellOldDataPtrPtr != updatedPiecePtr))
		{
			Utils::Log(std::format("PIECE CHECK udpate or add rendering piece at pos {}", updatePos.ToString()));
			TryRenderPieceAtPos(manager, updatePos, updatedPiecePtr);
		}

	}
	return true;
}


void EndCleanup()
{

}