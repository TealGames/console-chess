#pragma once
#include <wx/wx.h>
#include <wx/sound.h>
#include <unordered_map>
#include <optional>
#include <filesystem>
#include "Piece.hpp"
#include "Color.hpp"

enum class SpriteSymbolType
{
	MoveSpot,
	DisabledOverlay,
};

struct SubSpriteData
{
	wxPoint Position;
	wxSize Size;
	std::optional<wxImage> MaybeImage;
};

template<typename SpriteIDType>
struct SpriteMap
{
	const std::filesystem::path Path;
	wxImage* MapImage;
	const wxSize SpriteSpacing;
	std::unordered_map<SpriteIDType, SubSpriteData> SpriteData;

	std::optional<SubSpriteData> TryGetSpriteFromID(const SpriteIDType& id)
	{
		auto spriteDataIt = SpriteData.find(id);
		if (spriteDataIt == SpriteData.end()) return std::nullopt;
		else return spriteDataIt->second;
	}

	bool HasSpritesStored()
	{
		if (SpriteData.empty()) return false;
		int filledSprites = 0;
		for (const auto& data : SpriteData)
		{
			if (data.second.MaybeImage.has_value()) filledSprites++;
		}
		return filledSprites == SpriteData.size();
	}
};

enum class AudioClipType
{
	Move
};

class AudioClip
{
private:
	std::filesystem::path m_path;
	wxSound m_sound;

public:
	const std::filesystem::path& m_Path;
	const wxSound* m_Sound;
	
	AudioClip(const std::filesystem::path& path);
	AudioClip(const AudioClip& clip);

	bool HasGoodClip() const;
	bool TryPlaySound() const;
};

bool TryLoadResource(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage=nullptr);
bool TryLoadSubImageFromResource(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType,
	const wxPoint& startPos, const wxSize& size, wxImage* outImage = nullptr);

void Resize(wxImage& resourceImage, const wxSize& newSize);
void ResizePreserveAspect(wxImage& resourceImage, const wxSize& newSize, const wxPoint& point);
wxImage GetSubImage(const wxImage& resourceImage, const wxPoint& position, const wxSize& size);
wxBitmap GetBitMapFromSprite(wxImage& image, const wxSize& targetSize);

bool TryCacheAllSprites();

std::optional<wxImage> TryGetSpriteFromPiece(const PieceTypeInfo& info);
std::optional<wxBitmap> TryGetBitMapFromPiece(const PieceTypeInfo& info, const wxSize& targetSize);

std::optional<wxBitmap> TryGetBitMapForIcon(const SpriteSymbolType& symbolType, const wxSize& targetSize);

const AudioClip* TryGetSound(const AudioClipType& type);