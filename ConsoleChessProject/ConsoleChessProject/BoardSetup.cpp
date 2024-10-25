#include <format>
#include <string>
#include <unordered_map>
#include "HelperFunctions.hpp"
#include "Piece.hpp"

const std::string ROW_IDENTIFER = "Row";
const std::string EMPTY_IDENTIFIER = "_";
const std::unordered_map<Piece::PieceType, std::string> PIECE_IDENTIFIERS =
{
	{Piece::PieceType::Pawn, "PW"},
	{Piece::PieceType::Knight, "KN"},
	{Piece::PieceType::Bishop, "BS"},
	{Piece::PieceType::Rook, "RK"},
	{Piece::PieceType::Queen, "QU"},
	{Piece::PieceType::King, "KG"},
};

std::string GetIdentifierFromPieceType(const Piece::PieceType piece)
{
	if (!Utils::IterableHas(PIECE_IDENTIFIERS, piece))
	{
		std::string err = std::format("Tried to get JSON identifer from "
			"piece {} but it is not defined", piece);
		Utils::Log(Utils::LogType::Error, err);
		return "";
	}

	return PIECE_IDENTIFIERS.at(piece);
}