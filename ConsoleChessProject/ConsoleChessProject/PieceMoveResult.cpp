#include <vector>
#include <string>
#include "PieceMoveResult.hpp"
#include "Point2D.hpp"

PieceMoveResult::PieceMoveResult(const Utils::Point2DInt& pos, const bool isValid, const std::string& info)
	:AttemptedPositions({ pos }), IsValidMove(isValid), Info(info) {}

PieceMoveResult::PieceMoveResult(const std::vector<Utils::Point2DInt>& positions, const bool isValid, const std::string& info)
	:AttemptedPositions(positions), IsValidMove(isValid), Info(info) {}
