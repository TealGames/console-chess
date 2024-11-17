#include <wx/wx.h>
#include <string>
#include <filesystem>
#include "ResourceManager.hpp"
#include "Piece.hpp"
#include "Point2DInt.hpp"
#include "HelperFunctions.hpp"

//Size/spacing is row, col
const wxSize SPRITE_SIZE(101, 101);
const wxSize SPRITE_SPACING(21, 21);
const std::filesystem::path PIECES_PATH = "chess_pieces.png";
static wxImage* pieceSpriteMap= nullptr;

static const std::unordered_map<PieceTypeInfo, Utils::Point2DInt> PIECE_POSITIONS =
{
	{{ColorTheme::Light, PieceType::Queen},		{0,0}},
	{{ColorTheme::Light, PieceType::King},		{0,1}},
	{{ColorTheme::Light, PieceType::Bishop},	{0,2}},
	{{ColorTheme::Light, PieceType::Knight},	{0,3}},
	{{ColorTheme::Light, PieceType::Rook},		{0,4}},
	{{ColorTheme::Light, PieceType::Pawn},		{0,5}},
	{{ColorTheme::Dark,  PieceType::Queen},		{1,0}},
	{{ColorTheme::Dark,  PieceType::King},		{1,1}},
	{{ColorTheme::Dark,  PieceType::Bishop},	{1,2}},
	{{ColorTheme::Dark,  PieceType::Knight},	{1,3}},
	{{ColorTheme::Dark,  PieceType::Rook},		{1,4}},
	{{ColorTheme::Dark,  PieceType::Pawn},		{1,5}},
};

//Updates and adds an image handler if it is a new handler
static bool HasValidImageHandler(const wxBitmapType& resourceType)
{
	if (wxImage::FindHandler(resourceType)) return true;
	wxInitAllImageHandlers();

	if (!wxImage::FindHandler(resourceType))
	{
		const std::string error = std::format("Tried to load images of a type that does "
			"not have a bitmap image type handler");
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}
	return true;
}

bool TryLoad(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage)
{
	//Checks to make sure the image type has a handler
	if (!HasValidImageHandler(resourceType)) return false;

	bool result = false;
	const std::string fullpath = resourcePath.string();
	if (outImage != nullptr) result = outImage->LoadFile(fullpath, resourceType);
	else
	{
		outImage = new wxImage();
		result= outImage->LoadFile(fullpath, resourceType);
	}
	
	if (!result)
	{
		std::string err = std::format("Tried to load resource at path {} but failed", resourcePath.string());
		Utils::Log(Utils::LogType::Error, err);
		wxLogError(err.c_str());
	}
	std::string mess = fullpath + "Has result" + std::to_string(result) + std::to_string(outImage!=nullptr);
	wxLogMessage(mess.c_str());
	return result;
}

bool TryLoadAllPieceImages(std::unordered_map<PieceTypeInfo, wxImage>* outImages)
{
	if (outImages != nullptr) outImages->clear();

	if (pieceSpriteMap == nullptr)
	{
		pieceSpriteMap = new wxImage();
		if (!TryLoad(PIECES_PATH, wxBITMAP_TYPE_PNG, pieceSpriteMap)) return false;
	}

	wxImage currentSubImage;
	wxPoint tileMapPos;
	int r = 0;
	int c = 0;
	bool foundAtLeastOnePiece = false;
	for (const auto& piecePos : PIECE_POSITIONS)
	{
		foundAtLeastOnePiece = true;
		if (outImages != nullptr)
		{
			r = piecePos.second.x;
			c = piecePos.second.y;
			tileMapPos = { c * SPRITE_SIZE.x + c * SPRITE_SPACING.x, r * SPRITE_SIZE.y + r * SPRITE_SPACING.y };

			/*std::string message = std::format("Piece {} Pos: {} {}", piecePos.first.ToString(), 
				std::to_string(tileMapPos.x), std::to_string(tileMapPos.y));
			wxLogMessage(message.c_str());*/
			outImages->emplace(piecePos.first, GetSubImage(*pieceSpriteMap, tileMapPos, SPRITE_SIZE));
		}
		else break;
	}
	return foundAtLeastOnePiece;
}

void Resize(wxImage& resourceImage, const wxSize& newSize)
{
	resourceImage.Rescale(newSize.x, newSize.y, wxIMAGE_QUALITY_HIGH);
}

void ResizePreserveAspect(wxImage& resourceImage, const wxSize& newSize, const wxPoint& point)
{
	resourceImage.Resize(newSize, point);
}

wxImage GetSubImage(const wxImage& resourceImage, const wxPoint& position, const wxSize& size)
{
	return resourceImage.GetSubImage(wxRect(position.x, position.y, size.x, size.y));
}
