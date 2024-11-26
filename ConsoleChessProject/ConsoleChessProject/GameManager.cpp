#include <functional>
#include <vector>
#include <format>
#include <string>
#include <unordered_map>
#include <optional>
#include "HelperFunctions.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "BoardManager.hpp"
#include "Event.hpp"
#include "GameState.hpp"
#include "PieceMoveResult.hpp"

namespace Core
{
	GameManager::GameManager()
		: _allGameStates() //,GameStartEvent(), GameEndEvent(), TurnChangeEvent()
	{
		//Utils::Log(std::format("GAME MANAGER created Current game states: {}", std::to_string(TotalGameStatesCount())));
	}

	GameStateQueryResult GameManager::HasGameStateId(const std::string& gameStateId)
	{
		if (_allGameStates.empty()) return {false, std::nullopt};

		GameStateCollectionType::iterator gameStateIt = _allGameStates.find(gameStateId);
		bool hasState = gameStateIt != _allGameStates.end();
		return hasState ? GameStateQueryResult{ true, gameStateIt } : GameStateQueryResult{ false, std::nullopt };
	}

	GameState* GameManager::TryGetGameStateMutable(const std::string& gameStateID)
	{
		GameStateQueryResult result = HasGameStateId(gameStateID);
		if (!result.HasGameId) return nullptr;
		else if (result.HasGameId && !result.ResultIt.has_value())
		{
			const std::string error = std::format("[GAME_MANAGER]: Tried to get game state from ID: "
				"{} but the iterator is empty even though the result was found", gameStateID);
			Utils::Log(Utils::LogType::Error, error);
			return nullptr;
		}
		return &(result.ResultIt.value()->second);
	}

	const GameState* GameManager::TryGetGameState(const std::string& gameStateID)
	{
		return TryGetGameStateMutable(gameStateID);
	}

	bool GameManager::IsValidGameState(const GameState* state, const bool logError, const std::string& operationName)
	{
		if (state == nullptr)
		{
			std::vector<std::string> ids = Utils::GetKeysFromMap<std::string, GameState>(_allGameStates.begin(), _allGameStates.end());
			const std::string err = std::format("[GAME_MANAGER]: Tried to execute operation: {} "
				"but game state was NULL (probably not found). All active states ids: {}", operationName,
				Utils::ToStringIterable<std::vector<std::string>, std::string>(ids));
			Utils::Log(Utils::LogType::Error, err);
			return false;
		}
		return true;
	}

	const GameState& GameManager::StartNewGame(const std::string& newGameStateID)
	{
		Utils::Log(std::format("Size of game states: {}", std::to_string(_allGameStates.size())));
		GameStateQueryResult newGameIdQuery = HasGameStateId(newGameStateID);
		if (newGameIdQuery.HasGameId)
		{
			const std::string error = std::format("[GAME_MANAGER]: Tried to start a new game with a game state "
				"id of: {} but this id already exists for a currently existing game state", newGameStateID);
			Utils::Log(Utils::LogType::Error, error);
			return {};
		}

		auto newStateIt= _allGameStates.emplace(newGameStateID, GameState{});
		if (!newStateIt.second)
		{
			const std::string error = std::format("[GAME_MANAGER]: Tried to start a new game with a game state "
				"id of: {} but could not create the new game state", newGameStateID);
			Utils::Log(Utils::LogType::Error, error);
			return {};
		}

		GameState& newState = newStateIt.first->second;
		newState.CurrentPlayer = ColorTheme::Light;
		Board::CreateDefaultBoard(newState);
		
		return newState;

		//Utils::Log(Utils::LogType::Warning, std::format("GAME_MANAGER: Start game: {}", newState.ToString()));

		//TODO: error arises when invoking
		//GameStartEvent.Invoke();
		
	}

	std::optional<ColorTheme> GameManager::TryGetOtherPlayer(const GameState& state) const
	{
		if (state.CurrentPlayer == ColorTheme::Light) return ColorTheme::Dark;
		else return ColorTheme::Light;
	}

	bool GameManager::IsPositionWithinBounds(const Utils::Point2DInt& pos) const
	{
		return Board::IsWithinBounds(pos);
	}


	PieceMoveResult GameManager::TryMoveForState(const std::string& gameStateID,
		const Utils::Point2DInt& currentPos, const Utils::Point2DInt& newPos)
	{
		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		if (!IsValidGameState(maybeGameState, true, std::format("TryMove(id:{})", gameStateID)))
		{
			return { newPos, false, std::format("Failed to retrieve current game data") };
		}
			
		return Board::TryMove(*maybeGameState, currentPos, newPos);
	}

	std::vector<MoveInfo> GameManager::TryGetPossibleMovesForPieceAt(const std::string& gameStateID, const Utils::Point2DInt& pos)
	{
		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		//Utils::Log(std::format("Try get possible moves game manager has state: {}", std::to_string(maybeGameState!=nullptr)));
		if (!IsValidGameState(maybeGameState, true, std::format("GetPossibleMoves(id:{})", gameStateID)))
		{
			return {};
		}
		return Board::GetPossibleMovesForPieceAt(*maybeGameState, pos);
	}

	std::optional<ColorTheme> GameManager::TryAdvanceTurn(const std::string& gameStateID)
	{
		if (!ADVANCE_TURN) return std::nullopt;
		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		if (!IsValidGameState(maybeGameState, true, std::format("AdvanceTurn(id:{})", gameStateID))) return std::nullopt;

		std::optional<ColorTheme> maybeOtherPlayer = TryGetOtherPlayer(*maybeGameState);
		if (!maybeOtherPlayer.has_value())
		{
			const std::string error = std::format("Tried to change players turn in "
				"GameManager but there is empty player's turn! Current Player turn left unchanged: {}", 
				ToString(maybeGameState->CurrentPlayer));
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}

		maybeGameState->CurrentPlayer = maybeOtherPlayer.value();
		if (maybeGameState->InCheckmate || Board::GetAvailablePieces(*maybeGameState, maybeGameState->CurrentPlayer) == 0)
			EndGame(*maybeGameState);

		return maybeGameState->CurrentPlayer;
		//TurnChangeEvent.Invoke(maybeGameState->CurrentPlayer);
	}

	void GameManager::EndGame(GameState& state)
	{
		std::optional<ColorTheme> maybeOther= TryGetOtherPlayer(state);
		if (!maybeOther.has_value())
		{
			const std::string error = std::format("Tried to end the game in GameManager "
				"but there is no player's turn!");
			Utils::Log(Utils::LogType::Error, error);
			return;
		}

		//The player who did not cause game end is the one who wins
		//GameEndEvent.Invoke(EndGameInfo{maybeOther.value()});
	}

	size_t GameManager::TotalGameStatesCount() const
	{
		return _allGameStates.size();
	}

	void GameManager::AddEventCallback(const GameEventType& eventType, const GameEventCallbackType& callback)
	{
		if (eventType == GameEventType::PieceMoved) Board::AddPieceMoveCallback(callback);
		else if (eventType == GameEventType::SuccessfulTurn) Board::AddMoveExecutedCallback(callback);
		else
		{
			const std::string error = std::format("[GAME MANAGER]: Tried to add a game event callback"
				" for a type that has no event actions defined!");
			Utils::Log(Utils::LogType::Error, error);
			return;
		}
	}
}