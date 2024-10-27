#include <format>
#include <string>
#include <unordered_map>
#include "HelperFunctions.hpp"
#include "Piece.hpp"

const std::string ROW_IDENTIFER = "Row";
const std::string EMPTY_IDENTIFIER = "_";
const std::unordered_map<PieceType, std::string> PIECE_IDENTIFIERS =
{
	{PieceType::Pawn, "PW"},
	{PieceType::Knight, "KN"},
	{PieceType::Bishop, "BS"},
	{PieceType::Rook, "RK"},
	{PieceType::Queen, "QU"},
	{PieceType::King, "KG"},
};

std::string GetIdentifierFromPieceType(const PieceType piece)
{
	if (!Utils::IterableHas(PIECE_IDENTIFIERS, piece))
	{
		std::string err = std::format("Tried to get JSON identifer from "
			"piece {} but it is not defined", ToString(piece));
		Utils::Log(Utils::LogType::Error, err);
		return "";
	}

	return PIECE_IDENTIFIERS.at(piece);
}