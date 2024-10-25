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

	MoveResult(Utils::Position2D, bool, std::string info = "");
};

static constexpr int boardDimension = 8;
static std::array<std::array<Tile, boardDimension>, boardDimension> tiles;

static constexpr int colorPieces = 16;
static std::unordered_map<ColorTheme, std::array<Piece&, colorPieces>> allPieces;

void CreatePieces(const ColorTheme, bool);
void ResetBoard();
MoveResult TryMove(const Piece&, const Utils::Position2D&);
