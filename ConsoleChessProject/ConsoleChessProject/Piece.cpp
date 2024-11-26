#include <format>
#include <unordered_map>
#include "Piece.hpp"
#include "Color.hpp"
#include "Point2DInt.hpp"
#include "Vector2D.hpp"
#include "HelperFunctions.hpp"
#include "FrozenMap.hpp"
#include "Globals.hpp"

static constexpr double INF = std::numeric_limits<double>::infinity();

//Note: if capture dirs are not defined for a piece they are assumed the default move dirs
//and if any capture dirs are provided the move dirs are NOT included (and must be explicitly defined then)

//NOTE: POSITIONS AND CAPTURE IS (ROW, COL) so it would be (Y, X)
// row positions UP DECREASE (since row index decreeases) and row positions DOWN INCREASE
static const std::unordered_map<PieceType, PieceStaticInfo> PIECE_INFO(
	std::unordered_map<PieceType, PieceStaticInfo> {
		{PieceType::Pawn, { 1, 'P', false, {{-1,0}}, {{-1, -1}, {-1, 1}} }},
		{PieceType::Knight, {3, 'N', true, {{2, 1}, {1, 2}, {2, -1}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {-1, 2}}, {}}},
		{PieceType::Bishop, {3, 'B', false, {{INF, INF}, {INF, -INF}, {-INF, INF}, {-INF, -INF}}, {}}},
		{PieceType::Rook, {5, 'R', false, {{0, INF}, {INF, 0}, {0, -INF}, {-INF, 0}}, {}}},

		{PieceType::Queen, {9, 'Q', false, {{INF, INF}, {INF, -INF}, {-INF, INF}, {-INF, -INF},
								 {0, INF}, {INF, 0}, {0, -INF}, {-INF, 0}}, {}}},

		{PieceType::King, {INF, 'K', false, {{1,0}, {0, 1}, {-1, 0}, {0, -1}}, {}}}
	});

std::string PieceTypeInfo::ToString() const
{
	std::string displayStr = std::format("[{}, {}]", ::ToString(Color), ::ToString(PieceType));
	return displayStr;
}

bool PieceTypeInfo::operator==(const PieceTypeInfo& other) const
{
	return Color == other.Color && PieceType == other.PieceType;
}

Piece::Piece()
	: m_Color(ColorTheme::Light), m_PieceType(PieceType::Pawn), m_moveDirs(GetMoveDirsForPiece(m_PieceType)),
	m_captureDirs(GetCaptureMovesForPiece(m_PieceType)), m_state(Piece::State::Undefined), m_State(m_state) {}

Piece::Piece(const ColorTheme color, const PieceType piece)
	: m_Color(color), m_PieceType(piece), m_moveDirs(GetMoveDirsForPiece(piece)),
	m_captureDirs(GetCaptureMovesForPiece(piece)), m_state(Piece::State::Undefined), m_State(m_state) {}

Piece::Piece(const Piece& copy) 
	: m_Color(copy.m_Color), m_PieceType(copy.m_PieceType), m_moveDirs(GetMoveDirsForPiece(copy.m_PieceType)),
	m_captureDirs(GetCaptureMovesForPiece(copy.m_PieceType)), m_state(Piece::State::Undefined), m_State(m_state) {}

bool Piece::operator==(const Piece& piece) const
{
	return m_Color == piece.m_Color && m_PieceType == piece.m_PieceType && m_moveDirs == piece.m_moveDirs &&
		m_captureDirs == piece.m_captureDirs && m_State == piece.m_State;
}

bool Piece::HasDifferentCaptureMove()
{
	return m_captureDirs.size() > 0;
}

bool HasPieceTypeDefined(const PieceType type)
{
	return PIECE_INFO.find(type) != PIECE_INFO.end();
}

void Piece::UpdateState(const State& state)
{
	m_state = state;
}

double GetValueForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get value for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return 0;
	}
	return PIECE_INFO.at(type).ScoreValue;
}

bool CanPieceMoveOverPieces(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get bool if piece type {} "
			"can move over other peices but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return 0;
	}
	return PIECE_INFO.at(type).CanMoveOverPieces;
}

const std::vector<Utils::Vector2D> GetMoveDirsForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	std::vector<Utils::Vector2D> allMoveDirs;

	//NOTE: endX must be one past limit to ensure at least one input for elements that 
	//are not infinity or neg infinity
	int currentX = 0, endX = 0, currentY = 0, endY = 0;
	for (const auto& moveDir : PIECE_INFO.at(type).MoveDirs)
	{
		if (std::isinf(moveDir.x) && std::isinf(moveDir.y))
		{
			currentX = Utils::IsPosInifinity(moveDir.x) ? 1 : -1;
			endX = Utils::IsPosInifinity(moveDir.x) ? BOARD_DIMENSION : -BOARD_DIMENSION;
			currentY = Utils::IsPosInifinity(moveDir.y) ? 1 : -1;
			endY = Utils::IsPosInifinity(moveDir.y) ? BOARD_DIMENSION : -BOARD_DIMENSION;

			while (std::abs(currentX) < std::abs(endX) && std::abs(currentY) < std::abs(endY))
			{
				allMoveDirs.emplace_back(currentX, currentY);
				auto vec = Utils::Vector2D(currentX, currentY);
				Utils::Log(std::format("ADDING INF X: {} Y: {} POS: {}", std::to_string(Utils::IsPosInifinity(moveDir.x)),
					std::to_string(Utils::IsPosInifinity(moveDir.y)),  vec.ToString(Utils::Vector2D::VectorForm::Component)));
				currentX += endX > 0 ? 1 : -1;
				currentY += endY > 0 ? 1 : -1;
			}
		}
		else if (std::isinf(moveDir.x))
		{
			currentX = Utils::IsPosInifinity(moveDir.x) ? 1 : -1;
			endX = Utils::IsPosInifinity(moveDir.x) ? BOARD_DIMENSION : -BOARD_DIMENSION;

			while (std::abs(currentX) < std::abs(endX))
			{
				allMoveDirs.emplace_back(currentX, moveDir.y);
				currentX += endX > 0 ? 1 : -1;
			}
		}
		else if (std::isinf(moveDir.y))
		{
			currentY = Utils::IsPosInifinity(moveDir.y) ? 1 : -1;
			endY = Utils::IsPosInifinity(moveDir.y) ? BOARD_DIMENSION : -BOARD_DIMENSION;

			while (std::abs(currentY) < std::abs(endY))
			{
				allMoveDirs.emplace_back(moveDir.x, currentY);
				currentY += endY > 0 ? 1 : -1;
			}
		}
		else
		{
			allMoveDirs.emplace_back(moveDir.x, moveDir.y);
			
		}
	}

	std::string allVecsStr;
	int count = 0;
	for (const auto& element : allMoveDirs)
	{
		allVecsStr += element.ToString(Utils::Vector2D::VectorForm::Component);
		if (count == 8)
		{
			//Utils::Log(std::format("All Move dirs for piece {} is {}", ToString(type), allVecsStr));
			allVecsStr = "";
			count = 0;
		}
		else count++;
	}
	
	return allMoveDirs;
}

const std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get capture moves for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	const PieceStaticInfo& infoForType = PIECE_INFO.at(type);
	if (infoForType.CaptureDirs.empty()) return infoForType.MoveDirs;
	else return infoForType.CaptureDirs;
}

char GetNotationSymbolForPiece(const PieceType type)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get piece moves for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}
	return PIECE_INFO.at(type).MoveNotationCharacter;
}

const std::optional<PieceType> TryGetPieceFromNotationSymbol(const char& notation)
{
	for (const auto& pieceInfo : PIECE_INFO)
	{
		if (pieceInfo.second.MoveNotationCharacter == notation) 
			return pieceInfo.first;
	}
	return std::nullopt;
}

bool DoesMoveDeltaMatchPieceMoves(const PieceType type,
	const Utils::Point2DInt& startPos, const Utils::Point2DInt& endPos)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get move dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	const Utils::Vector2D delta = GetVector(startPos, endPos);
	if (delta == Utils::Vector2D::ZERO) return false;

	for (const auto& moveDir : PIECE_INFO.at(type).MoveDirs)
	{
		if (delta == moveDir) return true;

		bool xPosInifinity = Utils::IsPosInifinity(moveDir.x);
		bool xNegInifinity = Utils::IsNegInifinity(moveDir.x);
		bool yPosInfinity = Utils::IsPosInifinity(moveDir.y);
		bool yNegInfinity = Utils::IsNegInifinity(moveDir.y);

		//Note: since indices ascend downward for row, the delta's will be flipped from what expected
		bool hasCorrectX = (xPosInifinity && delta.x < 0) || (xNegInifinity && delta.x > 0) || (delta.x == moveDir.x);
		bool hasCorrectY = (yPosInfinity && delta.y > 0) || (yNegInfinity && delta.y < 0) || (delta.y == moveDir.y);
		if (hasCorrectX && hasCorrectY) return true;
	}
}

bool DoesMoveDeltaMatchCaptureMoves(const PieceType type,
	const Utils::Point2DInt& startPos, const Utils::Point2DInt& endPos)
{
	bool hasType = HasPieceTypeDefined(type);
	if (!hasType)
	{
		std::string err = std::format("Tried to get capture dirs for a piece of type {} "
			"but type has no defined info", ToString(type));
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	const Utils::Vector2D delta = GetVector(startPos, endPos);
	if (delta == Utils::Vector2D::ZERO) return false;


	for (const auto& moveDir : PIECE_INFO.at(type).MoveDirs)
	{
		if (delta == moveDir) return true;
	}
	return false;
}

std::string Piece::ToString(bool shorten) const
{
	std::string colorStr = ::ToString(m_Color);
	std::string pieceStr = ::ToString(m_PieceType);
	if (shorten)
	{
		colorStr = *colorStr.begin();
		pieceStr = *pieceStr.begin();
	}
	std::string str = std::format("[{} {}]", colorStr, pieceStr);
	return str;
}

std::string ToString(const PieceType& piece)
{
	switch (piece)
	{
	case PieceType::Pawn:
		return "Pawn";
	case PieceType::Knight:
		return "Knight";
	case PieceType::Bishop:
		return "Bishop";
	case PieceType::Rook:
		return "Rook";
	case PieceType::Queen:
		return "Queen";
	case PieceType::King:
		return "King";
	default:
		return "NULL";
	}
	return "";
}