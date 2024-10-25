#include <format>
#include "BoardManager.hpp"
#include "Position2D.hpp"
#include "Vector2D.hpp"
#include "Piece.hpp"
#include "HelperFunctions.hpp"

MoveResult::MoveResult(Utils::Position2D pos, bool isValid, std::string info = "")
	:attemptedPos(pos), isValidMove(isValid), info(info)
{}

std::array<std::array<Tile, boardDimension>, boardDimension> tiles;
std::unordered_map<ColorTheme, std::array<Piece&, colorPieces>> allPieces;

void CreatePieces(const ColorTheme color, bool overrideExisting)
{
	bool alreadyExist = Utils::IterableHas(allPieces, color);
	if (alreadyExist && !overrideExisting)
	{
		std::string err = std::format("Tried to create pieces for color {} but no override is allowed!", color);
		Utils::Log(Utils::LogType::Error, err);
		return;
	}
}

void ResetBoard()
{

}

MoveResult TryMove(const Piece& movedPiece, const Utils::Position2D& newPos)
{
	const Utils::Position2D& currentPos = movedPiece.pos;
	if (newPos.x >= boardDimension || newPos.x < 0 || newPos.y >= boardDimension || newPos.y < 0)
		return { newPos, false, std::format("Position ({}) is out of bounds", newPos) };

	if (movedPiece.DoesMoveDeltaMatchPieceMoves(newPos))
		return { newPos, false, std::format("Position ({}) does not follow {}'s moves", movedPiece.pieceType) };

	for (const auto& colorPieces : allPieces)
	{
		//We can move to other color pieces to capture, but not to same side
		if (colorPieces.first != movedPiece.color) continue;

		for (const auto& piece : colorPieces.second)
		{
			if (piece.pos == newPos)
			{
				std::string message = std::format("Position ({}) contains other piece {}",
					newPos, piece.ToString());
				return { newPos, false, message };
			}
		}
	}

	return { newPos, true };
}