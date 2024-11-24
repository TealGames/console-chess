#pragma once
#include <functional>
#include "Color.hpp"
#include "Event.hpp"
#include "GameState.hpp"

namespace GameManagement
{
	struct EndGameInfo
	{
		const ColorTheme& WinningPlayer;
	};

	class GameManager
	{
	public:
		static constexpr bool ADVANCE_TURN = true;

		Utils::Event<void> GameStartEvent;
		Utils::Event<void, EndGameInfo> GameEndEvent;
		Utils::Event<void, const ColorTheme> TurnChangeEvent;

	private:

	private:
		std::optional<ColorTheme> TryGetOtherPlayer(const GameState& state) const;
		
		void EndGame(GameState& state);

	public:
		GameManager();

		GameState StartNewGame();
		void AdvanceTurn(GameState& state);
	};
}
