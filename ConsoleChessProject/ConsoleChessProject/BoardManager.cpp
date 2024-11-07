#include <format>
#include <vector>
#include <optional>
#include <algorithm>
#include "BoardManager.hpp"
#include "BoardSetup.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "Piece.hpp"
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"
#include "Globals.hpp"

MoveResult::MoveResult(const Utils::Position2D& pos, const bool isValid, const std::string& info)
	:AttemptedPositions({pos}), IsValidMove(isValid), Info(info) {}

MoveResult::MoveResult(const std::vector<Utils::Position2D>& positions, const bool isValid, const std::string& info)
	:AttemptedPositions(positions), IsValidMove(isValid), Info(info) {}

//This stores 
using PiecePositionMapType = std::unordered_map<Utils::Position2D, Piece*>;
static PiecePositionMapType piecePositions;
static std::unordered_map<ColorTheme, std::vector<MoveInfo>> previousMoves;

struct PiecesRange
{
	const std::vector<Utils::Position2D> keys;
	const std::vector<Piece*> values;
};

struct PiecePositionData
{
	Piece* piece;
	const Utils::Position2D pos;
};

//We use the assumption that light is stored first followed by dark to retrieve
//the iterators to be used for any purpose
static const PiecesRange GetPiecesForColor(const ColorTheme color)
{
	std::vector<Utils::Position2D> keys;
	std::vector<Piece*> values;
	for (const auto& piecePosition : piecePositions)
	{
		if (piecePosition.second->color == color)
		{
			keys.push_back(piecePosition.first);
			values.push_back(piecePosition.second);
		}
	}
	return { keys, values };
}

//Will get the piece at the specified position using .find for map (fast)
Piece* GetPieceAtPosition(const Utils::Position2D& pos)
{
	auto it = piecePositions.find(pos);
	if (it == piecePositions.end()) return nullptr;
	else return it->second;
}

inline bool HasPieceAtPosition(const Utils::Position2D& pos, const Piece* const outPiece)
{
	const Piece* piecePtr = GetPieceAtPosition(pos);
	return piecePtr != nullptr;
}

//Will check if the position is within bounds of the board
static bool IsValidPosition(const Utils::Position2D& pos)
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

static bool TryUpdatePiecePosition(const PiecePositionData currentData, Utils::Position2D newPos)
{
	if (!HasPieceAtPosition(currentData.pos, currentData.piece))
	{
		std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
			"but it is not located there", currentData.piece->ToString(), currentData.pos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}

	if (!IsValidPosition(currentData.pos))
	{
		std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
			"but current pos is not valid", currentData.piece->ToString(), currentData.pos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}

	if (!IsValidPosition(newPos))
	{
		std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
			"to pos: {} but new pos is not valid", currentData.piece->ToString(), currentData.pos.ToString(), newPos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}

	size_t elementsErased = piecePositions.erase(currentData.pos);
	if (elementsErased == 0)
	{
		std::string error = std::format("Tried to place board piece: {} at pos: {} "
			"but failed to retrieve it and/or remove it from its old position {}",
			currentData.piece->ToString(), newPos.ToString(), currentData.pos.ToString());
		Utils::Log(Utils::LogType::Error, error);
		return;
	}
	piecePositions.emplace(newPos, currentData.piece );
}

static void PlaceDefaultBoardPieces(const ColorTheme& color, bool overrideExisting=true)
{
	const PiecesRange ranges = GetPiecesForColor(color);

	const std::vector<Utils::Position2D> oldPositions = ranges.keys;
	const std::vector<Piece*> piecePointers = ranges.values;

	int pieceIndex = 0;
	for (const auto& position : GetPositionsForPieces(color, piecePointers))
	{
		if (!IsValidPosition(position))
		{
			std::string error = std::format("Tried to place default board piece: {} at pos: {} "
				"but is invalid pos", piecePointers[pieceIndex]->ToString(), position.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return;
		}
		pieceIndex++;

		const Piece* outPiece;
		if (!overrideExisting && HasPieceAtPosition(position, outPiece))
		{
			std::string error = std::format("Tried to place default board piece: {} at pos: {} "
				"but piece: {} already exists and overriding is forbidden", 
				piecePointers[pieceIndex]->ToString(), position.ToString(), outPiece->ToString());
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		Piece* pieceData = piecePointers[pieceIndex];
		if (!TryUpdatePiecePosition({ pieceData, oldPositions[pieceIndex] }, position))
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
	const bool isCastle;
	const bool isKingSide;
	const bool isQueenSide;
};

static CastleInfo IsCastleMove(const PiecePositionData currentData, const Utils::Position2D& newPos)
{
	if (currentData.piece->pieceType != PieceType::King) return { false, false, false };

	int startCol = std::min(currentData.pos.y, newPos.y);
	int endCol = std::max(currentData.pos.y, newPos.y);
	int delta = endCol - startCol;

	const Piece* outPiece;
	int rookCol = delta > 0 ? BOARD_DIMENSION - 1 : 0;
	//If in the direction moved is NOT a rook at the end, it means we do not have castle chance
	if (!HasPieceAtPosition({ currentData.pos.x , rookCol }, outPiece)) return { false, false, false };
	if (outPiece == nullptr || outPiece->pieceType != PieceType::Rook) return { false, false, false };

	//If we move kingside it is 3, otherwise it is 4 diff
	if (std::abs(delta) != 3 && std::abs(delta) != 4) return { false, false, false };

	for (int i = startCol + 1; i <= endCol - 1; i++)
	{
		//If there is a piece in the way it means we cannot castle
		if (HasPieceAtPosition({ currentData.pos.x, i }, outPiece)) 
			return { false, false, false };
	}
	//Delta >0 means moves up, <0 means moves down queen side
	return { true, delta>0, delta < 0 };
}

static bool IsCapture(const PiecePositionData currentData, 
	const Utils::Position2D newPos, const Piece* outCapturedPiece = nullptr)
{
	const Piece* outPieceAtNewPos = nullptr;
	outCapturedPiece = outPieceAtNewPos;
	if (!HasPieceAtPosition(newPos, outPieceAtNewPos))return false;

	if (!DoesMoveDeltaMatchCaptureMoves(currentData.piece->pieceType, currentData.pos, newPos)) 
		return false;

	//TODO: are there any other checks to capture a piece
	return true;
}

//Will check if it is possible to move to that point using a variety of bounds checks,
//valid moves, and special move checks
MoveResult TryMove(const Utils::Position2D& currentPos, const Utils::Position2D& newPos)
{
	Piece* movedPiece;
	if (!HasPieceAtPosition(currentPos, movedPiece))
		return { newPos, false, std::format("Position ({}) has no piece", newPos.ToString()) };
	
	if (!IsValidPosition(newPos))
		return { newPos, false, std::format("Position ({}) is out of bounds", newPos.ToString()) };

	//Based on the castle info we assume to add the rook position 
	//based on if it is kingside or queenside
	PiecePositionData currentPosData = { movedPiece, currentPos };
	if (CastleInfo castleInfo= IsCastleMove(currentPosData, newPos); castleInfo.isCastle)
	{
		std::vector<Utils::Position2D> rookKingMoves = { newPos };

		if (castleInfo.isKingSide)
			rookKingMoves.emplace_back(newPos.x, newPos.y-1);
		else if (castleInfo.isQueenSide)
			rookKingMoves.emplace_back(newPos.x, newPos.y+1);
		else 
		{
			std::string error = std::format("Tried to move piece: {} at pos: {} "
				"but failed to update its position: {} to castle move since it is neither kingside nor queenside",
				currentPosData.piece->ToString(), currentPosData.pos.ToString(), newPos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return {newPos, false};
		}
		return { rookKingMoves, true };
	}

	//Update position 
	Piece* takenPiece = nullptr;
	if (IsCapture(currentPosData, newPos, takenPiece))
	{
		if (!TryUpdatePiecePosition({ movedPiece, currentPos }, newPos))
		{
			return { newPos, false, std::format("Tried to place default board piece: {} at pos: {} "
				"due to capture but failed to update its position {}",
				movedPiece->ToString(), currentPos.ToString(), newPos.ToString()) };
		}
		takenPiece->UpdateState(Piece::State::Captured);
		return { newPos, true };
	}

	if (DoesMoveDeltaMatchPieceMoves(movedPiece->pieceType, currentPos, newPos))
	{
		if (!TryUpdatePiecePosition({ movedPiece, currentPos }, newPos))
		{
			return { newPos, false, std::format("Tried to place default board piece: {} at pos: {} "
				"due to move delta but failed to update its position {}",
				movedPiece->ToString(), currentPos.ToString(), newPos.ToString()) };
		}
		return { newPos, true };
	}
		

	//TODO: add special move checks like castleing, taking, etc.

	return { newPos, false, std::format("Move Delta from {} -> {} does not match any possible moves for {}",
			currentPos.ToString(), newPos.ToString(), ToString(movedPiece->pieceType)) };
} 

std::string CleanInput(const std::string& input)
{
	std::string cleaned = Utils::StringUtil(input).Trim().RemoveChar(' ').ToString();
	return cleaned;
}

std::optional<MoveInfo> TryParseMoveInfoFromMove(const std::string& input)
{
	std::string cleanedInput = CleanInput(input);
	if (cleanedInput == NOTATION_KINGSIDE_CASTLE)
	{
		MoveInfo kingsideCastleInfo = { .PiecesMoved = {PieceType::King, PieceType::Rook}, 
			.SpecialMoveFlags= SpecialMove::KingSideCastle};
		return kingsideCastleInfo;
	}
	else if (cleanedInput == NOTATION_QUEENSIDE_CASTLE)
	{
		MoveInfo queensideCastleInfo = { .PiecesMoved = {PieceType::King, PieceType::Rook},
			.SpecialMoveFlags = SpecialMove::QueenSideCastle };
		return queensideCastleInfo;
	}

	std::optional<PieceType> pieceMoved = TryGetPieceFromNotationSymbol(cleanedInput[0]);
	std::optional<Utils::Position2D> optionalCurrentPosInfo;

	int colStringIndex = 1;
	int rowStringIndex = 2;
	bool isCapture = false;
	//If we have capture character, we have to increase to the 
	//next indices to search for new pos
	if (cleanedInput[1] == NOTATION_CAPTURE_CHAR)
	{
		colStringIndex++;
		rowStringIndex++;
		isCapture = true;
	}
	//If same piece can move to a spot we provide disambigiuoty check
	//and can provide an extra row
	else if (Utils::IsNumber(cleanedInput[colStringIndex]))
	{
		optionalCurrentPosInfo = { std::stod(&cleanedInput[colStringIndex]), NULL_POS,};
		colStringIndex++;
		rowStringIndex++;
	}
	//here is the disambigiuoty check for extra col
	else if (Utils::IsLetter(cleanedInput[colStringIndex]) && Utils::IsLetter(cleanedInput[rowStringIndex]))
	{
		optionalCurrentPosInfo = { NULL_POS, cleanedInput[colStringIndex] - 'a'};
		colStringIndex++;
		rowStringIndex++;
	}
	
	char col = cleanedInput[colStringIndex];
	char row = cleanedInput[rowStringIndex];
	Utils::Position2D pos(std::atoi(&row), col - 'a');

	std::optional<PieceType> promotePiece = std::nullopt;
	if (cleanedInput[rowStringIndex + 1] == NOTATION_PROMOTION_CHAR)
	{
		promotePiece= TryGetPieceFromNotationSymbol(cleanedInput[rowStringIndex+1]);
	}

	bool isCheck = cleanedInput.back() == NOTATION_CHECK_CHAR;
	bool isCheckmate = cleanedInput.back() == NOTATION_CHECKMATE_CHAR;
	
	SpecialMove moveFlags = SpecialMove::None;
	if (promotePiece.has_value()) moveFlags |= SpecialMove::Promotion;
	if (isCapture) moveFlags |= SpecialMove::Capture;

	MoveInfo info =
	{
		pieceMoved != std::nullopt ? std::vector<PieceType>{pieceMoved.value()} : std::vector<PieceType>{},
		optionalCurrentPosInfo,
		pos, 
		cleanedInput,
		moveFlags,
		promotePiece,
		isCheck,
		isCheckmate
	};
	return info;
}

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
			if (piecesMoved == type)
			{
				outFirstMove = &move;
				return true;
			}
		}
	}
	return false;
}