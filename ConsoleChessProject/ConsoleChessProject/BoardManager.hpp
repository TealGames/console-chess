#pragma once

#include <array>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <set>
#include "HelperFunctions.hpp"
#include "Point2DInt.hpp"
#include "Vector2D.hpp"
#include "Tile.hpp"
#include "Piece.hpp"
#include "Color.hpp"
#include "GameState.hpp"
#include "PieceMoveResult.hpp"

namespace Board
{
	/*enum class SpecialMove : unsigned int
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
	};*/

	//class MovePiecePositionData
	//{
	//private:
	//	//This needs to be a pointer since constructor
	//	//cannot initialize non const refs to const refs
	//	const Piece* _piece;
	//	Utils::Point2DInt _oldPos;
	//	Utils::Point2DInt _newPos;

	//public:
	//	const Piece& PieceRef;
	//	const Utils::Point2DInt& OldPos;
	//	const Utils::Point2DInt& NewPos;

	//	MovePiecePositionData(const Piece& piece, const Utils::Point2DInt& oldPos, const Utils::Point2DInt& newPos);
	//	MovePiecePositionData(const MovePiecePositionData& other);

	//	MovePiecePositionData& operator=(const MovePiecePositionData& other);
	//	bool operator==(const MovePiecePositionData& other) const;
	//};

	//class MoveInfo
	//{
	//private:
	//	//all the pieces moved and their NEW position
	//	std::vector<MovePiecePositionData> _piecesMoved;
	//	std::string _boardNotation;
	//	SpecialMove _specialMoveFlags;
	//	//Piece to promote to
	//	std::optional<Piece*> _piecePromotion;
	//	bool _isCheck;
	//	bool _isCheckmate;

	//public:
	//	//all the pieces moved and their NEW position
	//	const std::vector<MovePiecePositionData>& PiecesMoved;
	//	const std::string& BoardNotation = "";
	//	const SpecialMove& SpecialMoveFlags = SpecialMove::None;
	//	//Piece to promote to
	//	const std::optional<Piece*>& PiecePromotion;
	//	const bool& IsCheck = false;
	//	const bool& IsCheckmate = false;

	//	MoveInfo(const std::vector<MovePiecePositionData>& piecesMoved, const std::string& boardNotation, const SpecialMove& moveFlags,
	//		const std::optional<Piece*>& promotion, const bool& check, const bool& checkmate);

	//	bool operator==(const MoveInfo& otherInfo) const;
	//	MoveInfo& operator=(const MoveInfo& otherInfo);
	//};

	constexpr int TEAMS_COUNT = 2;
	constexpr int COLOR_PIECES_COUNT = 16;
	constexpr int NULL_POS = -1;
	const Utils::Point2DInt INVALID_MOVE = { NULL_POS, NULL_POS };

	constexpr char NOTATION_CAPTURE_CHAR = 'x';
	constexpr char NOTATION_PROMOTION_CHAR = '=';
	constexpr char NOTATION_CHECK_CHAR = '+';
	constexpr char NOTATION_CHECKMATE_CHAR = '#';
	const std::string NOTATION_KINGSIDE_CASTLE = "O-O";
	const std::string NOTATION_QUEENSIDE_CASTLE = "O-O-O";

	bool IsWithinBounds(const Utils::Point2DInt& pos);
	//bool InCheck();
	//bool InCheckmate();

	void ResetBoard(const GameState& state);
	void CreateDefaultBoard(GameState& state);
	const Piece* TryGetPieceAtPosition(const GameState& state, const Utils::Point2DInt& pos);
	std::optional<Utils::Point2DInt> TryGetPositionOfPiece(const GameState& state, const Piece& piece);

	bool HasPieceWithinPositionRange(const GameState& state, const Utils::Point2DInt& startPos, 
									 const Utils::Point2DInt& endPos, bool inclusive);
	std::vector<PiecePositionData> TryGetPiecesPosition(const GameState& state, const ColorTheme& color,
		const std::optional<PieceType>& type, const std::optional<std::vector<Piece::State>>& targetState);
	//const std::unordered_map<Utils::Point2DInt, Piece>& GetAllPieces();
	std::string GetPiecePositionsAsString(const GameState& state);
	size_t GetAvailablePieces(const GameState& state, ColorTheme& color);

	const std::vector<MoveInfo> GetPreviousMoves(const GameState& state, const ColorTheme& color);
	const MoveInfo* GetPieceFirstMove(const GameState& state, const ColorTheme& color, const PieceType& type);
	bool HasMovedPiece(const GameState& state, const ColorTheme& color, const PieceType& type);

	std::vector<MoveInfo> GetPossibleMovesForPieceAt(const GameState& state, const Utils::Point2DInt& pos);
	PieceMoveResult TryMove(GameState& state, const Utils::Point2DInt& currentPos, const Utils::Point2DInt& moveToPos);

	std::string CleanInput(const std::string& input);

	using PieceMoveCallbackType = std::function<void(const GameState&)>;
	void AddPieceMoveCallback(const PieceMoveCallbackType& callback);

	using MoveExecutedCallbackType = std::function<void(const GameState&)>;
	void AddMoveExecutedCallback(const MoveExecutedCallbackType& callback);
	//TODO: add parse/serialization method to convert moveinfo to chess notation input
}

