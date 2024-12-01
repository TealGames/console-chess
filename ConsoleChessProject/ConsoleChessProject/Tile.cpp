#include "Piece.hpp"
#include "Tile.hpp"
#include "Color.hpp"

Tile::Tile() : color(ArmyColor::Light), _currentPiece(nullptr), currentPiece(_currentPiece) {}
Tile::Tile(ArmyColor color) : color(color), _currentPiece(nullptr), currentPiece(_currentPiece) {}
Tile::Tile(ArmyColor color, const Piece& piece) :
	color(color), _currentPiece(&piece), currentPiece(_currentPiece) {}

void Tile::UpdatePiece(const Piece* newPiece)
{
	_currentPiece = newPiece;
}

bool Tile::HasPiece()
{
	return _currentPiece != nullptr;
}

/// <summary>
/// Returns true if it was successfully removed and false if it did not have any piece
/// </summary>
/// <returns></returns>
bool Tile::TryRemovePiece()
{
	if (HasPiece())
	{
		_currentPiece = nullptr;
		return true;
	}
	else
	{
		return false;
	}
}