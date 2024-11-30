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

std::string MovePiecePositionData::ToString() const
{
	return std::format("[Piece: {}, old: {} -> new: {}]", PieceRef.ToString(), OldPos.ToString(), NewPos.ToString());
}

bool MovePiecePositionData::operator==(const MovePiecePositionData& other) const
{
	return PieceRef == other.PieceRef && OldPos == other.OldPos && NewPos == other.NewPos;
}

MoveInfo::MoveInfo(const std::vector<MovePiecePositionData>& piecesMoved, const std::string& boardNotation, const SpecialMove& moveFlags,
	const Piece* const promotion, const Piece* const capturedPiece, const bool& check, const bool& checkmate) :
	_piecesMoved(piecesMoved), PiecesMoved(_piecesMoved), _boardNotation(boardNotation), BoardNotation(_boardNotation),
	_specialMoveFlags(moveFlags), SpecialMoveFlags(_specialMoveFlags), _piecePromotion(promotion), PiecePromotion(_piecePromotion),
	_capturedPiece(capturedPiece), PieceCaptured(_capturedPiece),
	_isCheck(check), IsCheck(_isCheck), _isCheckmate(checkmate), IsCheckmate(_isCheckmate)
{
}

MoveInfo::MoveInfo(const MoveInfo& other) noexcept:
_piecesMoved(other.PiecesMoved), PiecesMoved(_piecesMoved), _boardNotation(other.BoardNotation), BoardNotation(_boardNotation),
_specialMoveFlags(other.SpecialMoveFlags), SpecialMoveFlags(_specialMoveFlags), _piecePromotion(other.PiecePromotion), PiecePromotion(_piecePromotion),
_capturedPiece(other.PieceCaptured), PieceCaptured(_capturedPiece), _isCheck(other.IsCheck), IsCheck(_isCheck), 
_isCheckmate(other.IsCheckmate), IsCheckmate(_isCheckmate)
{
}

MoveInfo::MoveInfo(MoveInfo&& other) noexcept:
	_piecesMoved(std::move(other._piecesMoved)), _boardNotation(std::move(other._boardNotation)),
	_specialMoveFlags(other._specialMoveFlags), _piecePromotion(std::move(other._piecePromotion)),
	_capturedPiece(std::move(other._capturedPiece)),
	_isCheck(other._isCheck), _isCheckmate(other._isCheckmate),
	PiecesMoved(_piecesMoved), BoardNotation(_boardNotation),
	PieceCaptured(_capturedPiece),
	SpecialMoveFlags(_specialMoveFlags), PiecePromotion(_piecePromotion),
	IsCheck(_isCheck), IsCheckmate(_isCheckmate) 
{
	other._piecesMoved = {};
	other._boardNotation = "";
	other._specialMoveFlags = SpecialMove::None;
	other._piecePromotion = nullptr;
	other._piecePromotion = nullptr;
	other._isCheck = false;
	other._isCheckmate = false;
}

std::string MoveInfo::ToString() const
{
	return std::format("[PiecesMoved:{} SpecialMove:{} Piece Promoted:{} Captured:{} IsCheck:{} IsCheckmate: {}]", 
		Utils::ToStringIterable<const std::vector<MovePiecePositionData>, MovePiecePositionData>(PiecesMoved), 
		std::to_string(static_cast<unsigned int>(SpecialMoveFlags)), 
		PiecePromotion!=nullptr? PiecePromotion->ToString() : "NULL", 
		PieceCaptured!=nullptr ? PieceCaptured->ToString() : "NULL",
		std::to_string(IsCheck), std::to_string(IsCheckmate));
}

bool MoveInfo::operator==(const MoveInfo& otherInfo) const
{
	return PiecesMoved == otherInfo.PiecesMoved && BoardNotation == otherInfo.BoardNotation &&
		SpecialMoveFlags == otherInfo.SpecialMoveFlags && PiecePromotion == otherInfo.PiecePromotion &&
		PieceCaptured == otherInfo.PieceCaptured && IsCheck == otherInfo.IsCheck && IsCheckmate == otherInfo.IsCheckmate;
}

MoveInfo& MoveInfo::operator=(const MoveInfo& otherInfo) noexcept
{
	if (otherInfo == *this) return *this;

	_piecesMoved = otherInfo._piecesMoved;
	_boardNotation = otherInfo._boardNotation;
	_specialMoveFlags = otherInfo._specialMoveFlags;
	_piecePromotion = otherInfo._piecePromotion;
	_capturedPiece = otherInfo._capturedPiece;
	_isCheck = otherInfo._isCheck;
	_isCheckmate = otherInfo._isCheckmate;
	return *this;
}

std::string GameState::ToString() const
{
	const std::string piecesStr = Utils::ToStringIterable(InPlayPieces);
	const std::string capturedStr = Utils::ToStringIterable<std::vector<Piece*>, Piece*>(CapturedPieces);
	const std::string str = std::format("[GAME STATE-> player:{}, positions:{}, captured:{} inCheck:{}, inCheckmate:{}]", 
		::ToString(CurrentPlayer), piecesStr, capturedStr, std::to_string(InCheck), std::to_string(InCheckmate));
	return str;
}
