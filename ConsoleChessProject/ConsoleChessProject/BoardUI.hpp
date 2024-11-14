#pragma once
#include <wx/wx.h>
#include <unordered_map>
#include "Point2DInt.hpp"
#include "Cell.hpp"
#include "Piece.hpp"

void CreateBoard(wxWindow* parent);
bool TryRenderPieceAtPos(const Utils::Point2DInt& pos, const PieceTypeInfo& pieceInfo);
bool TryRenderAllPieces();
void EndCleanup();

