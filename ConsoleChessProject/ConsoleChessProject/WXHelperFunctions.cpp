#include <wx/wx.h>
#include <string>
#include <format>
#include "HelperFunctions.hpp"
#include "WXHelperFunctions.hpp"

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

	std::string ToString(const wxColour& color)
	{
		int r = static_cast<int>(color.Red());
		int g = static_cast<int>(color.Green());
		int b = static_cast<int>(color.Blue());
		const std::string colorStr = std::format("[R:{}, G:{}, B:{}]", std::to_string(r), std::to_string(g), std::to_string(b));
		return colorStr;
	}

	std::string ToString(const wxSize& size)
	{
		const std::string str = std::format("(X:{} Y:{})", std::to_string(size.x), std::to_string(size.y));
		return str;
	}

	wxSize BestSizePreserveAspect(const wxSize& startSize, const wxSize& targetSize)
	{
		float newWidthScale = static_cast<float>(targetSize.x) / startSize.x;
		float newHeightScale = static_cast<float>(targetSize.y) / startSize.y;

		float totalScale = std::min(newWidthScale, newHeightScale);
			
		return startSize * totalScale;
	}
}

