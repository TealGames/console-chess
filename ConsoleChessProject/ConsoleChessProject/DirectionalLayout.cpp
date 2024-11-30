#include <wx/wx.h>
#include "DirectionalLayout.hpp"
#include "WXHelperFunctions.hpp"

DirectionalLayout::DirectionalLayout(wxWindow* parent, const LayoutType& type, 
	const wxPoint& position, const wxSize& size, const long style)
	: wxPanel(parent, wxID_ANY, position, size, style)
{
	int orientation = 0;
	if (type == LayoutType::Horizontal) orientation = wxHORIZONTAL;
	else if (type == LayoutType::Vertical) orientation = wxVERTICAL;

	_sizer = new wxBoxSizer(orientation);
	Sizer = _sizer;
	this->SetSizer(_sizer);
}

DirectionalLayout::~DirectionalLayout()
{
	/*delete _sizer;
	delete Sizer;
	_sizer = nullptr;
	Sizer = nullptr;*/
}

void DirectionalLayout::AddChild(wxWindow* child, const int proportion, const SpacingType spacingType, const int spacing, const int childIndex)
{
	int spacingFlags = 0;
	if ((spacingType & SpacingType::Expand) != SpacingType::None) spacingFlags |= wxEXPAND;
	if ((spacingType & SpacingType::Top) != SpacingType::None) spacingFlags |= wxTOP;
	if ((spacingType & SpacingType::Bottom) != SpacingType::None) spacingFlags |= wxBOTTOM;
	if ((spacingType & SpacingType::Left) != SpacingType::None) spacingFlags |= wxLEFT;
	if ((spacingType & SpacingType::Right) != SpacingType::None) spacingFlags |= wxRIGHT;
	if ((spacingType & SpacingType::Center) != SpacingType::None) spacingFlags |= wxCENTER;

	if (childIndex < 0) _sizer->Add(child, proportion, spacingFlags, spacing);
	else _sizer->Insert(childIndex, child, proportion, spacingFlags, spacing);
	this->Layout();
}

void DirectionalLayout::ClearAll()
{
	_sizer->Clear();
}

void DirectionalLayout::DestroyLayout()
{
	ClearAll();
	DestroyChildren();
}