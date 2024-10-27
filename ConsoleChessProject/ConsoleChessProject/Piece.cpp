#include <format>
#include "Piece.hpp"
#include "Color.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "HelperFunctions.hpp"

struct PieceMoveInfo
{
	double value;
	const std::vector<Utils::Vector2D>& moveDirs;
	const std::vector<Utils::Vector2D>& captureDirs;
};

static const std::unordered_map<const PieceType, const PieceMoveInfo> PIECE_MOVES =
{
	{PieceType::Pawn, {1, {{1,0}, {1, 1}, {-1,-1}}, {}}},
	{PieceType::Knight, {3, {{2, 1}, {1, 2}, {2, -1}, {-2, 1}, {-2, 1}, {-1, -2}, {1, -2}, {2, -1}}, {} }},
	{PieceType::Bishop, {3, {{std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()},
									{std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()},
									{-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()},
									{-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()}},
									{}}},
	{PieceType::Rook, {5,   {{0, std::numeric_limits<double>::infinity()},
									{std::numeric_limits<double>::infinity(), 0},
									{0, -std::numeric_limits<double>::infinity()},
									{-std::numeric_limits<double>::infinity(), 0}},
									{}}},
	{PieceType::Queen, {9,  {{std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()},
									{std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()},
									{-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()},
									{-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()},
									{0, std::numeric_limits<double>::infinity()},
									{std::numeric_limits<double>::infinity(), 0},
									{0, -std::numeric_limits<double>::infinity()},
									{-std::numeric_limits<double>::infinity(), 0}},
									{}}},
	{PieceType::King, {std::numeric_limits<double>::infinity(), {{1,0}, {0, 1}, {-1, 0}, {0, -1}}, {}}}
};

Piece::Piece(const ColorTheme color, const PieceType piece, const Utils::Position2D pos)
	: color(color), pieceType(piece), _moveDirs(GetMoveDirsForPiece(piece)),
	_captureDirs(GetCaptureMovesForPiece(piece)), _pos(pos), pos(_pos), _isCaptured(false) {}

bool Piece::HasDifferentCaptureMove()
{
	return _captureDirs.size() > 0;
}

bool HasPieceTypeDefined(const PieceType type)
{
	return Utils::IterableHas(PIECE_MOVES, type);
}

void Piece::SetCaptured(bool isCaptured)
{
	_isCaptured = isCaptured;
}

void Piece::SetPos(Utils::Position2D newPos)
{
	_pos = newPos;
}

double GetValueForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get value for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return 0;
	}
	return PIECE_MOVES.at(type).value;
}

std::vector<Utils::Vector2D> GetMoveDirsForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return PIECE_MOVES.at(type).moveDirs;
}

std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get capture moves for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return PIECE_MOVES.at(type).captureDirs;
}

bool DoesMoveDeltaMatchPieceMoves(const PieceType type,
	const Utils::Position2D& startPos, const Utils::Position2D& endPos)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	const Utils::Vector2D delta = GetVector(startPos, endPos);
	if (delta == Utils::Vector2D::ZERO) return false;

	for (const auto& moveDir : PIECE_MOVES.at(type).moveDirs)
	{
		if (delta == moveDir) return true;

		bool xPosInifinity = Utils::IsInifinity(moveDir.x);
		bool xNegInifinity = Utils::IsNegInifinity(moveDir.x);
		bool yPosInfinity = Utils::IsInifinity(moveDir.y);
		bool yNegInfinity = Utils::IsNegInifinity(moveDir.y);

		//Note: since indices ascend downward for row, the delta's will be flipped from what expected
		bool hasCorrectX = (xPosInifinity && delta.x < 0) || (xNegInifinity && delta.x > 0) || (delta.x == moveDir.x);
		bool hasCorrectY = (yPosInfinity && delta.y > 0) || (yNegInfinity && delta.y < 0) || (delta.y == moveDir.y);
		if (hasCorrectX && hasCorrectY) return true;
	}
}

bool Piece::DoesMoveDeltaMatchPieceMoves(const Utils::Position2D& newPos) const
{
	return ::DoesMoveDeltaMatchPieceMoves(pieceType, pos, newPos);
}

std::string Piece::ToString() const
{
	std::string str = std::format("[{} {}]", ::ToString(color), ::ToString(pieceType));
	return str;
}

std::string ToString(const PieceType& piece)
{
	switch (piece)
	{
	case PieceType::Pawn:
		return "Pawn";
	case PieceType::Knight:
		return "Knight";
	case PieceType::Bishop:
		return "Bishop";
	case PieceType::Rook:
		return "Rook";
	case PieceType::Queen:
		return "Queen";
	case PieceType::King:
		return "King";
	default:
		return "NULL";
	}
	return "";
}