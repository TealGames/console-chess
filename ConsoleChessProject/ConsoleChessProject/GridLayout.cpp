#include <wx/wx.h>x
#include "GridLayout.hpp"
#include "WXHelperFunctions.hpp"

GridLayout::GridLayout(wxWindow* parent, const wxSize& rowsCols, const wxSize& margins, const wxPoint& position,
	const wxSize& size, const long style) : wxPanel(parent, wxID_ANY, position, size, style)
{
	_sizer = new wxGridSizer(rowsCols.x, rowsCols.y, margins.x, margins.y);
	Sizer = _sizer;
	this->SetSizer(_sizer);
}

GridLayout::~GridLayout()
{

}

void GridLayout::AddChild(wxWindow* child, const int proportion, const SpacingType spacingType, const int spacing)
{
	int spacingFlags = 0;
	if ((spacingType & SpacingType::Expand) != SpacingType::None) spacingFlags |= wxEXPAND;
	if ((spacingType & SpacingType::Top) != SpacingType::None) spacingFlags |= wxTOP;
	if ((spacingType & SpacingType::Bottom) != SpacingType::None) spacingFlags |= wxBOTTOM;
	if ((spacingType & SpacingType::Left) != SpacingType::None) spacingFlags |= wxLEFT;
	if ((spacingType & SpacingType::Right) != SpacingType::None) spacingFlags |= wxRIGHT;
	if ((spacingType & SpacingType::Center) != SpacingType::None) spacingFlags |= wxCENTER;

	_sizer->Add(child, proportion, spacingFlags, spacing);
	this->Layout();
}

void GridLayout::ClearAll()
{
	_sizer->Clear();
}

void GridLayout::DestroyLayout()
{
	ClearAll();
	DestroyChildren();
}