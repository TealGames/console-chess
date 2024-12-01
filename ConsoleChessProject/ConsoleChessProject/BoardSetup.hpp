#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include "Color.hpp"
#include "JSONUtils.hpp"
#include "Piece.hpp"
#include "json.hpp"
#include "Point2Dint.hpp"

enum class BoardType
{
	Default,
};

const std::filesystem::path BOARDS_PATH = "StartBoard.json";
std::vector<std::string> GetBoardJSON(const BoardType& boardType);

struct InitPiecePosition
{
	const PieceType PieceType;
	const ArmyColor Color;
	const Utils::Point2DInt NewPos;
};
std::vector<InitPiecePosition> GetDefaultBoardPiecePositions();
