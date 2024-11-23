#include <wx/wx.h>
#include "DirectionalLayout.hpp"

inline SpacingType operator|(SpacingType lhs, SpacingType rhs)
{
	return static_cast<SpacingType>(
		static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

SpacingType& operator|=(SpacingType lhs, SpacingType rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

inline SpacingType operator&(SpacingType lhs, SpacingType rhs)
{
	return static_cast<SpacingType>(
		static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

SpacingType& operator&=(SpacingType lhs, SpacingType rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

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
	delete _sizer;
	delete Sizer;
	_sizer = nullptr;
	Sizer = nullptr;
}

void DirectionalLayout::AddChild(wxWindow* child, const SpacingType spacingType, const int spacing)
{
	int spacingFlags = 0;
	if ((spacingType & SpacingType::Expand) != SpacingType::None) spacingFlags |= wxEXPAND;
	if ((spacingType & SpacingType::Top) != SpacingType::None) spacingFlags |= wxTOP;
	if ((spacingType & SpacingType::Bottom) != SpacingType::None) spacingFlags |= wxBOTTOM;
	if ((spacingType & SpacingType::Left) != SpacingType::None) spacingFlags |= wxLEFT;
	if ((spacingType & SpacingType::Right) != SpacingType::None) spacingFlags |= wxRIGHT;
	if ((spacingType & SpacingType::Center) != SpacingType::None) spacingFlags |= wxCENTER;

	_sizer->Add(child, 0, spacingFlags, spacing);
	this->Layout();
}