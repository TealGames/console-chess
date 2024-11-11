#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <string>
#include <limits>
#include "Vector2D.hpp"
#include "Color.hpp"
#include "Position2D.hpp"

enum class PieceType
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

struct PieceInfo
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
	const std::string& displayString;

	Piece(const ColorTheme, const PieceType, const std::string& displayString);
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
	const Utils::Position2D& startPos, const Utils::Position2D& endPos);
bool DoesMoveDeltaMatchCaptureMoves(const PieceType type,
	const Utils::Position2D& startPos, const Utils::Position2D& endPos);

std::string ToString(const PieceType&);