#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include "Color.hpp"
#include "JSONUtils.hpp"
#include "Piece.hpp"
#include "json.hpp"
#include "Point2Dint.hpp"

const std::filesystem::path BOARDS_PATH = "StartBoard.json";

std::string GetDefaultBoardJSON(const ColorTheme& color);

struct InitPiecePosition
{
	const PieceType PieceType;
	const Utils::Point2DInt NewPos;
};
std::vector<InitPiecePosition> GetDefaultBoardPiecePositions(const ColorTheme& color);
