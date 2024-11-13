#pragma once
#include <functional>
#include "Color.hpp"

struct EndGameInfo
{
	const ColorTheme& WinningPlayer;
};
using EndGameCallbackType = std::function<void(EndGameInfo)>;
using TurnChangeCallbackType = std::function<void(ColorTheme)>;

void StartGame();
void AddEndGameCallback(const EndGameCallbackType& callback);
void AddTurnChangeCallback(const TurnChangeCallbackType& callback);