#include <format>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include "HelperFunctions.hpp"
#include "Piece.hpp"
#include "Color.hpp"
#include "Point2DInt.hpp"
#include "json.hpp"
#include "BoardSetup.hpp"
#include "Globals.hpp"

using json = nlohmann::json;

static const std::unordered_map<ColorTheme, char> COLOR_SYMBOLS =
{ {ColorTheme::Light, 'L'}, {ColorTheme::Dark, 'D'}};
static const std::unordered_map<BoardType, std::string> BOARD_TYPE_PROPERTIES =
{ {BoardType::Default, "Default" } };
static const char EMPTY_IDENTIFIER = '-';

static std::optional<json> boardsJSON;
static std::unordered_map<BoardType, std::vector<std::string>> storedBoards;

static std::optional<ColorTheme> TryGetColorFromNotation(const char& notation)
{
	for (const auto& colorChar : COLOR_SYMBOLS)
	{
		if (colorChar.second == notation) return colorChar.first;
	}
	return std::nullopt;
}

static std::vector<std::string> GetBoardSpacesFrom(const json& fullBoard, const BoardType& type)
{
	auto propertyNameIt = BOARD_TYPE_PROPERTIES.find(type);
	if (propertyNameIt == BOARD_TYPE_PROPERTIES.end())
	{
		std::string err = "Tried to get board spaces for "
			"type with no property defined";
		Utils::Log(Utils::LogType::Error, err);
		return {};
	}

	json board = fullBoard[propertyNameIt->second];
	std::vector<std::string> boardSpaces = board.get<std::vector<std::string>>();
	return boardSpaces;
}

std::vector<std::string> GetBoardJSON(const BoardType& boardType)
{
	if (boardsJSON == std::nullopt)
	{
		std::ifstream fstream(BOARDS_PATH);
		boardsJSON = json::parse(fstream);
	}

	if (storedBoards.find(boardType) == storedBoards.end())
	{
		std::vector<std::string> boardCells = GetBoardSpacesFrom(boardsJSON.value(), boardType);
		if (boardCells.empty()) return {};
		
		storedBoards.insert({ boardType, boardCells });
		return boardCells;
	}
	return storedBoards.at(boardType);
}

std::vector<InitPiecePosition> GetDefaultBoardPiecePositions()
{
	//std::vector<const Piece*> noPositionSetPieces = pieces;
	std::vector<InitPiecePosition> positions;
	//positions.reserve(noPositionSetPieces.size());
	int spaceIndex = -1;
	char currentColorChar = '0';
	char currentPieceChar = '0';

	//Have to iterator through spaces and not pieces since there are multiple of the same piece
	//so instead we have to retrieve any we have left
	for (const auto& cellSymbol : GetBoardJSON(BoardType::Default))
	{
		spaceIndex++;
		if (cellSymbol.size()>0 && cellSymbol[0] == EMPTY_IDENTIFIER) continue;

		if (cellSymbol.size() <= 1)
		{
			std::string err = std::format("Tried to create data for cell symbol: {} for default board positions"
				"but the size does not have enough space for all data. Size: {}", cellSymbol, cellSymbol.size());
			Utils::Log(Utils::LogType::Error, err);
			return {};
		}

		currentColorChar = cellSymbol[0];
		currentPieceChar = cellSymbol[1];
		std::optional<ColorTheme> maybeColor = TryGetColorFromNotation(currentColorChar);
		if (!maybeColor.has_value())
		{
			std::string err = std::format("Tried to get default board positions"
				"but could not parse the color type char: {}", currentColorChar);
			Utils::Log(Utils::LogType::Error, err);
			return {};
		}

		std::optional<PieceType> maybePieceType = TryGetPieceFromNotationSymbol(currentPieceChar);
		if (!maybePieceType.has_value())
		{
			std::string err = std::format("Tried to get default board positions"
				"but could not parse the piece type char: {}", currentPieceChar);
			Utils::Log(Utils::LogType::Error, err);
			return {};
		}
		
		
		//std::vector<const Piece*>::const_iterator foundPieceWithTypeIt = std::find_if(noPositionSetPieces.begin(), noPositionSetPieces.end(),
		//	[&spacePieceType](const Piece* piece) -> bool 
		//	{
		//		return piece->pieceType == spacePieceType.value();
		//	});

		Utils::Point2DInt piecePos = { spaceIndex / BOARD_DIMENSION, spaceIndex % BOARD_DIMENSION };
		positions.emplace_back(InitPiecePosition{ maybePieceType.value(), maybeColor.value(), piecePos});

		//We erase to make sure if there are pieces of the same type are 
		//removed to not check it again
		//if (foundPieceWithTypeIt != noPositionSetPieces.end())
		//{
		//	//We get index of the piece we found from copied list and then check its index in the unchanged list
		//	//which should work since we do only check values that should not get removed from list prior
		//	size_t indexOfFoundPiece = Utils::GetIndexOfValue(pieces, *foundPieceWithTypeIt);
		//	Utils::Point2DInt piecePos = { spaceIndex / BOARD_DIMENSION, spaceIndex % BOARD_DIMENSION};

		//	positions.insert(positions.begin()+indexOfFoundPiece, piecePos);
		//	noPositionSetPieces.erase(foundPieceWithTypeIt);
		//}
	}
	return positions;
}