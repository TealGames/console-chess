#include <format>
#include <algorithm>
#include "BoardManager.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "Piece.hpp"
#include "HelperFunctions.hpp"

MoveResult::MoveResult(const Utils::Position2D& pos, const bool isValid, const std::string& info)
	:attemptedPos(pos), isValidMove(isValid), info(info)
{}

static std::array<std::array<Tile, BOARD_DIMENSION>, BOARD_DIMENSION> tiles;
static std::array<Piece*, 2*COLOR_PIECES_COUNT> allPieces;

void CreatePieces(const ColorTheme color, bool overrideExisting)
{
	bool alreadyExist = Utils::IterableHas(allPieces, color);
	if (alreadyExist && !overrideExisting)
	{
		std::string err = std::format("Tried to create pieces for color {} "
			"but no override is allowed!", ToString(color));
		Utils::Log(Utils::LogType::Error, err);
		return;
	}
}

void ResetBoard()
{

}

static std::array<Piece*, COLOR_PIECES_COUNT> GetPiecesForColor(const ColorTheme color)
{
	std::array<Piece*, COLOR_PIECES_COUNT> pieces;
	const auto startIt = color == ColorTheme::Light ? 0 : COLOR_PIECES_COUNT;
	const auto endIt = startIt + COLOR_PIECES_COUNT;
	std::copy(startIt, endIt, std::back_inserter(pieces));
	return pieces;
}

MoveResult TryMove(const Piece& movedPiece, const Utils::Position2D& newPos)
{
	const Utils::Position2D& currentPos = movedPiece.pos;
	if (newPos.x >= BOARD_DIMENSION || newPos.x < 0 || newPos.y >= BOARD_DIMENSION || newPos.y < 0)
		return { newPos, false, std::format("Position ({}) is out of bounds", newPos.ToString()) };

	if (movedPiece.DoesMoveDeltaMatchPieceMoves(newPos))
	{
		const std::string info = std::format("Position ({}) does not follow {}'s moves",
			newPos.ToString(), ToString(movedPiece.pieceType));
		return { newPos, false,  info};
	}
		
	for (const Piece* colorPiece : allPieces)
	{
		//We can move to other color pieces to capture, but not to same side
		if (colorPiece==nullptr || colorPiece->color != movedPiece.color) continue;

		if (colorPiece->pos == newPos)
		{
			std::string message = std::format("Position ({}) contains other piece {}",
				newPos.ToString(), colorPiece->ToString());
			return { newPos, false, message };
		}
	}

	return { newPos, true };
}