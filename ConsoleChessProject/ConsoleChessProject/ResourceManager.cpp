#include <wx/wx.h>
#include <string>
#include <filesystem>
#include "ResourceManager.hpp"
#include "Piece.hpp"
#include "HelperFunctions.hpp"

const std::string imageAbsPath = "C/Users/lukas/Desktop/Projects/C & C++ Programs/C++/console - chess/ConsoleChessProject/ConsoleChessProject/";
static const std::unordered_map<PieceType, std::filesystem::path> pieceImagePaths =
{
	{PieceType::Queen, "../Resources/chess_piece_queen.png"},
};

bool TryLoad(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage)
{
	bool result = false;
	if (outImage != nullptr) result = outImage->LoadFile(resourcePath.string(), resourceType);
	else
	{
		wxImage image;
		result= image.LoadFile(resourcePath.string(), resourceType);
	}
	
	if (!result)
	{
		std::string err = std::format("Tried to load resource at path {} but failed", resourcePath.string());
		Utils::Log(Utils::LogType::Error, err);
		wxLogError(err.c_str());
	}
	return result;
}

bool TryLoadPieceImage(const PieceType& piece, wxImage* outImage)
{
	auto foundPiecePath = pieceImagePaths.find(piece);
	if (foundPiecePath != pieceImagePaths.end())
		return TryLoad(foundPiecePath->second, wxBITMAP_TYPE_PNG, outImage);
	return false;
}

bool TryLoadAll()
{
	for (const auto& pieceImagePath : pieceImagePaths)
	{
		if (!TryLoadPieceImage(pieceImagePath.first))
		{
			std::string err = std::format("Tried to load piece {} failed", ToString(pieceImagePath.first));
			Utils::Log(Utils::LogType::Error, err);
			wxLogError(err.c_str());
			return false;
		}
	}
}
