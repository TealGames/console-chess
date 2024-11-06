#include <format>
#include <unordered_map>
#include "Piece.hpp"
#include "Color.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "HelperFunctions.hpp"
#include "FrozenMap.hpp"

static constexpr double INF = std::numeric_limits<double>::infinity();
static const std::unordered_map<PieceType, PieceInfo> PIECE_INFO(
	std::unordered_map<PieceType, PieceInfo> {
		{PieceType::Pawn, {1, 'P', {{1,0}, {1, 1}, {-1,-1}}, {}}},
		{PieceType::Knight, {3, 'N', {{2, 1}, {1, 2}, {2, -1}, {-2, 1}, {-2, 1}, {-1, -2}, {1, -2}, {2, -1}}, {}}},
		{PieceType::Bishop, {3, 'B', {{INF, INF}, {INF, -INF}, {-INF, INF}, {-INF, -INF}}, {}}},
		{PieceType::Rook, {5, 'R', {{0, INF}, {INF, 0}, {0, -INF}, {-INF, 0}}, {}}},

		{PieceType::Queen, {9, 'Q', {{INF, INF}, {INF, -INF}, {-INF, INF}, {-INF, -INF},
								 {0, INF}, {INF, 0}, {0, -INF}, {-INF, 0}}, {}}},

		{PieceType::King, {INF, 'K', {{1,0}, {0, 1}, {-1, 0}, {0, -1}}, {}}}
	});

Piece::Piece(const ColorTheme color, const PieceType piece, const std::string& displayString)
	: color(color), pieceType(piece), _moveDirs(GetMoveDirsForPiece(piece)),
	_captureDirs(GetCaptureMovesForPiece(piece)), _state(Piece::State::Undefined), state(_state), 
	_displayString(displayString), displayString(_displayString){}

bool Piece::HasDifferentCaptureMove()
{
	return _captureDirs.size() > 0;
}

bool HasPieceTypeDefined(const PieceType type)
{
	return PIECE_INFO.find(type) != PIECE_INFO.end();
}

void Piece::UpdateState(const State& state)
{
	_state = state;
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
	return PIECE_INFO.at(type).ScoreValue;
}

const std::vector<Utils::Vector2D> GetMoveDirsForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return PIECE_INFO.at(type).MoveDirs;
}

const std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get capture moves for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return PIECE_INFO.at(type).MoveDirs;
}

char GetNotationSymbolForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get piece moves for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return PIECE_INFO.at(type).MoveNotationCharacter;
}

const std::optional<PieceType> TryGetPieceFromNotationSymbol(const char& notation)
{
	for (const auto& pieceInfo : PIECE_INFO)
	{
		if (pieceInfo.second.MoveNotationCharacter == notation) 
			return pieceInfo.first;
	}
	return std::nullopt;
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

	for (const auto& moveDir : PIECE_INFO.at(type).MoveDirs)
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

bool DoesMoveDeltaMatchCaptureMoves(const PieceType type,
	const Utils::Position2D& startPos, const Utils::Position2D& endPos)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get capture dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	const Utils::Vector2D delta = GetVector(startPos, endPos);
	if (delta == Utils::Vector2D::ZERO) return false;


	for (const auto& moveDir : PIECE_INFO.at(type).MoveDirs)
	{
		if (delta == moveDir) return true;
	}
	return false;
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