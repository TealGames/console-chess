#pragma once
#include <wx/wx.h>
#include <unordered_map>
#include <filesystem>
#include "Piece.hpp"
#include "Color.hpp"


bool TryLoad(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage=nullptr);
bool TryLoadAllPieceImages(std::unordered_map<PieceTypeInfo, wxImage>* outImages=nullptr);

void Resize(wxImage& resourceImage, const wxSize& newSize);
void ResizePreserveAspect(wxImage& resourceImage, const wxSize& newSize, const wxPoint& point);
wxImage GetSubImage(const wxImage& resourceImage, const wxPoint& position, const wxSize& size);

bool TryCacheAllSprites();
std::optional<wxImage> TryGetSpriteFromPiece(const PieceTypeInfo& info);