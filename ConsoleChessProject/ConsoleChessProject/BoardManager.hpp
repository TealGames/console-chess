#pragma once

#include <array>
#include <unordered_map>
#include "HelperFunctions.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "Tile.hpp"
#include "Piece.hpp"
#include "Color.hpp"

class MoveResult
{
public:
	const bool isValidMove;
	const Utils::Position2D attemptedPos;
	const std::string info;

	MoveResult(const Utils::Position2D&, const bool, const std::string& info = "");
};

constexpr int BOARD_DIMENSION = 8;
constexpr int COLOR_PIECES_COUNT = 16;

void CreatePieces(const ColorTheme, bool);
void ResetBoard();
MoveResult TryMove(const Piece&, const Utils::Position2D&);

