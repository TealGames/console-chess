#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include "Point2DInt.hpp"
#include "Piece.hpp"
#include "Color.hpp"

enum class SpecialMove : unsigned int
{
	None = 0,
	KingSideCastle = 1 << 0,
	QueenSideCastle = 1 << 1,
	Capture = 1 << 2,
	Promotion = 1 << 3,
};

inline SpecialMove operator|(SpecialMove lhs, SpecialMove rhs);
SpecialMove& operator|=(SpecialMove lhs, SpecialMove rhs);
inline SpecialMove operator&(SpecialMove lhs, SpecialMove rhs);
SpecialMove& operator&=(SpecialMove lhs, SpecialMove rhs);

struct PiecePositionData
{
	const Piece& PieceRef;
	const Utils::Point2DInt& Pos;

	PiecePositionData(const Piece& piece, const Utils::Point2DInt& pos);
};

class MovePiecePositionData
{
private:
	//This needs to be a pointer since constructor
	//cannot initialize non const refs to const refs
	const Piece* _piece;
	Utils::Point2DInt _oldPos;
	Utils::Point2DInt _newPos;

public:
	const Piece& PieceRef;
	const Utils::Point2DInt& OldPos;
	const Utils::Point2DInt& NewPos;

	MovePiecePositionData(const Piece& piece, const Utils::Point2DInt& oldPos, const Utils::Point2DInt& newPos);
	MovePiecePositionData(const MovePiecePositionData& other);

	std::string ToString() const;

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
	std::optional<PieceTypeInfo> _capturedPiece;
	bool _isCheck;
	bool _isCheckmate;

public:
	//all the pieces moved and their NEW position
	const std::vector<MovePiecePositionData>& PiecesMoved;
	const std::string& BoardNotation = "";
	const SpecialMove& SpecialMoveFlags = SpecialMove::None;
	//Piece to promote to
	const std::optional<Piece*>& PiecePromotion;
	const std::optional<PieceTypeInfo>& PieceCaptured;
	const bool& IsCheck = false;
	const bool& IsCheckmate = false;

	MoveInfo(const std::vector<MovePiecePositionData>& piecesMoved, const std::string& boardNotation, const SpecialMove& moveFlags,
		const std::optional<Piece*>& promotion, const std::optional<PieceTypeInfo>& pieceCaptured, const bool& check, const bool& checkmate);
	MoveInfo(const MoveInfo& other) noexcept;
	MoveInfo(MoveInfo&& other) noexcept;

	std::string ToString() const;

	bool operator==(const MoveInfo& otherInfo) const;
	MoveInfo& operator=(const MoveInfo& otherInfo) noexcept;
};

using PiecePositionMapType = std::unordered_map<Utils::Point2DInt, Piece*>;
struct GameState
{
	ColorTheme CurrentPlayer = ColorTheme::Light;
	std::unordered_map<ColorTheme, int> TeamValue = {};

	//TODO: maybe create general all peices list and then have separate for in play and captured
	std::vector<Piece> AllPieces = {};
	PiecePositionMapType InPlayPieces = {};
	std::vector<Piece*> CapturedPieces = {};

	std::unordered_map<ColorTheme, std::vector<MoveInfo>> PreviousMoves = {};

	bool InCheckmate = false;
	bool InCheck = false;

	std::string ToString() const;
};

