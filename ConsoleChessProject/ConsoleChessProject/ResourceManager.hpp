#pragma once
#include <wx/wx.h>
#include <filesystem>
#include "Piece.hpp"

bool TryLoad(const std::filesystem::path& resourcePath, const wxBitmapType& resourceType, wxImage* outImage=nullptr);
bool TryLoadPieceImage(const PieceType& piece, wxImage* outImage=nullptr);

bool TryLoadAll();

