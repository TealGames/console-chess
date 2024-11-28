#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include <optional>
#include "Color.hpp"
#include "Event.hpp"
#include "GameState.hpp"
#include "BoardManager.hpp"
#include "Point2DInt.hpp"
#include "PieceMoveResult.hpp"

namespace Core
{
	using GameStateCollectionType = std::unordered_map<std::string, GameState>;

	struct EndGameInfo
	{
		const ColorTheme& WinningPlayer;
	};

	struct GameStateQueryResult
	{
		const bool HasGameId;
		std::optional<GameStateCollectionType::iterator> ResultIt;
	};

	using GameEventCallbackType = std::function<void(const GameState& state)>;
	enum class GameEventType
	{
		StartGame,
		PieceMoved,
		SuccessfulTurn,
	};

	struct MoveValueInfo
	{
		std::unordered_map<ColorTheme, int> TeamPointDelta;
	};
		
	class GameManager
	{
	private:
		/// <summary>
		/// Game state and their corresponding IDS
		/// </summary>
		GameStateCollectionType m_allGameStates;
		std::unordered_map<GameEventType, std::vector<GameEventCallbackType>> m_eventListeners;

	public:
		static constexpr bool ADVANCE_TURN = true;

		//Utils::Event<void> GameStartEvent;
		//Utils::Event<void, EndGameInfo> GameEndEvent;
		//Utils::Event<void, const ColorTheme> TurnChangeEvent;

	private:

		//This has to not be const because we need to get mutable iterator
		GameStateQueryResult HasGameStateId(const std::string& gameStateId);
		bool IsValidGameState(const GameState* state, const std::string& operationName);
		GameState* TryGetGameStateMutable(const std::string& gameStateID);

		ColorTheme GetOtherPlayer(const GameState& state) const;
		
		void EndGame(GameState& state);
		void InvokeEvent(const GameState& state, const GameEventType gameEvent);

	public:
		GameManager();
		const GameState* TryGetGameState(const std::string& gameStateID);
		const GameState& StartNewGame(const std::string& newGameStateID);
		
		bool IsPositionWithinBounds(const Utils::Point2DInt& pos) const;

		/// <summary>
		/// Makes the necesssary changes to the game state to advance turn
		/// and returns the new player's turn
		/// </summary>
		/// <param name="gameStateID"></param>
		/// <returns></returns>
		std::optional<ColorTheme> TryAdvanceTurn(const std::string& gameStateID);

		PieceMoveResult TryMoveForState(const std::string& gameStateID,
			const Utils::Point2DInt& currentPos, const Utils::Point2DInt& newPos);

		std::vector<MoveInfo> TryGetPossibleMovesForPieceAt(const std::string& gameStateID, const Utils::Point2DInt& pos);
		std::optional<MoveValueInfo> TryCalculateLastMoveValue(const std::string& gameStateID, const ColorTheme colorMoves);
		size_t TotalGameStatesCount() const;

		/// <summary>
		/// Will calculate winning percent as something like 99.9%
		/// </summary>
		/// <param name="state"></param>
		/// <param name="color"></param>
		/// <returns></returns>
		std::unordered_map<ColorTheme, float> CalculateWinPercentage(const GameState& state) const;

		void AddEventCallback(const GameEventType& eventType, const GameEventCallbackType& callback);
	};
}
