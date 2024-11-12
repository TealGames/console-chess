#include <format>
#include <string>
#include <vector>
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

static const std::unordered_map<ColorTheme, std::string> COLOR_PROPERTY_NAMES =
{ {ColorTheme::Light, "LightPieces"}, {ColorTheme::Dark, "DarkPieces"}};
static const std::string DEFAULT_BOARD_PROPERTY = "Default";
static const char EMPTY_IDENTIFIER = '-';

static std::optional<json> boardsJSON;
static std::unordered_map<ColorTheme, std::string> storedBoards;

static std::string GetBoardSpacesFrom(const json& fullBoard, const ColorTheme& color)
{
	auto darkBoard = fullBoard[COLOR_PROPERTY_NAMES.at(ColorTheme::Dark)];
	std::vector<std::string> boardSpaces = darkBoard.get<std::vector<std::string>>();
	return Utils::CollapseToSingleString(boardSpaces);
}

std::string GetDefaultBoardJSON(const ColorTheme& color)
{
	if (boardsJSON == std::nullopt)
	{
		std::ifstream fstream(BOARDS_PATH);
		boardsJSON = json::parse(fstream);
	}

	if (storedBoards.find(color) == storedBoards.end())
	{
		json defaultBoardProperty = boardsJSON.value()[DEFAULT_BOARD_PROPERTY];
		std::string boardSpacesAsString;
		if (color == ColorTheme::Dark)
			boardSpacesAsString = GetBoardSpacesFrom(defaultBoardProperty, ColorTheme::Dark);
		else if (color == ColorTheme::Light)
			boardSpacesAsString = GetBoardSpacesFrom(defaultBoardProperty, ColorTheme::Light);
		else
		{
			std::string err = std::format("Tried to get default board for undefined color {}", ToString(color));
			Utils::Log(Utils::LogType::Error, err);
			return "";
		}
		storedBoards.insert({ color, boardSpacesAsString });
		return boardSpacesAsString;
	}
	return storedBoards.at(color);
}

std::vector<Utils::Point2DInt> GetPositionsForPieces(const ColorTheme& color, const std::vector<const Piece*>& pieces)
{
	std::vector<const Piece*> noPositionSetPieces = pieces;
	std::vector<Utils::Point2DInt> positions;
	positions.reserve(noPositionSetPieces.size());
	int spaceIndex = -1;

	//Have to iterator through spaces and not pieces since there are multiple of the same piece
	//so instead we have to retrieve any we have left
	for (const auto& spaceSymbol : GetDefaultBoardJSON(color))
	{
		spaceIndex++;
		if (spaceSymbol == EMPTY_IDENTIFIER) continue;
		std::optional<PieceType> spacePieceType = TryGetPieceFromNotationSymbol(spaceSymbol);

		//TODO: maybe log error or warning?
		if (!spacePieceType.has_value()) continue;
		
		std::vector<const Piece*>::const_iterator foundPieceWithTypeIt = std::find_if(noPositionSetPieces.begin(), noPositionSetPieces.end(),
			[&spacePieceType](const Piece* piece) -> bool 
			{
				return piece->pieceType == spacePieceType.value();
			});

		//We erase to make sure if there are pieces of the same type are 
		//removed to not check it again
		if (foundPieceWithTypeIt != noPositionSetPieces.end())
		{
			//We get index of the piece we found from copied list and then check its index in the unchanged list
			//which should work since we do only check values that should not get removed from list prior
			size_t indexOfFoundPiece = Utils::GetIndexOfValue(pieces, *foundPieceWithTypeIt);
			Utils::Point2DInt piecePos = { spaceIndex / BOARD_DIMENSION, spaceIndex % BOARD_DIMENSION};

			positions.insert(positions.begin()+indexOfFoundPiece, piecePos);
			noPositionSetPieces.erase(foundPieceWithTypeIt);
		}
	}
}