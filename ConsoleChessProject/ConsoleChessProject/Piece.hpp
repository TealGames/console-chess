#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include "Vector2D.hpp"
#include "Color.hpp"
#include "Position2D.hpp"

class Piece
{
public:
	enum class PieceType
	{
		Pawn,
		Knight,
		Bishop,
		Rook,
		Queen,
		King
	};

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
	static const std::unordered_map<PieceType, PieceMoveInfo> pieceMoves;

	Utils::Position2D _pos;
	bool _isCaptured;

	bool HasDifferentCaptureMove();
	static bool HasPieceTypeDefined(const PieceType);
	static bool DoesMoveDeltaMatchPieceMoves(const PieceType,
		const Utils::Position2D&, const Utils::Position2D&);

public:
	const ColorTheme color;
	const Utils::Position2D& pos;
	const PieceType pieceType;

	Piece(const ColorTheme, const PieceType, const Utils::Position2D);

	void SetCaptured(bool);
	void SetPos(const Utils::Position2D);
	bool DoesMoveDeltaMatchPieceMoves(const Utils::Position2D&) const;
	std::string ToString() const;

	static double GetValueForPiece(const PieceType);
	static std::vector<Utils::Vector2D> GetMoveDirsForPiece(const PieceType);
	static std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const PieceType);
};