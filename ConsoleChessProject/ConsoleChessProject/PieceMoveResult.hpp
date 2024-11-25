#pragma once
#include <vector>
#include <string>
#include "Point2DInt.hpp"

struct PieceMoveResult
{
	const bool IsValidMove;
	const std::vector<Utils::Point2DInt> AttemptedPositions;
	const std::string Info;

	PieceMoveResult(const Utils::Point2DInt&, const bool, const std::string& info = "");
	PieceMoveResult(const std::vector<Utils::Point2DInt>&, const bool, const std::string& info = "");
};

