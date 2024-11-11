#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include "Color.hpp"
#include "JSONUtils.hpp"
#include "Piece.hpp"
#include "json.hpp"

const std::filesystem::path BOARDS_PATH = "StartBoard.json";

std::string GetDefaultBoardJSON(const ColorTheme& color);

//Pieces has to be not const and not reference since we need to have a copy to remove values
std::vector<Utils::Position2D> GetPositionsForPieces(const ColorTheme& color, const std::vector<const Piece*>& pieces);
