#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include "JSONUtils.hpp"
#include "Piece.hpp"

const std::string ROW_IDENTIFER;
const std::string EMPTY_IDENTIFIER;
const std::unordered_map<Piece::PieceType, std::string> PIECE_IDENTIFIERS;