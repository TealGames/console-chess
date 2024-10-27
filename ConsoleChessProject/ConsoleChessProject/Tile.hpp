#pragma once
#include "Piece.hpp"
#include "Color.hpp";

class Tile
{
private:
	const Piece* _currentPiece;

public:
	const Piece* const currentPiece;
	const ColorTheme color;
	bool HasPiece();

	Tile();
	Tile(const ColorTheme);
	Tile(const ColorTheme, const Piece&);

	void UpdatePiece(const Piece&);
	bool TryRemovePiece();
};