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

namespace std
{
	template<> struct hash<MoveInfo>
	{
		//TODO: improve hasing algorithm to include all members
		std::size_t operator()(const MoveInfo& moveInfo) const noexcept
		{
			std::size_t hashValue = 0;

			/*for (const auto& pieceData : moveInfo.PiecesMoved) {
				hashValue ^= std::hash<MovePiecePositionData>()(pieceData) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
			}*/

			//Note: 0x9e3779b9 is golden ratio as hex and can add some irregularity to the hashing algorithm to avoid collisions
			hashValue ^= std::hash<std::string>()(moveInfo.BoardNotation) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
			hashValue ^= std::hash<int>()(static_cast<int>(moveInfo.SpecialMoveFlags)) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);

			/*if (moveInfo.PiecePromotion.has_value()) {
				hashValue ^= std::hash<Piece*>()(moveInfo.PiecePromotion.value()) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
			}*/

			hashValue ^= std::hash<bool>()(moveInfo.IsCheck) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
			hashValue ^= std::hash<bool>()(moveInfo.IsCheckmate) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);

			return hashValue;
		}
	};
}

using PiecePositionMapType = std::unordered_map<Utils::Point2DInt, Piece>;
struct GameState
{
	ColorTheme CurrentPlayer;

	PiecePositionMapType PiecePositions;
	std::unordered_map<ColorTheme, std::vector<MoveInfo>> PreviousMoves;

	bool InCheckmate;
	bool InCheck;

	std::string ToString() const;
};

