#pragma once
#include <wx/wx.h>
#include <string>

enum class SpacingType : unsigned int
{
	None = 0,
	Expand = 1 << 0,
	Top = 1 << 1,
	Bottom = 1 << 2,
	Left = 1 << 3,
	Right = 1 << 4,
	Center = 1 << 5,
};

inline SpacingType operator|(SpacingType lhs, SpacingType rhs);
SpacingType& operator|=(SpacingType lhs, SpacingType rhs);
inline SpacingType operator&(SpacingType lhs, SpacingType rhs);
SpacingType& operator&=(SpacingType lhs, SpacingType rhs);

const SpacingType SPACING_ALL_SIDES = SpacingType::Top | SpacingType::Left
| SpacingType::Bottom | SpacingType::Right;

namespace WXUtils
{
	void CenterX(wxWindow* element);
	std::string ToString(const wxColour& color);
	std::string ToString(const wxSize& size);

	wxSize BestSizePreserveAspect(const wxSize& startSize, const wxSize& targetSize);
}


