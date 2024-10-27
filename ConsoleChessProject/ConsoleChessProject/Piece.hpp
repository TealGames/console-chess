#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include "Vector2D.hpp"
#include "Color.hpp"
#include "Position2D.hpp"

enum class PieceType
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

class Piece
{
private:
	struct PieceMoveInfo
	{
		double value;
		const std::vector<Utils::Vector2D>& moveDirs;
		const std::vector<Utils::Vector2D>& captureDirs;
	};

	const std::vector<Utils::Vector2D>& _moveDirs;
	const std::vector<Utils::Vector2D>& _captureDirs;
	//Note: infinity means any value
	/*static const std::unordered_map<PieceType, PieceMoveInfo> pieceMoves;*/

	Utils::Position2D _pos;
	bool _isCaptured;

	bool HasDifferentCaptureMove();
	/*static bool HasPieceTypeDefined(const PieceType);
	static bool DoesMoveDeltaMatchPieceMoves(const PieceType,
		const Utils::Position2D&, const Utils::Position2D&);*/

public:
	const ColorTheme color;
	const Utils::Position2D& pos;
	const PieceType pieceType;

	Piece(const ColorTheme, const PieceType, const Utils::Position2D);

	void SetCaptured(bool isCaptured);
	void SetPos(const Utils::Position2D newPos);
	bool DoesMoveDeltaMatchPieceMoves(const Utils::Position2D& newPos) const;
	std::string ToString() const;
};

double GetValueForPiece(const PieceType piece);
std::vector<Utils::Vector2D> GetMoveDirsForPiece(const PieceType piece);
std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const PieceType piece);

std::string ToString(const PieceType&);