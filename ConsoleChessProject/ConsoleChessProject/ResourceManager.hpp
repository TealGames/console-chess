#pragma once
#include <wx/wx.h>
#include <filesystem>
#include "Piece.hpp"
#include "Color.hpp"

struct PieceInfo
{
	const ColorTheme Color;
	const PieceType PieceType;
};

bool TryLoad(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage=nullptr);
bool TryLoadPieceImages(const std::vector<PieceInfo>& pieces, std::vector<wxImage>* outImages=nullptr);

bool TryLoadAll();

void Resize(wxImage& resourceImage, const wxSize& newSize);
void ResizePreserveAspect(wxImage& resourceImage, const wxSize& newSize, const wxPoint& point);
wxImage GetSubImage(const wxImage& resourceImage, const wxPoint& position, const wxSize& size);
