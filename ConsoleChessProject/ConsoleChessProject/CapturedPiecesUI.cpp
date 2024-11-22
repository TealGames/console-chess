#include <wx/wx.h>
#include "CapturedPiecesUI.hpp"
#include "ResourceManager.hpp"
#include "BoardManager.hpp"
#include "GameState.hpp"

static wxPanel* lightPanel = nullptr;
static wxPanel* darkPanel = nullptr;

static void UpdateDisplay(const GameState& state)
{
	wxImage image = TryGetSpriteFromPiece();
	wxBitmap map(image);
	_bitMapDisplay = new wxStaticBitmap(this, wxID_ANY, map, this->GetPosition());
	_bitMapDisplay->Center();
}

void CreateCaptureDisplay(wxWindow* parent)
{
	lightPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, PANEL_SIZE);
	darkPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, PANEL_SIZE);

	Board::AddPieceMoveCallback(&UpdateDisplay);
}
