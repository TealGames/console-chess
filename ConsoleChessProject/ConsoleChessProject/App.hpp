#pragma once
#include <wx/wx.h>
#include "ChessEngine.hpp"

class App : public wxApp
{
private:
	ChessEngine m_Engine;

public:
	bool OnInit();
	int OnExit();
};