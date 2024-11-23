#pragma once
#include <wx/wx.h>

enum class LayoutType
{
	Horizontal,
	Vertical,
};

enum class SpacingType : unsigned int
{
	None= 0,
	Expand= 1 << 0,
	Top= 1 << 1,
	Bottom= 1 << 2,
	Left= 1 << 3,
	Right= 1 << 4,
	Center= 1<<5,
};

inline SpacingType operator|(SpacingType lhs, SpacingType rhs);
SpacingType& operator|=(SpacingType lhs, SpacingType rhs);
inline SpacingType operator&(SpacingType lhs, SpacingType rhs);
SpacingType& operator&=(SpacingType lhs, SpacingType rhs);

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

	void AddChild(wxWindow* child, const SpacingType spacingType=static_cast<SpacingType>(0), const int spacing=0);
};

