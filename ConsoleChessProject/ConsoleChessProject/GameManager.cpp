#include <functional>
#include <vector>
#include "GameManager.hpp"
#include "Color.hpp"
#include "BoardManager.hpp"

static ColorTheme currentPlayer;
static std::vector<EndGameCallbackType> endGameCallbacks;
static std::vector<TurnChangeCallbackType> turnChangeCallbacks;

void StartGame()
{
	currentPlayer = ColorTheme::Light;
}

static ColorTheme GetOtherPlayer()
{
	if (currentPlayer == ColorTheme::Light) return ColorTheme::Dark;
	else return ColorTheme::Light;
}

static void EndGame()
{
	const ColorTheme otherPlayer = GetOtherPlayer();
	for (const EndGameCallbackType& callback : endGameCallbacks)
	{
		callback({ otherPlayer });
	}
}

static void NextTurn()
{
	currentPlayer = GetOtherPlayer();
	if (InCheckmate() || GetAvailablePieces(currentPlayer) == 0) EndGame();

	for (const TurnChangeCallbackType& callback : turnChangeCallbacks)
	{
		callback(currentPlayer);
	}
}


void AddEndGameCallback(const EndGameCallbackType& callback)
{
	endGameCallbacks.push_back(callback);
}

void AddTurnChangeCallback(const TurnChangeCallbackType& callback)
{
	turnChangeCallbacks.push_back(callback);
}