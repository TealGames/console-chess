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
		GameStateCollectionType _allGameStates;

	public:
		static constexpr bool ADVANCE_TURN = true;

		//Utils::Event<void> GameStartEvent;
		//Utils::Event<void, EndGameInfo> GameEndEvent;
		//Utils::Event<void, const ColorTheme> TurnChangeEvent;

	private:
		//This has to not be const because we need to get mutable iterator
		GameStateQueryResult HasGameStateId(const std::string& gameStateId);
		bool IsValidGameState(const GameState* state, const bool logError, const std::string& operationName);
		GameState* TryGetGameStateMutable(const std::string& gameStateID);

		std::optional<ColorTheme> TryGetOtherPlayer(const GameState& state) const;
		
		void EndGame(GameState& state);

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

		void AddEventCallback(const GameEventType& eventType, const GameEventCallbackType& callback);
	};
}
