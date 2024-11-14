#pragma once
#include <vector>
#include <array>
#include <functional>
#include <optional>
#include <unordered_map>
#include <string>
#include <limits>
#include "Vector2D.hpp"
#include "Color.hpp"
#include "Point2DInt.hpp"

enum class PieceType
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

constexpr std::array<PieceType, 6> ALL_PIECE_TYPES = {
	PieceType::Pawn, PieceType::Knight, PieceType::Bishop,
	PieceType::Rook, PieceType::Queen, PieceType::King };

struct PieceTypeInfo
{
	const ColorTheme Color;
	const PieceType PieceType;

	std::string ToString() const;
	bool operator==(const PieceTypeInfo& other) const;
};

namespace std
{
	template<> struct hash<PieceTypeInfo>
	{
		std::size_t operator()(const PieceTypeInfo& info) const noexcept
		{
			std::size_t colorHash = std::hash<int>()(static_cast<int>(info.Color));
			std::size_t typeHash = std::hash<int>()(static_cast<int>(info.PieceType));
			return colorHash ^ (typeHash << 1);
		}
	};
}

struct PieceStaticInfo
{
	const double ScoreValue;
	const char MoveNotationCharacter;
	const std::vector<Utils::Vector2D> MoveDirs;
	const std::vector<Utils::Vector2D> CaptureDirs;
};

class Piece
{
public:
	enum State
	{
		Undefined,
		InPlay,
		Captured,
	};
private:
	const std::vector<Utils::Vector2D>& _moveDirs;
	const std::vector<Utils::Vector2D>& _captureDirs;

	const std::string _displayString;
	State _state;

	bool HasDifferentCaptureMove();

public:
	const ColorTheme color;
	const PieceType pieceType;
	const State& state;

	Piece(const ColorTheme, const PieceType);
	Piece(const Piece& copy);

	void UpdateState(const State& state);
	std::string ToString() const;

	bool operator==(const Piece& piece) const;
};

double GetValueForPiece(const PieceType piece);
const std::vector<Utils::Vector2D> GetMoveDirsForPiece(const PieceType piece);
const std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const PieceType piece);
char GetNotationSymbolForPiece(const PieceType piece);
const std::optional<PieceType> TryGetPieceFromNotationSymbol(const char& notation);

bool DoesMoveDeltaMatchPieceMoves(const PieceType type,
	const Utils::Point2DInt& startPos, const Utils::Point2DInt& endPos);
bool DoesMoveDeltaMatchCaptureMoves(const PieceType type,
	const Utils::Point2DInt& startPos, const Utils::Point2DInt& endPos);

std::string ToString(const PieceType&);