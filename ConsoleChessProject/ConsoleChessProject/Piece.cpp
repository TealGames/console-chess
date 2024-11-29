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

//Since we do not want to duplicate the same piece movements but with flipped signs or something
//we instead transform them with a multiplier for x and y. REMEMBER: {x= rows, y= cols}
static const std::unordered_map<ColorTheme, Utils::Vector2D> COLOR_MOVE_CAPTURE_MULTUPLIERS = { {ColorTheme::Dark, Utils::Vector2D{-1, 1}} };

std::string PieceTypeInfo::ToString() const
{
	std::string displayStr = std::format("[{}, {}]", ::ToString(Color), ::ToString(PieceType));
	return displayStr;
}

bool PieceTypeInfo::operator==(const PieceTypeInfo& other) const
{
	return Color == other.Color && PieceType == other.PieceType;
}

PieceTypeInfo& PieceTypeInfo::operator=(const PieceTypeInfo& info)
{
	if (this == &info) return *this;

	Color = info.Color;
	PieceType = info.PieceType;
	return *this;
}

Piece::Piece()
	: m_Color(ColorTheme::Light), m_PieceType(PieceType::Pawn), m_moveDirs(GetMoveDirsForPiece(m_Color, m_PieceType)),
	m_captureDirs(GetCaptureMovesForPiece(m_Color, m_PieceType)), m_state(Piece::State::Undefined), m_State(m_state) {}

Piece::Piece(const ColorTheme color, const PieceType piece)
	: m_Color(color), m_PieceType(piece), m_moveDirs(GetMoveDirsForPiece(m_Color, piece)),
	m_captureDirs(GetCaptureMovesForPiece(m_Color, piece)), m_state(Piece::State::Undefined), m_State(m_state) {}

Piece::Piece(const Piece& copy) 
	: m_Color(copy.m_Color), m_PieceType(copy.m_PieceType), m_moveDirs(GetMoveDirsForPiece(m_Color, copy.m_PieceType)),
	m_captureDirs(GetCaptureMovesForPiece(m_Color, copy.m_PieceType)), m_state(Piece::State::Undefined), m_State(m_state) {}

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

const std::vector<Utils::Vector2D> GetMoveDirsForPiece(const ColorTheme color, const PieceType type)
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
	auto maybeMultiplierIt = COLOR_MOVE_CAPTURE_MULTUPLIERS.find(color);
	Utils::Vector2D moveMultiplier = (maybeMultiplierIt != COLOR_MOVE_CAPTURE_MULTUPLIERS.end())? 
										maybeMultiplierIt->second : Utils::Vector2D{1, 1};

	//NOTE: endX must be one past limit to ensure at least one input for elements that 
	//are not infinity or neg infinity
	int currentX = 0, endX = 0, currentY = 0, endY = 0;
	for (const auto& moveDir : PIECE_INFO.at(type).MoveDirs)
	{
		if (std::isinf(moveDir.m_X) && std::isinf(moveDir.m_Y))
		{
			currentX = Utils::IsPosInifinity(moveDir.m_X) ? 1 : -1;
			endX = Utils::IsPosInifinity(moveDir.m_X) ? BOARD_DIMENSION : -BOARD_DIMENSION;
			currentY = Utils::IsPosInifinity(moveDir.m_Y) ? 1 : -1;
			endY = Utils::IsPosInifinity(moveDir.m_Y) ? BOARD_DIMENSION : -BOARD_DIMENSION;

			while (std::abs(currentX) < std::abs(endX) && std::abs(currentY) < std::abs(endY))
			{
				allMoveDirs.emplace_back(currentX* moveMultiplier.m_X, currentY* moveMultiplier.m_Y);
				auto vec = Utils::Vector2D(currentX, currentY);
				Utils::Log(std::format("ADDING INF X: {} Y: {} POS: {}", std::to_string(Utils::IsPosInifinity(moveDir.m_X)),
					std::to_string(Utils::IsPosInifinity(moveDir.m_Y)),  vec.ToString(Utils::Vector2D::VectorForm::Component)));
				currentX += endX > 0 ? 1 : -1;
				currentY += endY > 0 ? 1 : -1;
			}
		}
		else if (std::isinf(moveDir.m_X))
		{
			currentX = Utils::IsPosInifinity(moveDir.m_X) ? 1 : -1;
			endX = Utils::IsPosInifinity(moveDir.m_X) ? BOARD_DIMENSION : -BOARD_DIMENSION;

			while (std::abs(currentX) < std::abs(endX))
			{
				allMoveDirs.emplace_back(currentX * moveMultiplier.m_X, moveDir.m_Y * moveMultiplier.m_Y);
				currentX += endX > 0 ? 1 : -1;
			}
		}
		else if (std::isinf(moveDir.m_Y))
		{
			currentY = Utils::IsPosInifinity(moveDir.m_Y) ? 1 : -1;
			endY = Utils::IsPosInifinity(moveDir.m_Y) ? BOARD_DIMENSION : -BOARD_DIMENSION;

			while (std::abs(currentY) < std::abs(endY))
			{
				allMoveDirs.emplace_back(moveDir.m_X * moveMultiplier.m_X, currentY * moveMultiplier.m_Y);
				currentY += endY > 0 ? 1 : -1;
			}
		}
		else
		{
			allMoveDirs.emplace_back(moveDir.m_X * moveMultiplier.m_X, moveDir.m_Y * moveMultiplier.m_Y);
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

const std::vector<Utils::Vector2D> GetCaptureMovesForPiece(const ColorTheme color, const PieceType type)
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
	std::vector<Utils::Vector2D> captureMoves = infoForType.CaptureDirs.empty() ? infoForType.MoveDirs : infoForType.CaptureDirs;

	auto maybeMultiplierIt = COLOR_MOVE_CAPTURE_MULTUPLIERS.find(color);
	if (maybeMultiplierIt != COLOR_MOVE_CAPTURE_MULTUPLIERS.end())
	{
		for (int i = 0; i < captureMoves.size(); i++)
		{
			captureMoves[i] = Utils::Vector2D(captureMoves[i].m_X* maybeMultiplierIt->second.m_X, 
											  captureMoves[i].m_Y * maybeMultiplierIt->second.m_Y);
		}
	}
	return captureMoves;
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

		bool xPosInifinity = Utils::IsPosInifinity(moveDir.m_X);
		bool xNegInifinity = Utils::IsNegInifinity(moveDir.m_X);
		bool yPosInfinity = Utils::IsPosInifinity(moveDir.m_Y);
		bool yNegInfinity = Utils::IsNegInifinity(moveDir.m_Y);

		//Note: since indices ascend downward for row, the delta's will be flipped from what expected
		bool hasCorrectX = (xPosInifinity && delta.m_X < 0) || (xNegInifinity && delta.m_X > 0) || (delta.m_X == moveDir.m_X);
		bool hasCorrectY = (yPosInfinity && delta.m_Y > 0) || (yNegInfinity && delta.m_Y < 0) || (delta.m_Y == moveDir.m_Y);
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
		colorStr = *(colorStr.begin());
		pieceStr = *(pieceStr.begin());
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