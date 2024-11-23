#include "WXHelperFunctions.hpp"
#include <wx/wx.h>

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

