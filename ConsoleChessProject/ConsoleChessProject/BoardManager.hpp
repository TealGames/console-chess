#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include "HelperFunctions.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "Tile.hpp"
#include "Piece.hpp"
#include "Color.hpp"

class MoveResult
{
public:
	const bool IsValidMove;
	const std::vector<Utils::Position2D> AttemptedPositions;
	const std::string Info;

	MoveResult(const Utils::Position2D&, const bool, const std::string& info = "");
	MoveResult(const std::vector<Utils::Position2D>&, const bool, const std::string& info = "");
};

enum class SpecialMove : unsigned int
{
	None				= 0,
	KingSideCastle		= 1 << 0,
	QueenSideCastle		= 1 << 1,
	Capture				= 1 << 2,
	Promotion			= 1 << 3,
};

inline SpecialMove operator|(SpecialMove lhs, SpecialMove rhs) {
	return static_cast<SpecialMove>(
		static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

SpecialMove& operator|=(SpecialMove lhs, SpecialMove rhs) {
	lhs = lhs | rhs;
	return lhs;
}

inline SpecialMove operator&(SpecialMove lhs, SpecialMove rhs) {
	return static_cast<SpecialMove>(
		static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

SpecialMove& operator&=(SpecialMove lhs, SpecialMove rhs) {
	lhs = lhs & rhs;
	return lhs;
}

struct MoveInfo
{
	const std::vector<PieceType> PiecesMoved = {};
	const std::optional<Utils::Position2D> PiecePositionInfo;
	const Utils::Position2D MoveToPosition;
	const std::string& BoardNotation = "";
	const SpecialMove SpecialMoveFlags = SpecialMove::None;
	//Piece to promote to
	const std::optional<PieceType> PiecePromotion; 
	const bool IsCheck = false;
	const bool IsCheckmate = false;
};

constexpr int TEAMS_COUNT = 2;
constexpr int COLOR_PIECES_COUNT = 16;
constexpr int NULL_POS = -1;
const Utils::Position2D INVALID_MOVE = { NULL_POS, NULL_POS };

constexpr char NOTATION_CAPTURE_CHAR = 'x';
constexpr char NOTATION_PROMOTION_CHAR = '=';
constexpr char NOTATION_CHECK_CHAR = '+';
constexpr char NOTATION_CHECKMATE_CHAR = '#';
const std::string NOTATION_KINGSIDE_CASTLE = "O-O";
const std::string NOTATION_QUEENSIDE_CASTLE = "O-O-O";

void ResetBoard();
void CreateDefaultBoard();
Piece* GetPieceAtPosition(const Utils::Position2D& pos);
inline bool HasPieceAtPosition(const Utils::Position2D& pos, const Piece* outPiece);

MoveResult TryMove(const Utils::Position2D& currentPos, const Utils::Position2D& moveToPos);

std::string CleanInput(const std::string& input);
std::optional<MoveInfo> TryParseMoveInfoFromMove(const std::string& input);
//TODO: add parse/serialization method to convert moveinfo to chess notation input

const std::vector<MoveInfo>& GetPreviousMoves(const ColorTheme& color);
bool HasMovedPiece(const ColorTheme& color, const PieceType& type, const MoveInfo* outFirstMove=nullptr);