#pragma once
#include <wx/wx.h>
#include <unordered_map>
#include <vector>
#include <optional>
#include "Point2DInt.hpp"
#include "Cell.hpp"
#include "Piece.hpp"
#include "GameState.hpp"
#include "GameManager.hpp"
#include "Globals.hpp"

const wxSize BOARD_SIZE = wxSize(CELL_SIZE.x * BOARD_DIMENSION, CELL_SIZE.y * BOARD_DIMENSION);

void CreateBoardCells(wxWindow* parent);

/// <summary>
/// Note: this needs to have non-const state so that lamdas when calling click events
/// and update the game state
/// </summary>
/// <param name="state"></param>
void BindCellEventsForGameState(Core::GameManager& manager, const std::string& gameStateId);
Cell* TryGetCellAtPosition(const Utils::Point2DInt point);
std::optional<Utils::Point2DInt> TryGetPositionOfCell(const Cell& cell);

bool TryRenderPieceAtPos(const Core::GameManager& manager, 
	const Utils::Point2DInt& pos, const Piece* pieceInfo);
/// <summary>
/// Will go through all pieces in current state and render them in their respective cells
/// </summary>
/// <param name="state"></param>
/// <returns></returns>
bool TryRenderAllPieces(const Core::GameManager& manager, const GameState& state);

/// <summary>
/// Will update the cells positions based on the new piece data
/// </summary>
/// <param name="state"></param>
/// <param name="positions"></param>
/// <returns></returns>
bool TryRenderUpdateCells(const Core::GameManager& manager, 
	const GameState& gameState, std::vector<Utils::Point2DInt> positions);

void UpdateInteractablePieces(const ColorTheme& interactableColor);
void EndCleanup();
