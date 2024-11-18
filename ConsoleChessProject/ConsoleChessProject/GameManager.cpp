#include <functional>
#include <vector>
#include "GameManager.hpp"
#include "Color.hpp"
#include "BoardManager.hpp"
#include "Event.hpp"
#include "GameState.hpp"

namespace GameManagement
{
	GameManager::GameManager()
		: GameStartEvent(), GameEndEvent(), TurnChangeEvent()
	{}

	GameState GameManager::StartNewGame()
	{
		GameState newState = {};
		newState.CurrentPlayer= ColorTheme::Light;
		Board::CreateDefaultBoard(newState); 
		Utils::Log(Utils::LogType::Warning, std::format("Start game: {}", newState.ToString()));

		//TODO: error arises when invoking
		//GameStartEvent.Invoke();
		return newState;
	}

	std::optional<ColorTheme> GameManager::TryGetOtherPlayer(const GameState& state) const
	{
		if (state.CurrentPlayer == ColorTheme::Light) return ColorTheme::Dark;
		else return ColorTheme::Light;
	}

	void GameManager::AdvanceTurn(GameState& state)
	{
		std::optional<ColorTheme> maybeOtherPlayer = TryGetOtherPlayer(state);
		if (!maybeOtherPlayer.has_value())
		{
			std::string error = std::format("Tried to change players turn in "
				"GameManager but there is empty player's turn! Current Player turn left unchanged: {}", 
				ToString(state.CurrentPlayer));
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		state.CurrentPlayer = maybeOtherPlayer.value();
		if (state.InCheckmate || Board::GetAvailablePieces(state, state.CurrentPlayer) == 0) 
			EndGame(state);

		TurnChangeEvent.Invoke(state.CurrentPlayer);
	}

	void GameManager::EndGame(GameState& state)
	{
		std::optional<ColorTheme> maybeOther= TryGetOtherPlayer(state);
		if (!maybeOther.has_value())
		{
			std::string error = std::format("Tried to end the game in GameManager "
				"but there is no player's turn!");
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		//The player who did not cause game end is the one who wins
		//GameEndEvent.Invoke(EndGameInfo{maybeOther.value()});
	}
}