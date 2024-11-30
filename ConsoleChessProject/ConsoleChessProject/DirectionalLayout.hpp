#pragma once
#include <wx/wx.h>
#include "WXHelperFunctions.hpp"

enum class LayoutType
{
	Horizontal,
	Vertical,
};

class DirectionalLayout : public wxPanel
{
private:
	wxBoxSizer* _sizer = nullptr;
public:
	const wxBoxSizer* Sizer = nullptr;

private:
public:
	DirectionalLayout(wxWindow* parent, const LayoutType& type, const wxPoint& position= wxDefaultPosition,
		const wxSize& size= wxDefaultSize, const long style=0);
	~DirectionalLayout();

	/// <summary>
	/// </summary>
	/// <param name="child"></param>
	/// <param name="proportion">The amount of space it should take up in the parent container when resized.
	/// If the amount is 0, it will not be changed and any amount >0 is amount of weight in parent</param>
	/// <param name="marginType">The type of spacing for the matgins. </param>
	/// <param name="margin">The amount of spacing units based on the type for the child</param>
	void AddChild(wxWindow* child, const int proportion=0, const SpacingType marginType=static_cast<SpacingType>(0), const int margin=0, const int childIndex=-1);

	/// <summary>
	/// Removes all children from having their layout updated via sizer
	/// </summary>
	void ClearAll();

	//Will remove the children and detach them from sizer 
	// (simply destroying them might lead to nullrefs with sizer)
	void DestroyLayout();
};

