#include <format>
#include <vector>
#include <optional>
#include <algorithm>
#include "BoardManager.hpp"
#include "BoardSetup.hpp"
#include "Point2DInt.hpp"
#include "Vector2D.hpp"
#include "Piece.hpp"
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"
#include "Globals.hpp"

MoveResult::MoveResult(const Utils::Point2DInt& pos, const bool isValid, const std::string& info)
	:AttemptedPositions({pos}), IsValidMove(isValid), Info(info) {}

MoveResult::MoveResult(const std::vector<Utils::Point2DInt>& positions, const bool isValid, const std::string& info)
	:AttemptedPositions(positions), IsValidMove(isValid), Info(info) {}

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

using PiecePositionMapType = std::unordered_map<Utils::Point2DInt, Piece>;
static PiecePositionMapType piecePositions;
static std::unordered_map<ColorTheme, std::vector<MoveInfo>> previousMoves;

static bool inCheckmate;
static bool inCheck;

bool InCheck()
{
	return inCheck;
}

bool InCheckmate()
{
	return inCheckmate;
}

static std::vector<const Piece*> MakeImmutable(std::vector<Piece*> pieces)
{
	std::vector<const Piece*> immutablePieces;
	for (const auto& piece : pieces)
	{
		immutablePieces.push_back(piece);
	}
	return immutablePieces;
}

struct PiecesRange
{
	const std::vector<Utils::Point2DInt> keys;
	const std::vector<Piece*> values;
};

static const PiecesRange GetPiecesForColorMutable(const ColorTheme color)
{
	std::vector<Utils::Point2DInt> keys;
	std::vector<Piece*> values;

	//cant be a const iteration since we need to get a pointer to a not const value
	for (auto& piecePosition : piecePositions)
	{
		if (piecePosition.second.color == color)
		{
			keys.push_back(piecePosition.first);
			values.push_back(&piecePosition.second);
		}
	}
	return { keys, values };
}

//Will get the piece at the specified position using .find for map (fast)
Piece* GetPieceAtPositionMutable(const Utils::Point2DInt& pos)
{
	PiecePositionMapType::iterator it = piecePositions.find(pos);
	if (it == piecePositions.end()) return nullptr;
	else return &(it->second);
}

inline bool TryGetPieceAtPosition(const Utils::Point2DInt& pos, const Piece* outPiece= nullptr)
{
	const Piece* piecePtr = GetPieceAtPositionMutable(pos);
	outPiece = piecePtr;
	return piecePtr != nullptr;
}

static bool TryGetPieceAtPositionMutable(const Utils::Point2DInt& pos, Piece* outPiece = nullptr)
{
	Piece* piecePtr = GetPieceAtPositionMutable(pos);
	outPiece = piecePtr;
	return piecePtr != nullptr;
}

std::optional<Utils::Point2DInt> TryGetPositionOfPiece(const Piece& piece)
{
	for (const auto& piecePos : piecePositions)
	{
		if (piecePos.second == piece) return piecePos.first;
	}
	return std::nullopt;
}

bool HasPieceWithinPositionRange(const Utils::Point2DInt& startPos, const Utils::Point2DInt& endPos, bool inclusive)
{
	Utils::Point2DInt currentPosition = startPos;
	Utils::Point2DInt endPosition = endPos;

	int xTotalDelta = endPosition.x - currentPosition.x;
	int yTotalDelta = endPosition.y - currentPosition.y;
	int xCurrentDelta = 0;
	int yCurrentDelta = 0;

	while (currentPosition!=endPosition)
	{
		//If we go perfect diagonal we increase both
		if (xTotalDelta == yTotalDelta)
		{
			xCurrentDelta = Utils::GetSign(xTotalDelta);
			yCurrentDelta = Utils::GetSign(yTotalDelta);
		}
		//Next if we have perfect vertical we only increase y
		else if (Utils::ApproximateEquals(currentPosition.x, endPosition.x))
		{
			xCurrentDelta = 0;
			yCurrentDelta = Utils::GetSign(yTotalDelta);
		}
		//If we have perfect horizontal we go x
		//OR if we have not uniformed diagonal, we first get x
		else
		{
			xCurrentDelta = Utils::GetSign(xTotalDelta);
			yCurrentDelta = 0;
		}

		currentPosition = { currentPosition.x + xCurrentDelta, currentPosition.y + yCurrentDelta };
		if (TryGetPieceAtPosition(currentPosition)) return true;

		if (currentPosition == endPosition) break;
	}
	return false;
}

std::vector<const Piece*> TryGetAvailablePieces(const ColorTheme& color, const PieceType& type)
{
	std::vector<const Piece*> foundPieces;
	for (const auto& piecePos : piecePositions)
	{
		if (piecePos.second.state == Piece::State::InPlay &&
			piecePos.second.color == color && piecePos.second.pieceType == type)
			foundPieces.push_back(&piecePos.second);
	}
	return foundPieces;
}

std::vector<PiecePositionData> TryGetAvailablePiecesPosition(const ColorTheme& color, const PieceType& type)
{
	std::vector<PiecePositionData> foundPieces;
	for (const auto& piecePos : piecePositions)
	{
		if (piecePos.second.state == Piece::State::InPlay &&
			piecePos.second.color == color && piecePos.second.pieceType == type)
			foundPieces.emplace_back(piecePos.second, piecePos.first);
	}
	return foundPieces;
}

std::vector<PiecePositionData> TryGetAvailablePiecesPosition(const ColorTheme& color)
{
	std::vector<PiecePositionData> positions;
	for (const auto& piecePos : piecePositions)
	{
		if (!piecePos.second.state == Piece::State::InPlay &&
			piecePos.second.color == color) positions.emplace_back(piecePos.second, piecePos.first);
	}
	return positions;
}

int GetAvailablePieces(const ColorTheme& color)
{
	return TryGetAvailablePiecesPosition(color).size();
}

static bool MoveInfoHasData(const std::vector<MoveInfo>& allMoveInfo, const std::function<bool(MoveInfo)> iterationCheck)
{
	for (const auto& moveInfo : allMoveInfo)
	{
		if (iterationCheck(moveInfo)) return true;
	}
	return false;
}

static bool EraseMoveInfoWithData(std::vector<MoveInfo>& allMoveInfo, const std::function<bool(MoveInfo)> iterationCheck)
{
	int index = 0;
	for (const auto& moveInfo : allMoveInfo)
	{
		if (iterationCheck(moveInfo))
		{
			allMoveInfo.erase(allMoveInfo.begin()+index);
			return true;
		}
		index++;
	}
	return false;
}

//Will check if the position is within bounds of the board
bool IsWithinBounds(const Utils::Point2DInt& pos)
{
	if (pos.x < 0 || pos.y >= BOARD_DIMENSION) return false;
	else if (pos.x < 0 || pos.y >= BOARD_DIMENSION) return false;
	return true;
}

void ResetBoard()
{
	/*std::array<Tile, BOARD_DIMENSION> emptyRow = {};
	tiles.fill(emptyRow);*/
	piecePositions.clear();
}

static bool IsCheckOrMateAtPiece(const Utils::Point2DInt& posAttemptingCheck, bool updateGlobal)
{
	const Piece* pieceAttemptingCheck = nullptr;
	TryGetPieceAtPosition(posAttemptingCheck, pieceAttemptingCheck);
	const std::vector<MoveInfo> possibleMoves = GetPossibleMovesForPieceAt(posAttemptingCheck);
	
	const Piece* pieceChecked = nullptr;
	bool inCheckResult = false;
	if (pieceAttemptingCheck != nullptr)
	{
		for (const auto& possibleMove : possibleMoves)
		{
			for (const auto& pieceMove : possibleMove.PiecesMoved)
			{
				//If this move is not for this piece we do not consider it
				if (&pieceMove.PieceRef != pieceAttemptingCheck) continue;

				//If the piece at the end of the path is a king of a different color it is a check
				TryGetPieceAtPosition(pieceMove.NewPos, pieceChecked);
				if (pieceChecked != nullptr && pieceChecked->pieceType == PieceType::King &&
					pieceChecked->color != pieceAttemptingCheck->color)
				{
					inCheckResult = true;
					break;
				}
			}
		}
	}

	bool inCheckmateResult = false;
	if (inCheckResult)
	{
		std::vector<PiecePositionData> kingPositions = TryGetAvailablePiecesPosition(pieceAttemptingCheck->color ==
			ColorTheme::Dark ? ColorTheme::Light : ColorTheme::Dark, PieceType::King);
		if (kingPositions.size() != 1)
		{
			std::string error = std::format("Tried to check for check and checkmate but no king was found!");
			Utils::Log(Utils::LogType::Error, error);
			return false;
		}
		const std::vector<MoveInfo> kingMoves = GetPossibleMovesForPieceAt(kingPositions[0].Pos);
		inCheckmateResult = kingMoves.size() == 0;
	}

	if (updateGlobal)
	{
		inCheck = inCheckResult;
		inCheckmate = inCheckmateResult;
	}
	
	return inCheckResult || inCheckmateResult;
}

static std::vector<PiecePositionData> GetCheckablePositions(const ColorTheme& color)
{
	std::vector<PiecePositionData> checkablePiecePositions;
	for (const auto& piece : piecePositions)
	{
		if (piece.second.color != color) continue;
		if (IsCheckOrMateAtPiece(piece.first, false))
			checkablePiecePositions.emplace_back(piece.second, piece.first);
	}

	return checkablePiecePositions;
}

static bool TryUpdatePiecePosition(const PiecePositionData& currentData, const Utils::Point2DInt& newPos)
{
	if (!TryGetPieceAtPosition(currentData.Pos, &currentData.PieceRef))
	{
		std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
			"but it is not located there", currentData.PieceRef.ToString(), currentData.Pos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}

	if (!IsWithinBounds(currentData.Pos))
	{
		std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
			"but current pos is not valid", currentData.PieceRef.ToString(), currentData.Pos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}

	if (!IsWithinBounds(newPos))
	{
		std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
			"to pos: {} but new pos is not valid", currentData.PieceRef.ToString(), currentData.Pos.ToString(), newPos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}

	size_t elementsErased = piecePositions.erase(currentData.Pos);
	if (elementsErased == 0)
	{
		std::string error = std::format("Tried to place board piece: {} at pos: {} "
			"but failed to retrieve it and/or remove it from its old position {}",
			currentData.PieceRef.ToString(), newPos.ToString(), currentData.Pos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}
	piecePositions.emplace(newPos, currentData.PieceRef);
}

static bool TryUpdatePiecePosition(const PiecePositionData currentData, Utils::Point2DInt newPos, MoveInfo moveInfo)
{
	TryUpdatePiecePosition(currentData, newPos);

	ColorTheme color = currentData.PieceRef.color;
	if (previousMoves.find(color) == previousMoves.end())
	{
		previousMoves.emplace(color, std::vector<MoveInfo>{moveInfo });
	}	
	else
	{
		previousMoves.at(color).push_back(moveInfo);
	}
}

static void PlaceDefaultBoardPieces(const ColorTheme& color, bool overrideExisting=true)
{
	const PiecesRange ranges = GetPiecesForColorMutable(color);

	const std::vector<Utils::Point2DInt> oldPositions = ranges.keys;
	const std::vector<Piece*> piecePointers = ranges.values;

	int pieceIndex = 0;
	for (const auto& position : GetPositionsForPieces(color, MakeImmutable(piecePointers)))
	{
		if (!IsWithinBounds(position))
		{
			std::string error = std::format("Tried to place default board piece: {} at pos: {} "
				"but is invalid pos", piecePointers[pieceIndex]->ToString(), position.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return;
		}
		pieceIndex++;

		const Piece* outPiece = nullptr;
		if (!overrideExisting && TryGetPieceAtPosition(position, outPiece))
		{
			std::string error = std::format("Tried to place default board piece: {} at pos: {} "
				"but piece: {} already exists and overriding is forbidden", 
				piecePointers[pieceIndex]->ToString(), position.ToString(), outPiece->ToString());
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		Piece* pieceData = piecePointers[pieceIndex];
		if (!TryUpdatePiecePosition(PiecePositionData{ *pieceData, oldPositions[pieceIndex] }, position))
		{
			std::string error = std::format("Tried to place default board piece: {} at pos: {} "
				"but failed to update its position {}",
				piecePointers[pieceIndex]->ToString(), position.ToString(), oldPositions[pieceIndex].ToString());
			Utils::Log(Utils::LogType::Error, error);
			return;
		}
		pieceData->UpdateState(Piece::State::InPlay);
	}
}

void CreateDefaultBoard()
{
	ResetBoard();
	
	PlaceDefaultBoardPieces(ColorTheme::Dark);
	PlaceDefaultBoardPieces(ColorTheme::Light);
}

struct CastleInfo
{
	const bool canCastle;
	const bool isKingSide;
	const bool isQueenSide;

	const Utils::Point2DInt kingSideCastleMove;
	const Utils::Point2DInt queenSideCastleMove;
};

static CastleInfo CanCastle(const ColorTheme& color)
{
	//Note: we dont want to check rook because it would check any rook moved and 
	//we might still be able to castle on one side and not on the other
	if (HasMovedPiece(color, PieceType::King)) return {false, false, false};
	const std::vector<const Piece*> rooks = TryGetAvailablePieces(color, PieceType::Rook);

	//TODO: optimize these calls so they are batched together to find pos for both rook and king at the same time
	const std::vector<PiecePositionData> kingPositions = TryGetAvailablePiecesPosition(color, PieceType::King);
	if (kingPositions.size() != 1) return { false, false, false };
	Utils::Point2DInt kingPos = kingPositions[0].Pos;

	const std::vector<PiecePositionData> rookPositions = TryGetAvailablePiecesPosition(color, PieceType::Rook);
	if (rookPositions.size() <= 0 || rookPositions.size() > 2) return { false, false, false };
	
	bool canKingSide = true;
	bool canQueenSide = true;
	bool checkingKingSide = false;
	Utils::Point2DInt currentPosCheck;
	for (const auto& rookPos : rookPositions)
	{
		if (rookPos.Pos.x != kingPos.x) continue;

		currentPosCheck = kingPos;
		checkingKingSide = rookPos.Pos.y > currentPosCheck.y ? true : false;
		while (currentPosCheck.y != rookPos.Pos.y)
		{
			//King side goes up the cols queen side goes down
			if (checkingKingSide) currentPosCheck = { currentPosCheck.x, currentPosCheck.y + 1 };
			else currentPosCheck = { currentPosCheck.x, currentPosCheck.y - 1 };

			//Since we do the decrement/incremenet above, it can check to rook pos after loop condition
			if (currentPosCheck.y >= rookPos.Pos.y) break;

			if (TryGetPieceAtPosition(currentPosCheck))
			{
				if (checkingKingSide) canKingSide = false;
				else canQueenSide = false;
				break;
			}
		}
	}

	Utils::Point2DInt kingSideCastleMove = canKingSide ? Utils::Point2DInt{ kingPos.x, kingPos.y + 2 } : Utils::Point2DInt{};
	Utils::Point2DInt queenSideCastleMove = canKingSide ? Utils::Point2DInt{ kingPos.x, kingPos.y - 2 } : Utils::Point2DInt{};
	return { canKingSide || canQueenSide, canKingSide, canQueenSide, kingSideCastleMove, queenSideCastleMove};
}
static CastleInfo IsCastleMove(const PiecePositionData currentData, const Utils::Point2DInt& newPos)
{
	if (currentData.PieceRef.pieceType != PieceType::King) return { false, false, false };

	int startCol = std::min(currentData.Pos.y, newPos.y);
	int endCol = std::max(currentData.Pos.y, newPos.y);
	int delta = endCol - startCol;

	const Piece* outPiece;
	int rookCol = delta > 0 ? BOARD_DIMENSION - 1 : 0;
	//If in the direction moved is NOT a rook at the end, it means we do not have castle chance
	if (!TryGetPieceAtPosition({ currentData.Pos.x , rookCol}, outPiece)) return { false, false, false };
	if (outPiece == nullptr || outPiece->pieceType != PieceType::Rook) return { false, false, false };

	//If we move kingside it is 3, otherwise it is 4 diff
	if (std::abs(delta) != 3 && std::abs(delta) != 4) return { false, false, false };

	for (int i = startCol + 1; i <= endCol - 1; i++)
	{
		//If there is a piece in the way it means we cannot castle
		if (TryGetPieceAtPosition({ currentData.Pos.x, i }, outPiece)) 
			return { false, false, false };
	}
	//Delta >0 means moves up, <0 means moves down queen side
	return { true, delta>0, delta < 0 };
}

static bool IsCapture(const PiecePositionData currentData, 
	const Utils::Point2DInt newPos, const Piece* outCapturedPiece = nullptr)
{
	const Piece* outPieceAtNewPos = nullptr;
	outCapturedPiece = outPieceAtNewPos;
	if (!TryGetPieceAtPosition(newPos, outPieceAtNewPos))return false;

	if (!DoesMoveDeltaMatchCaptureMoves(currentData.PieceRef.pieceType, currentData.Pos, newPos))
		return false;

	//TODO: are there any other checks to capture a piece
	return true;
}

std::vector<MoveInfo> GetPossibleMovesForPieceAt(const Utils::Point2DInt& startPos)
{
	if (!IsWithinBounds(startPos))
		return {};

	Piece* movedPiece;
	if (!TryGetPieceAtPosition(startPos, movedPiece))
		return {};

	std::vector<MoveInfo> possibleMoves;
	Utils::Point2DInt moveNewPos;
	const std::vector<Utils::Vector2D> captureMoves = GetCaptureMovesForPiece(movedPiece->pieceType);
	for (auto& movePos : GetMoveDirsForPiece(movedPiece->pieceType))
	{
		moveNewPos = GetVectorEndPoint(startPos, movePos);
		if (!HasPieceWithinPositionRange(startPos, moveNewPos) && IsWithinBounds(moveNewPos))
		{
			//We have to check if it is a capture because capture moves might not be different
			//from move dirs so we might capture during regualar moves
			SpecialMove specialMove = IsCapture(PiecePositionData{ *movedPiece, startPos }, moveNewPos) ? 
				SpecialMove::Capture : SpecialMove::None;

			possibleMoves.emplace_back(
				std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, moveNewPos)
				},
				"",
				specialMove,
				std::nullopt,
				false,
				false
			);
		}
	}

	for (auto& captureMove : captureMoves)
	{
		moveNewPos = GetVectorEndPoint(startPos, captureMove);
		if (!HasPieceWithinPositionRange(startPos, moveNewPos) && IsWithinBounds(moveNewPos))
		{
			possibleMoves.emplace_back(
				std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, moveNewPos)
				},
				"",
				SpecialMove::Capture,
				std::nullopt,
				false,
				false
			);
		}
	}

	CastleInfo castleInfo = CanCastle(movedPiece->color);
	if (movedPiece->pieceType == PieceType::King && castleInfo.canCastle)
	{
		if (castleInfo.isKingSide)
		{
			possibleMoves.emplace_back(
				std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, castleInfo.kingSideCastleMove)
				},
				"",
				SpecialMove::Capture,
				std::nullopt,
				false,
				false
			);
		}
		else if (castleInfo.isQueenSide)
		{
			possibleMoves.emplace_back(
				std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, castleInfo.queenSideCastleMove)
				},
				"",
				SpecialMove::Capture,
				std::nullopt,
				false,
				false
			);
		}

		//Kings also get checked for any positions that are able to be checked
		//and those positions are illegal and are removed
		std::vector<PiecePositionData> checkPositions = GetCheckablePositions(movedPiece->color);
		if (checkPositions.size() != 0)
		{
			for (const auto& checkPos : checkPositions)
			{
				EraseMoveInfoWithData(possibleMoves, [&checkPos, movedPiece](MoveInfo info) -> bool
					{ 
						for (const auto& move : info.PiecesMoved)
						{
							if (&move.PieceRef != movedPiece) continue;
							if (move.NewPos == checkPos.Pos) return true;
						}
						return false;
					});
			}
		}
	}
	
	return possibleMoves;
}

//Will check if it is possible to move to that point using a variety of bounds checks,
//valid moves, and special move checks
MoveResult TryMove(const Utils::Point2DInt& currentPos, const Utils::Point2DInt& newPos)
{
	if (!IsWithinBounds(currentPos))
		return { newPos, false, std::format("Tried to move from a place outside the board") };

	Piece* movedPiece;
	if (!TryGetPieceAtPosition(currentPos, movedPiece))
		return { newPos, false, std::format("Position ({}) has no piece", newPos.ToString()) };

	if (!IsWithinBounds(newPos))
		return { newPos, false, std::format("Tried to move to a place outside the board") };

	const std::vector<MoveInfo> possibleMoves = GetPossibleMovesForPieceAt(currentPos);
	if (!possibleMoves.size()==0)
		return { newPos, false, std::format("There are no possible moves for this piece") };

	for (const auto& moveInfo : possibleMoves)
	{
		for (const auto& move : moveInfo.PiecesMoved)
		{
			//If the piece moved for this move is not the one we want we skip
			if (&move.PieceRef != movedPiece) continue;

			if (move.NewPos == newPos)
			{
				TryUpdatePiecePosition(PiecePositionData{ *movedPiece, currentPos }, newPos, moveInfo);
				return { newPos, true };
			}
		}
	}
	return { newPos, false, std::format("New pos does not match any pos for this piece") };

	//Piece* movedPiece;
	//if (!HasPieceAtPosition(currentPos, movedPiece))
	//	return { newPos, false, std::format("Position ({}) has no piece", newPos.ToString()) };
	//
	//if (!IsWithinBounds(newPos))
	//	return { newPos, false, std::format("Position ({}) is out of bounds", newPos.ToString()) };

	////Based on the castle info we assume to add the rook position 
	////based on if it is kingside or queenside
	//PiecePositionData currentPosData = { movedPiece, currentPos };
	//if (CastleInfo castleInfo= IsCastleMove(currentPosData, newPos); castleInfo.canCastle)
	//{
	//	std::vector<Utils::Position2D> rookKingMoves = { newPos };

	//	if (castleInfo.isKingSide)
	//		rookKingMoves.emplace_back(newPos.x, newPos.y-1);
	//	else if (castleInfo.isQueenSide)
	//		rookKingMoves.emplace_back(newPos.x, newPos.y+1);
	//	else 
	//	{
	//		std::string error = std::format("Tried to move piece: {} at pos: {} "
	//			"but failed to update its position: {} to castle move since it is neither kingside nor queenside",
	//			currentPosData.piece->ToString(), currentPosData.pos.ToString(), newPos.ToString());
	//		Utils::Log(Utils::LogType::Error, error);
	//		return {newPos, false};
	//	}
	//	return { rookKingMoves, true };
	//}

	////Update position 
	//Piece* takenPiece = nullptr;
	//if (IsCapture(currentPosData, newPos, takenPiece))
	//{
	//	if (!TryUpdatePiecePosition({ movedPiece, currentPos }, newPos))
	//	{
	//		return { newPos, false, std::format("Tried to place default board piece: {} at pos: {} "
	//			"due to capture but failed to update its position {}",
	//			movedPiece->ToString(), currentPos.ToString(), newPos.ToString()) };
	//	}
	//	takenPiece->UpdateState(Piece::State::Captured);
	//	return { newPos, true };
	//}

	//if (DoesMoveDeltaMatchPieceMoves(movedPiece->pieceType, currentPos, newPos))
	//{
	//	if (!TryUpdatePiecePosition({ movedPiece, currentPos }, newPos))
	//	{
	//		return { newPos, false, std::format("Tried to place default board piece: {} at pos: {} "
	//			"due to move delta but failed to update its position {}",
	//			movedPiece->ToString(), currentPos.ToString(), newPos.ToString()) };
	//	}
	//	return { newPos, true };
	//}
	//	

	////TODO: add special move checks like castleing, taking, etc.

	//return { newPos, false, std::format("Move Delta from {} -> {} does not match any possible moves for {}",
	//		currentPos.ToString(), newPos.ToString(), ToString(movedPiece->pieceType)) };
} 

std::string CleanInput(const std::string& input)
{
	std::string cleaned = Utils::StringUtil(input).Trim().RemoveChar(' ').ToString();
	return cleaned;
}

//std::optional<MoveInfo> TryParseMoveInfoFromMove(const ColorTheme& color, const std::string& input)
//{
//	std::string cleanedInput = CleanInput(input);
//	if (cleanedInput == NOTATION_KINGSIDE_CASTLE)
//	{
//		MoveInfo kingsideCastleInfo = { .PiecesMoved = {PieceType::King, PieceType::Rook}, 
//			.SpecialMoveFlags= SpecialMove::KingSideCastle};
//		return kingsideCastleInfo;
//	}
//	else if (cleanedInput == NOTATION_QUEENSIDE_CASTLE)
//	{
//		MoveInfo queensideCastleInfo = { .PiecesMoved = {PieceType::King, PieceType::Rook},
//			.SpecialMoveFlags = SpecialMove::QueenSideCastle };
//		return queensideCastleInfo;
//	}
//
//	std::optional<PieceType> pieceMoved = TryGetPieceFromNotationSymbol(cleanedInput[0]);
//	std::optional<Utils::Position2D> optionalCurrentPosInfo;
//
//	int colStringIndex = 1;
//	int rowStringIndex = 2;
//	bool isCapture = false;
//	//If we have capture character, we have to increase to the 
//	//next indices to search for new pos
//	if (cleanedInput[1] == NOTATION_CAPTURE_CHAR)
//	{
//		colStringIndex++;
//		rowStringIndex++;
//		isCapture = true;
//	}
//	//If same piece can move to a spot we provide disambigiuoty check
//	//and can provide an extra row
//	else if (Utils::IsNumber(cleanedInput[colStringIndex]))
//	{
//		optionalCurrentPosInfo = { std::stod(&cleanedInput[colStringIndex]), NULL_POS,};
//		colStringIndex++;
//		rowStringIndex++;
//	}
//	//here is the disambigiuoty check for extra col
//	else if (Utils::IsLetter(cleanedInput[colStringIndex]) && Utils::IsLetter(cleanedInput[rowStringIndex]))
//	{
//		optionalCurrentPosInfo = { NULL_POS, cleanedInput[colStringIndex] - 'a'};
//		colStringIndex++;
//		rowStringIndex++;
//	}
//
//	auto possiblePieces = TryGetAvailablePiecesPosition();
//	
//	char col = cleanedInput[colStringIndex];
//	char row = cleanedInput[rowStringIndex];
//	Utils::Position2D newPos(std::atoi(&row), col - 'a');
//
//	std::optional<PieceType> promotePiece = std::nullopt;
//	if (cleanedInput[rowStringIndex + 1] == NOTATION_PROMOTION_CHAR)
//	{
//		promotePiece= TryGetPieceFromNotationSymbol(cleanedInput[rowStringIndex+1]);
//	}
//
//	bool isCheck = cleanedInput.back() == NOTATION_CHECK_CHAR;
//	bool isCheckmate = cleanedInput.back() == NOTATION_CHECKMATE_CHAR;
//	
//	SpecialMove moveFlags = SpecialMove::None;
//	if (promotePiece.has_value()) moveFlags |= SpecialMove::Promotion;
//	if (isCapture) moveFlags |= SpecialMove::Capture;
//
//	MoveInfo info =
//	{
//		pieceMoved != {GetPieceAtPosition()},
//		optionalCurrentPosInfo,
//		newPos, 
//		cleanedInput,
//		moveFlags,
//		promotePiece,
//		isCheck,
//		isCheckmate
//	};
//	return info;
//}

const std::vector<MoveInfo>& GetPreviousMoves(const ColorTheme& color)
{
	if (previousMoves.size() == 0) return {};
	if (previousMoves.find(color) == previousMoves.end()) return {};

	return previousMoves.at(color);
}

bool HasMovedPiece(const ColorTheme& color, const PieceType& type, const MoveInfo* outFirstMove) 
{	
	const std::vector<MoveInfo> colorMoves = GetPreviousMoves(color);
	if (colorMoves.size() == 0) return false;

	for (const auto& move : colorMoves)
	{
		if (move.PiecesMoved.size() == 0) continue;
		for (const auto& piecesMoved : move.PiecesMoved)
		{
			if (piecesMoved.PieceRef.pieceType == type)
			{
				outFirstMove = &move;
				return true;
			}
		}
	}
	return false;
}