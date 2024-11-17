#include <functional>
#include <vector>
#include "GameManager.hpp"
#include "Color.hpp"
#include "BoardManager.hpp"
#include "Event.hpp"

namespace GameManagement
{
	GameManager::GameManager()
		: _currentPlayerTurn(std::nullopt), GameStartEvent(), GameEndEvent(), TurnChangeEvent()
	{}

	void GameManager::StartNewGame()
	{
		_currentPlayerTurn = ColorTheme::Light;
		Board::CreateDefaultBoard();    
		GameStartEvent.Invoke();
	}

	std::optional<ColorTheme> GameManager::TryGetOtherPlayer() const
	{
		if (!_currentPlayerTurn.has_value()) return std::nullopt;

		if (_currentPlayerTurn == ColorTheme::Light) return ColorTheme::Dark;
		else return ColorTheme::Light;
	}

	void GameManager::NextTurn()
	{
		_currentPlayerTurn = TryGetOtherPlayer();
		if (!_currentPlayerTurn.has_value())
		{
			std::string error = std::format("Tried to change players turn in "
				"GameManager but there is empty player's turn!");
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		if (Board::InCheckmate() || 
			Board::GetAvailablePieces(_currentPlayerTurn.value()) == 0) EndGame();

		TurnChangeEvent.Invoke(_currentPlayerTurn.value());
	}

	void GameManager::EndGame()
	{
		std::optional<ColorTheme> maybeOther= TryGetOtherPlayer();
		if (!maybeOther.has_value())
		{
			std::string error = std::format("Tried to end the game in GameManager "
				"but there is no player's turn!");
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		//The player who did not cause game end is the one who wins
		GameEndEvent.Invoke(EndGameInfo{maybeOther.value()});
	}
}