#pragma once
#include <functional>
#include "Color.hpp"
#include "Event.hpp"

namespace GameManagement
{

	struct EndGameInfo
	{
		const ColorTheme& WinningPlayer;
	};

	class GameManager
	{
	public:
		Utils::Event<void> GameStartEvent;
		Utils::Event<void, EndGameInfo> GameEndEvent;
		Utils::Event<void, ColorTheme> TurnChangeEvent;

	private:
		std::optional<ColorTheme> _currentPlayerTurn;

	private:
		std::optional<ColorTheme> TryGetOtherPlayer() const;
		void NextTurn();
		void EndGame();

	public:
		GameManager();

		void StartNewGame();
	};
}
