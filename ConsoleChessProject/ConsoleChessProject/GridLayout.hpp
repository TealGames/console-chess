#pragma once
#include <wx/wx.h>
#include "WXHelperFunctions.hpp"

class GridLayout : public wxPanel
{
private:
	wxGridSizer* _sizer = nullptr;
public:
	const wxGridSizer* Sizer = nullptr;

private:
public:
	/// <summary>
	/// </summary>
	/// <param name="parent"></param>
	/// <param name="rowsCols">[#rows, #cols]</param>
	/// <param name="margins">[vertical gap, horizontal gap]</param>
	/// <param name="position"></param>
	/// <param name="size"></param>
	/// <param name="style"></param>
	GridLayout(wxWindow* parent, const wxSize& rowsCols, const wxSize& margins, const wxPoint& position = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, const long style = 0);
	~GridLayout();

	/// <summary>
	/// </summary>
	/// <param name="child"></param>
	/// <param name="proportion">The amount of space it should take up in the parent container when resized.
	/// If the amount is 0, it will not be changed and any amount >0 is amount of weight in parent</param>
	/// <param name="marginType">The type of spacing for the matgins. </param>
	/// <param name="margin">The amount of spacing units based on the type for the child</param>
	void AddChild(wxWindow* child, const int proportion = 0, const SpacingType marginType = static_cast<SpacingType>(0), const int margin = 0);

	/// <summary>
	/// Removes all children from having their layout updated via sizer
	/// </summary>
	void ClearAll();

	//Will remove the children and detach them from sizer 
	// (simply destroying them might lead to nullrefs with sizer)
	void DestroyLayout();
};

