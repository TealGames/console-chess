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

struct PiecePositionData
{
	const Piece& PieceRef;
	const Utils::Position2D& Pos;

	PiecePositionData(const Piece& piece, const Utils::Position2D& pos);
};

class MovePiecePositionData
{
private:
	//This needs to be a pointer since constructor
	//cannot initialize non const refs to const refs
	const Piece* _piece;
	Utils::Position2D _oldPos;
	Utils::Position2D _newPos;

public:
	const Piece& PieceRef;
	const Utils::Position2D& OldPos;
	const Utils::Position2D& NewPos;

	MovePiecePositionData(const Piece& piece, const Utils::Position2D& oldPos, const Utils::Position2D& newPos);
	MovePiecePositionData(const MovePiecePositionData& other);

	MovePiecePositionData& operator=(const MovePiecePositionData& other);
	bool operator==(const MovePiecePositionData& other) const;
};

class MoveInfo
{
private:
	//all the pieces moved and their NEW position
	std::vector<MovePiecePositionData> _piecesMoved;
	std::string _boardNotation;
	SpecialMove _specialMoveFlags;
	//Piece to promote to
	std::optional<Piece*> _piecePromotion; 
	bool _isCheck;
	bool _isCheckmate;

public:
	//all the pieces moved and their NEW position
	const std::vector<MovePiecePositionData>& PiecesMoved;
	const std::string& BoardNotation = "";
	const SpecialMove& SpecialMoveFlags = SpecialMove::None;
	//Piece to promote to
	const std::optional<Piece*>& PiecePromotion;
	const bool& IsCheck = false;
	const bool& IsCheckmate = false;

	MoveInfo(const std::vector<MovePiecePositionData>& piecesMoved, const std::string& boardNotation, const SpecialMove& moveFlags,
		const std::optional<Piece*>& promotion, const bool& check, const bool& checkmate);

	bool operator==(const MoveInfo& otherInfo) const;
	MoveInfo& operator=(const MoveInfo& otherInfo);
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
inline bool TryGetPieceAtPosition(const Utils::Position2D& pos, const Piece* outPiece);
std::optional<Utils::Position2D> TryGetPositionOfPiece(const Piece& piece);

bool HasPieceWithinPositionRange(const Utils::Position2D& startPos, const Utils::Position2D& endPos, bool inclusive=true);
std::vector<PiecePositionData> TryGetAvailablePiecesPosition(const ColorTheme& color, const PieceType& type);

const std::vector<MoveInfo>& GetPreviousMoves(const ColorTheme& color);
bool HasMovedPiece(const ColorTheme& color, const PieceType& type, const MoveInfo* outFirstMove = nullptr);

std::vector<MoveInfo> GetPossibleMovesForPieceAt(const Utils::Position2D& pos);
MoveResult TryMove(const Utils::Position2D& currentPos, const Utils::Position2D& moveToPos);

std::string CleanInput(const std::string& input);
//std::string GetNotationFromMoveInfo(const MoveInfo& info);
//std::optional<MoveInfo> TryParseMoveInfoFromMove(const ColorTheme& color, const std::string& input);

//TODO: add parse/serialization method to convert moveinfo to chess notation input

