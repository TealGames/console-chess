#include <wx/wx.h>
#include <wx/sound.h>
#include <mmsystem.h>
#include <Windows.h>
#include <format>
#include <string>
#include <iostream>
#include <optional>
#include <filesystem>
#include <unordered_map>
#include "ResourceManager.hpp"
#include "Piece.hpp"
#include "Point2DInt.hpp"
#include "HelperFunctions.hpp"

#pragma comment(lib, "winmm.lib")

//Size/spacing is row, col
const wxSize SPRITE_SIZE(101, 101);

static SpriteMap<PieceTypeInfo> PieceSprites
{
	"chess_pieces.png", nullptr, wxSize(21, 21),
	std::unordered_map<PieceTypeInfo, SubSpriteData>{
	{{ArmyColor::Light, PieceType::King},		{{0,0}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Light, PieceType::Queen},		{{0,1}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Light, PieceType::Bishop},	{{0,2}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Light, PieceType::Knight},	{{0,3}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Light, PieceType::Rook},		{{0,4}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Light, PieceType::Pawn},		{{0,5}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Dark,  PieceType::King},		{{1,0}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Dark,  PieceType::Queen},		{{1,1}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Dark,  PieceType::Bishop},	{{1,2}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Dark,  PieceType::Knight},	{{1,3}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Dark,  PieceType::Rook},		{{1,4}, SPRITE_SIZE, std::nullopt}},
	{{ArmyColor::Dark,  PieceType::Pawn},		{{1,5}, SPRITE_SIZE, std::nullopt}}}
};

static SpriteMap<SpriteSymbolType> IconSprites
{
	"chess_symbols.png", nullptr, wxSize(0, 0),
	std::unordered_map<SpriteSymbolType, SubSpriteData>{
	{SpriteSymbolType::MoveSpot,				{{0,0}, {200, 200}, std::nullopt}},
	{SpriteSymbolType::DisabledOverlay,			{{0,1}, {200, 200}, std::nullopt}}}
};

AudioClip::AudioClip(const std::filesystem::path& path) : 
	m_path(path), m_Path(m_path), m_sound(m_path.string()), m_Sound(nullptr)
{
	if (m_sound.IsOk()) m_Sound = &m_sound;
}

AudioClip::AudioClip(const AudioClip& clip) :
	m_path(clip.m_path), m_Path(m_path), m_sound(m_path.string()), m_Sound(nullptr)
{
	if (m_sound.IsOk()) m_Sound = &m_sound;
}

bool AudioClip::HasGoodClip() const
{
	return m_Sound != nullptr;
}

bool AudioClip::TryPlaySound() const
{
	if (!HasGoodClip()) return false;
	bool played = m_Sound->Play(wxSOUND_ASYNC);
	Utils::Log(Utils::LogType::Error, std::format("playing sound at: {} played:{} is ok: {}",
		m_path.string(), std::to_string(played), std::to_string(m_sound.IsOk())));
	return played;
}

void AudioClip::PlaySoundWindows() const
{
	//wstring is wxchar[] (like string is char[]) since playsound accepts unicode characters
	std::wstring widePath = m_path.wstring();
	Utils::Log(Utils::LogType::Error, std::format("Playing windows sound at {}", m_path.string()));
	PlaySound(m_path.wstring().c_str(), NULL, SND_FILENAME | SND_ASYNC);
}

static const std::unordered_map<AudioClipType, AudioClip> AudioClips =
{
	{AudioClipType::Move, AudioClip("chess_move.wav")},
	{ AudioClipType::PieceSelect, AudioClip("chess_tile_select.wav") }
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

bool TryLoadResource(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage)
{
	//Checks to make sure the image type has a handler
	if (!HasValidImageHandler(resourceType))
	{
		Utils::Log("PIECE CHECK Invalid image handelr");
		return false;
	}

	bool result = false;
	const std::string fullpath = resourcePath.string();
	if (outImage != nullptr)
	{
		result = outImage->LoadFile(fullpath, resourceType);
	}
	else
	{
		Utils::Log("LOADING Out image is nullptr so local failed!");
		outImage = new wxImage();
		result= outImage->LoadFile(fullpath, resourceType);
	}
	
	if (!result)
	{
		std::string err = std::format("Tried to load resource at path {} but failed", resourcePath.string());
		Utils::Log(Utils::LogType::Error, err);
		wxLogError(err.c_str());
	}
	return result;
}

bool TryLoadSubImageFromResource(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType,
	const wxPoint& startPos, const wxSize& size, wxImage* outImage)
{
	wxImage resourceLoadOutImage;
	if (!TryLoadResource(resourcePath, resourceType, &resourceLoadOutImage)) return false;
	wxImage subImage= GetSubImage(resourceLoadOutImage, startPos, size);
	*outImage = subImage;
	return true;
}

/*
bool TryLoadAllPieceImages()
{
	if (PieceSprites.MapImage == nullptr)
	{
		PieceSprites.MapImage = new wxImage();
		Utils::Log(std::format("LOADING state of temp image: {}", std::to_string(PieceSprites.MapImage != nullptr)));
		if (!TryLoadResource(PieceSprites.Path, wxBITMAP_TYPE_PNG, PieceSprites.MapImage)) return false;
	}
	
	wxImage currentSubImage;
	wxPoint tileMapPos;
	int r = 0;
	int c = 0;
	bool foundAtLeastOnePiece = false;
	for (const auto& pieceSpriteData : PieceSprites.SpriteData)
	{
		foundAtLeastOnePiece = true;
		r = pieceSpriteData.second.Position.x;
		c = pieceSpriteData.second.Position.y;
		tileMapPos = { c * SPRITE_SIZE.x + c * SPRITE_SPACING.x, r * SPRITE_SIZE.y + r * SPRITE_SPACING.y };

		std::string message = std::format("LOADING {},{} Pos: {} {} has map: {} state of image: {}", std::to_string(r), std::to_string(c),
			std::to_string(tileMapPos.x), std::to_string(tileMapPos.y), std::to_string(PieceSprites.MapImage != nullptr),
			std::to_string(PieceSprites.SpriteData.at(pieceSpriteData.first).Position.x));
		wxLogMessage(message.c_str());
		PieceSprites.SpriteData.at(pieceSpriteData.first).MaybeImage = GetSubImage(*PieceSprites.MapImage, tileMapPos, SPRITE_SIZE);
		//wxImage result = GetSubImage(*PieceSprites.MapImage, tileMapPos, SPRITE_SIZE);
	}
	return foundAtLeastOnePiece;
}
*/

template<typename T>
bool TryLoadAllSpriteMapImages(SpriteMap<T>& map)
{
	if (map.MapImage == nullptr)
	{
		map.MapImage = new wxImage();
		//Utils::Log(std::format("LOADING state of temp image: {}", std::to_string(PieceSprites.MapImage != nullptr)));
		if (!TryLoadResource(map.Path, wxBITMAP_TYPE_PNG, map.MapImage)) return false;
	}

	wxImage currentSubImage;
	wxPoint tileMapPos;
	int r = 0;
	int c = 0;

	wxSize spriteSize;
	wxSize spriteSpacing;
	bool foundAtLeastOnePiece = false;
	for (const auto& pieceSpriteData : map.SpriteData)
	{
		foundAtLeastOnePiece = true;
		r = pieceSpriteData.second.Position.x;
		c = pieceSpriteData.second.Position.y;

		spriteSize = pieceSpriteData.second.Size;
		spriteSpacing = map.SpriteSpacing;
		tileMapPos = { c * spriteSize.x + c * spriteSpacing.x, r * spriteSize.y + r * spriteSpacing.y };

		/*std::string message = std::format("LOADING {},{} Pos: {} {} has map: {} state of image: {}", std::to_string(r), std::to_string(c),
			std::to_string(tileMapPos.x), std::to_string(tileMapPos.y), std::to_string(PieceSprites.MapImage != nullptr),
			std::to_string(PieceSprites.SpriteData.at(pieceSpriteData.first).Position.x));
		wxLogMessage(message.c_str());*/
		map.SpriteData.at(pieceSpriteData.first).MaybeImage = GetSubImage(*map.MapImage, tileMapPos, spriteSize);
		//wxImage result = GetSubImage(*PieceSprites.MapImage, tileMapPos, SPRITE_SIZE);
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

bool TryCacheAllSprites()
{
	if (!PieceSprites.HasSpritesStored() && !TryLoadAllSpriteMapImages(PieceSprites))
	{
		const std::string error = std::format("Tried to cache all piece sprites "
			"but failed to load all from resource manager");
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}
	else if (!IconSprites.HasSpritesStored() && !TryLoadAllSpriteMapImages(IconSprites))
	{
		const std::string error = std::format("Tried to cache all sprite icons "
			"but failed to load all from resource manager");
		Utils::Log(Utils::LogType::Error, error);
		return false;
	}
	return true;
}

wxBitmap GetBitMapFromSprite(wxImage& image, const wxSize& targetSize)
{
	wxSize startSize(image.GetWidth(), image.GetHeight());
	if (startSize != targetSize)
	{
		float newWidthScale = static_cast<float>(targetSize.x) / startSize.x;
		float newHeightScale = static_cast<float>(targetSize.y) / startSize.y;
		float totalScale = std::max(newWidthScale, newHeightScale);
		wxSize size(image.GetWidth() * totalScale, image.GetHeight() * totalScale);

		Resize(image, size);
		std::string m = std::format("Start: ({}, {})  END: ({}, {}) target: {} {}", std::to_string(startSize.x), std::to_string(startSize.y),
			std::to_string(image.GetWidth()), std::to_string(image.GetHeight()), std::to_string(targetSize.x), std::to_string(targetSize.y));
		wxLogMessage(m.c_str());
	}

	return wxBitmap{ image };
}

std::optional<wxImage> TryGetSpriteFromPiece(const PieceTypeInfo& info)
{
	TryCacheAllSprites();
	std::optional<SubSpriteData> maybeData = PieceSprites.TryGetSpriteFromID(info);
	if (!maybeData.has_value()) return std::nullopt;
	else return maybeData->MaybeImage;
}

std::optional<wxBitmap> TryGetBitMapFromPiece(const PieceTypeInfo& info, const wxSize& targetSize)
{
	std::optional<wxImage> maybeImage = TryGetSpriteFromPiece(info);
	if (!maybeImage.has_value()) return std::nullopt;

	//TODO: maybe potential issue here where we use a reference to a local value that 
	//will go out of scope and is used for returning structure
	return GetBitMapFromSprite(maybeImage.value(), targetSize);
}

std::optional<wxBitmap> TryGetBitMapForIcon(const SpriteSymbolType& symbolType, const wxSize& targetSize)
{
	TryCacheAllSprites();
	std::optional<SubSpriteData> maybeData = IconSprites.TryGetSpriteFromID(symbolType);
	if (!maybeData.has_value()) return std::nullopt;
	else if (!maybeData.value().MaybeImage.has_value()) return std::nullopt;

	//TODO: maybe potential issue here where we use a reference to a local value that 
	//will go out of scope and is used for returning structure
	return GetBitMapFromSprite(maybeData.value().MaybeImage.value(), targetSize);
}

const AudioClip* TryGetSound(const AudioClipType& type)
{
	auto clipIt = AudioClips.find(type);
	if (clipIt == AudioClips.end()) return nullptr;
	else if (!clipIt->second.HasGoodClip()) return nullptr;
	else return &(clipIt->second);
}
