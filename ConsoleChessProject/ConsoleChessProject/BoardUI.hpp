#pragma once
#include <wx/wx.h>
#include <unordered_map>
#include <vector>
#include <optional>
#include "Point2DInt.hpp"
#include "Cell.hpp"
#include "Piece.hpp"
#include "GameState.hpp"

void CreateBoardCells(wxWindow* parent);

/// <summary>
/// Note: this needs to have non-const state so that lamdas when calling click events
/// and update the game state
/// </summary>
/// <param name="state"></param>
void BindCellEventsForGameState(GameState& state);
Cell* TryGetCellAtPosition(const Utils::Point2DInt point);
std::optional<Utils::Point2DInt> TryGetPositionOfCell(const Cell& cell);

bool TryRenderPieceAtPos(const Utils::Point2DInt& pos, const Piece* pieceInfo);
/// <summary>
/// Will go through all pieces in current state and render them in their respective cells
/// </summary>
/// <param name="state"></param>
/// <returns></returns>
bool TryRenderAllPieces(const GameState& state);

/// <summary>
/// Will update the cells positions based on the new piece data
/// </summary>
/// <param name="state"></param>
/// <param name="positions"></param>
/// <returns></returns>
bool TryRenderUpdateCells(const GameState& state, std::vector<Utils::Point2DInt> positions);
void UpdateInteractablePieces(const ColorTheme& interactableColor);
void EndCleanup();
