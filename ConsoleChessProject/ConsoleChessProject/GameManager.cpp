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
		: m_allGameStates(), m_eventListeners{} //,GameStartEvent(), GameEndEvent(), TurnChangeEvent()
	{
		//Utils::Log(std::format("GAME MANAGER created Current game states: {}", std::to_string(TotalGameStatesCount())));
	}

	GameStateQueryResult GameManager::HasGameStateId(const std::string& gameStateId)
	{
		if (m_allGameStates.empty()) return {false, std::nullopt};

		GameStateCollectionType::iterator gameStateIt = m_allGameStates.find(gameStateId);
		bool hasState = gameStateIt != m_allGameStates.end();
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

	bool GameManager::IsValidGameState(const GameState* state, const std::string& operationName)
	{
		if (state == nullptr)
		{
			std::vector<std::string> ids = Utils::GetKeysFromMap<std::string, GameState>(m_allGameStates.begin(), m_allGameStates.end());
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
		Utils::Log(std::format("Size of game states: {}", std::to_string(m_allGameStates.size())));
		GameStateQueryResult newGameIdQuery = HasGameStateId(newGameStateID);
		if (newGameIdQuery.HasGameId)
		{
			const std::string error = std::format("[GAME_MANAGER]: Tried to start a new game with a game state "
				"id of: {} but this id already exists for a currently existing game state", newGameStateID);
			Utils::Log(Utils::LogType::Error, error);
			return {};
		}

		auto newStateIt= m_allGameStates.emplace(newGameStateID, GameState{});
		if (!newStateIt.second)
		{
			const std::string error = std::format("[GAME_MANAGER]: Tried to start a new game with a game state "
				"id of: {} but could not create the new game state", newGameStateID);
			Utils::Log(Utils::LogType::Error, error);
			return {};
		}

		GameState& newState = newStateIt.first->second;
		newState.CurrentPlayer = ColorTheme::Light;
		newState.TeamValue = { {ColorTheme::Light, 0}, {ColorTheme::Dark, 0}};
		Board::CreateDefaultBoard(newState);
		InvokeEvent(newState, GameEventType::StartGame);
		
		return newState;

		//Utils::Log(Utils::LogType::Warning, std::format("GAME_MANAGER: Start game: {}", newState.ToString()));

		//TODO: error arises when invoking
		//GameStartEvent.Invoke();
		
	}

	static ColorTheme GetOtherTeam(const ColorTheme color)
	{
		if (color == ColorTheme::Light) return ColorTheme::Dark;
		else return ColorTheme::Light;
	}

	ColorTheme GameManager::GetOtherPlayer(const GameState& state) const
	{
		return GetOtherTeam(state.CurrentPlayer);
	}

	bool GameManager::IsPositionWithinBounds(const Utils::Point2DInt& pos) const
	{
		return Board::IsWithinBounds(pos);
	}

	PieceMoveResult GameManager::TryMoveForState(const std::string& gameStateID,
		const Utils::Point2DInt& currentPos, const Utils::Point2DInt& newPos)
	{
		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		if (!IsValidGameState(maybeGameState, std::format("TryMove(id:{})", gameStateID)))
		{
			return { newPos, false, std::format("Failed to retrieve current game data") };
		}
		
		PieceMoveResult moveResult = Board::TryMove(*maybeGameState, currentPos, newPos);
		/*Utils::Log(std::format("MOVE INFO: game manager try move update peice all prev moves: {}",
			Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(maybeGameState->PreviousMoves.at(maybeGameState->CurrentPlayer))));*/

		if (moveResult.IsValidMove) InvokeEvent(*maybeGameState, GameEventType::PieceMoved);
		return moveResult;
	}

	std::vector<MoveInfo> GameManager::TryGetPossibleMovesForPieceAt(const std::string& gameStateID, const Utils::Point2DInt& pos)
	{
		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		//Utils::Log(std::format("Try get possible moves game manager has state: {}", std::to_string(maybeGameState!=nullptr)));
		if (!IsValidGameState(maybeGameState, std::format("GetPossibleMoves(id:{})", gameStateID)))
		{
			return {};
		}
		return Board::GetPossibleMovesForPieceAt(*maybeGameState, pos);
	}

	std::optional<MoveValueInfo> GameManager::TryCalculateLastMoveValue(const std::string& gameStateID, const ColorTheme color)
	{
		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		//Utils::Log(std::format("Try get possible moves game manager has state: {}", std::to_string(maybeGameState!=nullptr)));
		if (!IsValidGameState(maybeGameState, std::format("TryCalculateMoveValue(id:{})", gameStateID)))
		{
			return std::nullopt;
		}

		/*Utils::Log(std::format("MOVE INFO: Try calc last move for color: {} previous moves: {}", ToString(color),
			maybeGameState != nullptr ? Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(maybeGameState->PreviousMoves.at(color)) : "NULL"));*/

		auto colorMovesIt = maybeGameState->PreviousMoves.find(color);
		if (colorMovesIt == maybeGameState->PreviousMoves.end())
		{
			/*Utils::Log(Utils::LogType::Error, std::format("MOVE INFO: Previous moves for color not found {} ",
				ToString(color)));*/

			return std::nullopt;
		}
		if (colorMovesIt->second.empty())
		{
			/*Utils::Log(Utils::LogType::Error, std::format("MOVE INFO: Previous moves for color empty {}", ToString(color)));*/
			return std::nullopt;
		}

		/*Utils::Log(Utils::LogType::Error, std::format("CALLING CAL LAST MOVE VAL for color: {} move: {}",
			ToString(color),
			maybeGameState!=nullptr? std::to_string(maybeGameState->PreviousMoves.size()) : "NULL"));*/
			//return std::nullopt;

		const MoveInfo& moveInfo = colorMovesIt->second.at(colorMovesIt->second.size() - 1);
		std::unordered_map<ColorTheme, int> teamPoints = { {ColorTheme::Light, 0}, {ColorTheme::Dark, 0} };

		Utils::Log(Utils::LogType::Error, std::format("MOVE INFO: try calc last move for: {} value points moveinfo: {} ",
			ToString(color),
			moveInfo.ToString()));

		if (Utils::HasFlag(static_cast<unsigned int>(moveInfo.SpecialMoveFlags),
			static_cast<unsigned int>(SpecialMove::Capture)) && moveInfo.PieceCaptured!=nullptr)
		{
			int pieceValue = GetValueForPiece(moveInfo.PieceCaptured->m_PieceType);
			Utils::Log("CALC last move team points");
			teamPoints.at(moveInfo.PieceCaptured->m_Color) -= pieceValue;
			teamPoints.at(GetOtherTeam(moveInfo.PieceCaptured->m_Color)) += pieceValue;
		}

		return MoveValueInfo{ teamPoints };
	}

	std::optional<ColorTheme> GameManager::TryAdvanceTurn(const std::string& gameStateID)
	{
		if (!ADVANCE_TURN) return std::nullopt;

		GameState* maybeGameState = TryGetGameStateMutable(gameStateID);
		if (!IsValidGameState(maybeGameState, std::format("AdvanceTurn(id:{})", gameStateID))) return std::nullopt;

		std::optional<MoveValueInfo> maybeMoveVals= TryCalculateLastMoveValue(gameStateID, maybeGameState->CurrentPlayer);
		if (maybeMoveVals.has_value())
		{
			Utils::Log(std::format("CALC: adding points to team dark {} light {}", 
				std::to_string(maybeMoveVals.value().TeamPointDelta.at(ColorTheme::Dark)), 
				std::to_string(maybeMoveVals.value().TeamPointDelta.at(ColorTheme::Light))));

			maybeGameState->TeamValue[ColorTheme::Light] += maybeMoveVals.value().TeamPointDelta.at(ColorTheme::Light);
			maybeGameState->TeamValue[ColorTheme::Dark] += maybeMoveVals.value().TeamPointDelta.at(ColorTheme::Dark);
		}
		
		/*if (!maybeMoveVals.has_value())
		{
			const std::string error = std::format("Tried to change players turn in "
				"GameManager but the last move values for color: {} were not found",
				ToString(maybeGameState->CurrentPlayer));
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}
		maybeGameState->TeamValue[ColorTheme::Light] += maybeMoveVals.value().TeamPointDelta.at(ColorTheme::Light);
		maybeGameState->TeamValue[ColorTheme::Dark] += maybeMoveVals.value().TeamPointDelta.at(ColorTheme::Dark);*/

		ColorTheme otherPlayer = GetOtherPlayer(*maybeGameState);

		maybeGameState->CurrentPlayer = otherPlayer;
		InvokeEvent(*maybeGameState, GameEventType::SuccessfulTurn);
		if (maybeGameState->InCheckmate || Board::GetAvailablePieces(*maybeGameState, maybeGameState->CurrentPlayer) == 0)
			EndGame(*maybeGameState);

		return maybeGameState->CurrentPlayer;
		//TurnChangeEvent.Invoke(maybeGameState->CurrentPlayer);
	}

	void GameManager::EndGame(GameState& state)
	{
		std::optional<ColorTheme> maybeOther= GetOtherPlayer(state);
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
		return m_allGameStates.size();
	}

	void GameManager::AddEventCallback(const GameEventType& eventType, const GameEventCallbackType& callback)
	{
		auto existingListenersIt = m_eventListeners.find(eventType);
		if (existingListenersIt == m_eventListeners.end())
		{
			m_eventListeners.emplace(eventType, std::vector<GameEventCallbackType>{callback});
		}
		else
		{
			existingListenersIt->second.emplace_back(callback);
		}
	}

	std::unordered_map<ColorTheme, float> GameManager::CalculateWinPercentage(const GameState& state) const
	{
		if (state.TeamValue.at(ColorTheme::Dark) == state.TeamValue.at(ColorTheme::Light)) 
			return { {ColorTheme::Light, 0.5, }, {ColorTheme::Dark, 0.5} };

		ColorTheme smallerScoreColor = state.TeamValue.at(ColorTheme::Light) < state.TeamValue.at(ColorTheme::Dark) ?
										ColorTheme::Light : ColorTheme::Dark;
		ColorTheme greaterScoreColor= GetOtherTeam(smallerScoreColor);

		int smallestScore = state.TeamValue.at(smallerScoreColor);
		int smallestScoreDelta = 0;
		if (smallestScore < 0) smallestScoreDelta = std::abs(smallestScore) - smallestScore;

		int greaterScore = state.TeamValue.at(greaterScoreColor);

		std::unordered_map<ColorTheme, float> colorPercent = {};
		smallestScore += smallestScoreDelta;
		greaterScore += smallestScoreDelta;
		colorPercent.emplace(smallerScoreColor, (float)(smallestScore)/(smallestScore + greaterScore));
		colorPercent.emplace(greaterScoreColor, (float)(greaterScore)/(smallestScore + greaterScore));
		Utils::Log(Utils::LogType::Error, std::format("calc win percent: l:{} d:{} small: {} greater: {}", 
			std::to_string(colorPercent.at(ColorTheme::Light)), 
			std::to_string(colorPercent.at(ColorTheme::Dark)),
			std::to_string(smallestScore), 
			std::to_string(greaterScore)));
		return colorPercent; 
	}

	void GameManager::InvokeEvent(const GameState& state, const GameEventType gameEvent)
	{
		auto listenersIt = m_eventListeners.find(gameEvent);
		if (listenersIt == m_eventListeners.end() || listenersIt->second.empty()) return;
		for (const auto& listener : listenersIt->second)
		{
			listener(state);
		}
	}
}