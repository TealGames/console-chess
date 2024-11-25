#pragma once
#include <vector>
#include "MainFrame.hpp"
#include "GameManager.hpp"

class ChessEngine
{
private:
	std::vector<MainFrame*> m_Windows;
	Core::GameManager GameManager;

public:
	
private:
	
	
public:
	ChessEngine();
	~ChessEngine();

	void Initialize();
	void Shutdown();

	void CreateWindow(const std::string windowName);
};

