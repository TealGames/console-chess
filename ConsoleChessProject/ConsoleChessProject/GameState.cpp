#include <optional>
#include <unordered_map>
#include <format>
#include <string>
#include <vector>
#include "GameState.hpp"
#include "Point2DInt.hpp"
#include "Color.hpp"
#include "HelperFunctions.hpp"

inline SpecialMove operator|(SpecialMove lhs, SpecialMove rhs)
{
	return static_cast<SpecialMove>(
		static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

SpecialMove& operator|=(SpecialMove lhs, SpecialMove rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

inline SpecialMove operator&(SpecialMove lhs, SpecialMove rhs)
{
	return static_cast<SpecialMove>(
		static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

SpecialMove& operator&=(SpecialMove lhs, SpecialMove rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

PiecePositionData::PiecePositionData(const Piece& piece, const Utils::Point2DInt& pos)
	: PieceRef(piece), Pos(pos) {}

MovePiecePositionData::MovePiecePositionData(const Piece& piece, const Utils::Point2DInt& oldPos, const Utils::Point2DInt& newPos)
	: _piece(&piece), PieceRef(*_piece), _oldPos(oldPos), OldPos(_oldPos), _newPos(newPos), NewPos(_newPos) {}

MovePiecePositionData::MovePiecePositionData(const MovePiecePositionData& other)
	: _piece(&other.PieceRef), PieceRef(*_piece), _oldPos(other.OldPos), OldPos(_oldPos), _newPos(other.NewPos), NewPos(_newPos) {}

MovePiecePositionData& MovePiecePositionData::operator=(const MovePiecePositionData& other)
{
	_piece = other._piece;
	_oldPos = other._oldPos;
	_newPos = other._newPos;
	return *this;
}

bool MovePiecePositionData::operator==(const MovePiecePositionData& other) const
{
	return PieceRef == other.PieceRef && OldPos == other.OldPos && NewPos == other.NewPos;
}

MoveInfo::MoveInfo(const std::vector<MovePiecePositionData>& piecesMoved, const std::string& boardNotation, const SpecialMove& moveFlags,
	const std::optional<Piece*>& promotion, const bool& check, const bool& checkmate) :
	_piecesMoved(piecesMoved), PiecesMoved(_piecesMoved), _boardNotation(boardNotation), BoardNotation(_boardNotation),
	_specialMoveFlags(moveFlags), SpecialMoveFlags(_specialMoveFlags), _piecePromotion(promotion), PiecePromotion(_piecePromotion),
	_isCheck(check), IsCheck(_isCheck), _isCheckmate(checkmate), IsCheckmate(_isCheckmate)
{
}

bool MoveInfo::operator==(const MoveInfo& otherInfo) const
{
	return PiecesMoved == otherInfo.PiecesMoved && BoardNotation == otherInfo.BoardNotation &&
		SpecialMoveFlags == otherInfo.SpecialMoveFlags && PiecePromotion == otherInfo.PiecePromotion &&
		IsCheck == otherInfo.IsCheck && IsCheckmate == otherInfo.IsCheckmate;
}

MoveInfo& MoveInfo::operator=(const MoveInfo& otherInfo)
{
	if (otherInfo == *this) return *this;

	_piecesMoved = otherInfo._piecesMoved;
	_boardNotation = otherInfo._boardNotation;
	_specialMoveFlags = otherInfo._specialMoveFlags;
	_piecePromotion = otherInfo._piecePromotion;
	_isCheck = otherInfo._isCheck;
	_isCheckmate = otherInfo._isCheckmate;
	return *this;
}

std::string GameState::ToString() const
{
	const std::string piecesStr = Utils::ToStringIterable(PiecePositions);
	const std::string str = std::format("[GAME STATE-> player:{}, positions:{}, inCheck:{}, inCheckmate:{}]", 
		::ToString(CurrentPlayer), piecesStr, std::to_string(InCheck), std::to_string(InCheckmate));
	return str;
}
