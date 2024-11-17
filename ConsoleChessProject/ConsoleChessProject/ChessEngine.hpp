#pragma once
#include <vector>
#include "MainFrame.hpp"

class ChessEngine
{
private:
	std::vector<MainFrame*> windows;

public:

private:
	
	
public:
	ChessEngine();
	~ChessEngine();

	void Initialize();
	void Shutdown();

	void CreateWindow(const std::string windowName);
};

