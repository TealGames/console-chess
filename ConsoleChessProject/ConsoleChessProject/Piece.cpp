#include <format>
#include "Piece.hpp"
#include "Color.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "HelperFunctions.hpp"

const std::unordered_map<Piece::PieceType, Piece::PieceMoveInfo> Piece::pieceMoves =
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

bool Piece::HasPieceTypeDefined(const PieceType type)
{
	return Utils::IterableHas(pieceMoves, type);
}

void Piece::SetCaptured(bool isCaptured)
{
	_isCaptured = isCaptured;
}

void Piece::SetPos(Utils::Position2D newPos)
{
	_pos = newPos;
}

double Piece::GetValueForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get value for a piece of type {} "
			"but type has no defined info", type);
		Utils::Log(Utils::LogType::Error, err);
		return 0;
	}
	return pieceMoves.at(type).value;
}

std::vector<Utils::Vector2D> Piece::GetMoveDirsForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", type);
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return pieceMoves.at(type).moveDirs;
}

std::vector<Utils::Vector2D> Piece::GetCaptureMovesForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get capture moves for a piece of type {} "
			"but type has no defined info", type);
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return pieceMoves.at(type).captureDirs;
}

bool Piece::DoesMoveDeltaMatchPieceMoves(const PieceType type,
	const Utils::Position2D& startPos, const Utils::Position2D& endPos)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", type);
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	const Utils::Vector2D delta = Utils::Vector2D::GetVector(startPos, endPos);
	if (delta == Utils::Vector2D::ZERO) return false;

	for (const auto& moveDir : pieceMoves.at(type).moveDirs)
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
	return Piece::DoesMoveDeltaMatchPieceMoves(pieceType, pos, newPos);
}

std::string Piece::ToString() const
{
	std::string str = std::format("[{} {}]", color, pieceType);
	return str;
}