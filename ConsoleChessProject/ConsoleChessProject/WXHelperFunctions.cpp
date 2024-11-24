#include "WXHelperFunctions.hpp"
#include <wx/wx.h>

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

namespace WXUtils
{
	void CenterX(wxWindow* element)
	{
		wxWindow* parent = element->GetParent();
		int xDiff = parent->GetSize().x - element->GetSize().x;

		//We use the x diff to set the poisition (remember if it based on the left
		//endge so we can just divide by 2 to get amount needed to move)
		element->SetPosition(wxPoint(xDiff / 2, element->GetPosition().y));
	}
}

