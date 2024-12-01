#pragma once
#include "Piece.hpp"
#include "Color.hpp"

class Tile
{
private:
	const Piece* _currentPiece;

public:
	const Piece* const currentPiece;
	const ArmyColor color;
	bool HasPiece();

	Tile();
	Tile(const ArmyColor);
	Tile(const ArmyColor, const Piece&);

	void UpdatePiece(const Piece*);
	bool TryRemovePiece();
};