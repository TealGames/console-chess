#include <format>
#include <vector>
#include <optional>
#include <algorithm>
#include "BoardManager.hpp"
#include "BoardSetup.hpp"
#include "Point2DInt.hpp"
#include "Vector2D.hpp"
#include "Piece.hpp"
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"
#include "Globals.hpp"
#include "GameState.hpp"
#include "PieceMoveResult.hpp"

namespace Board
{
	/*using PiecePositionMapType = std::unordered_map<Utils::Point2DInt, Piece>;
	static PiecePositionMapType piecePositions;
	static std::unordered_map<ColorTheme, std::vector<MoveInfo>> previousMoves;

	static bool inCheckmate;
	static bool inCheck;*/
	static std::vector<Utils::Point2DInt> _allPossiblePositions;
	//static std::vector<PieceMoveCallbackType> _pieceMoveEvent;
	//static std::vector<MoveExecutedCallbackType> _successfulMoveEvent;

	/*static void InvokePieceMoveEvent(const GameState& state)
	{
		for (const auto& callback : _pieceMoveEvent)
		{
			callback(state);
		}
	}

	void AddPieceMoveCallback(const PieceMoveCallbackType& callback)
	{
		_pieceMoveEvent.push_back(callback);
	}

	static void InvokeSuccessfulMoveEvent(const GameState& state)
	{
		for (const auto& callback : _successfulMoveEvent)
		{
			callback(state);
		}
	}

	void AddMoveExecutedCallback(const MoveExecutedCallbackType& callback)
	{
		_successfulMoveEvent.push_back(callback);
	}*/

	static std::vector<Utils::Point2DInt> GetAllPositionsOnBoard()
	{
		if (!_allPossiblePositions.empty()) return _allPossiblePositions;

		for (int r = 0; r < BOARD_DIMENSION; r++)
		{
			for (int c = 0; c < BOARD_DIMENSION; c++)
			{
				_allPossiblePositions.emplace_back(r, c);
			}
		}
		std::sort(_allPossiblePositions.begin(), _allPossiblePositions.end());
		return _allPossiblePositions;
	}

	/*bool InCheck()
	{
		return inCheck;
	}

	bool InCheckmate()
	{
		return inCheckmate;
	}*/

	static std::vector<const Piece*> MakeImmutable(std::vector<Piece*> pieces)
	{
		std::vector<const Piece*> immutablePieces;
		for (const auto& piece : pieces)
		{
			immutablePieces.push_back(piece);
		}
		return immutablePieces;
	}

	/*struct PiecesRange
	{
		const std::vector<Utils::Point2DInt> keys;
		const std::vector<Piece*> values;
	};*/

	//static const PiecesRange GetPiecesForColorMutable(const GameState& state, const ColorTheme color)
	//{
	//	std::vector<Utils::Point2DInt> keys;
	//	std::vector<Piece*> values;

	//	//cant be a const iteration since we need to get a pointer to a not const value
	//	for (auto& piecePosition : state.PiecePositions)
	//	{
	//		if (piecePosition.second.color == color)
	//		{
	//			keys.push_back(piecePosition.first);
	//			values.push_back(&piecePosition.second);
	//		}
	//	}
	//	return { keys, values };
	//}

	/// <summary>
	///  Will get the piece at the specified position using .find for map (fast)
	/// Note: state is not modified but needs to be non-const to supply
	/// correct args to underlying function
	/// </summary>
	/// <param name="state"></param>
	/// <param name="pos"></param>
	/// <returns></returns>
	static Piece* TryGetPieceAtPositionMutable(GameState& state, const Utils::Point2DInt& pos)
	{
		PiecePositionMapType::iterator it = state.PiecePositions.find(pos);
		if (it == state.PiecePositions.end()) return nullptr;
		else return &(it->second);
	}

	const Piece* TryGetPieceAtPosition(const GameState& state, const Utils::Point2DInt& pos)
	{
		PiecePositionMapType::const_iterator it = state.PiecePositions.find(pos);
		if (it == state.PiecePositions.end()) return nullptr;
		else return &(it->second);
	}

	std::optional<Utils::Point2DInt> TryGetPositionOfPiece(const GameState& state, const Piece& piece)
	{
		for (const auto& piecePos : state.PiecePositions)
		{
			if (piecePos.second == piece)
			{
				return piecePos.first;
			}
		}
		return std::nullopt;
	}

	/*const std::unordered_map<Utils::Point2DInt, Piece>& GetAllPieces()
	{
		return piecePositions;
	}*/

	std::string GetPiecePositionsAsString(const GameState& state)
	{
		std::string boardRepresentation;
		std::string currentPieceStr;
		int row = 0;
		for (const auto& piecePos : state.PiecePositions)
		{
			currentPieceStr = std::format("[{}@ {}] ", piecePos.second.ToString(true), piecePos.first.ToString());
			boardRepresentation += currentPieceStr;
			row++;

			if (row >= BOARD_DIMENSION)
			{
				row = 0;
				boardRepresentation += "\n";
			}
		}
		return boardRepresentation;
	}


	bool HasPieceWithinPositionRange(const GameState& state, const Utils::Point2DInt& startPos, 
		const Utils::Point2DInt& endPos, bool inclusive)
	{
		Utils::Point2DInt currentPosition = startPos;
		Utils::Point2DInt endPosition = endPos;

		int xTotalDelta = endPosition.x - currentPosition.x;
		int yTotalDelta = endPosition.y - currentPosition.y;
		int xCurrentDelta = Utils::GetSign(xTotalDelta);
		int yCurrentDelta = Utils::GetSign(yTotalDelta);

		//If inclusive we step back so that the first forward would be the start pos
		if (inclusive) currentPosition = { currentPosition.x- xCurrentDelta, currentPosition.y - yCurrentDelta };
		else endPosition = { endPosition.x - xCurrentDelta, endPosition.y - yCurrentDelta };

		while (currentPosition != endPosition)
		{
			//If we go perfect diagonal we increase both
			if (std::abs(xTotalDelta) == std::abs(yTotalDelta))
			{
				xCurrentDelta = Utils::GetSign(xTotalDelta);
				yCurrentDelta = Utils::GetSign(yTotalDelta);
			}
			//Next if we have perfect vertical we only increase y
			else if (Utils::ApproximateEquals(currentPosition.x, endPosition.x))
			{
				xCurrentDelta = 0;
				yCurrentDelta = Utils::GetSign(yTotalDelta);
			}
			//If we have perfect horizontal we go x
			//OR if we have not uniformed diagonal, we first get x
			else
			{
				xCurrentDelta = Utils::GetSign(xTotalDelta);
				yCurrentDelta = 0;
			}

			currentPosition = { currentPosition.x + xCurrentDelta, currentPosition.y + yCurrentDelta };
			if (!IsWithinBounds(currentPosition)) continue;
			/*Utils::Log(std::format("Checking if piece in pos range: {} to {} xdelta: {} ydelta: {}, current pos check: {} has piece: {}", 
				startPos.ToString(), endPos.ToString(), std::to_string(xCurrentDelta), std::to_string(yCurrentDelta),
				currentPosition.ToString(), 
				std::to_string(TryGetPieceAtPosition(state, currentPosition) != nullptr)));*/
			
			if (TryGetPieceAtPosition(state, currentPosition) != nullptr) return true;

			if (currentPosition == endPosition) break;
		}
		return false;
	}

	/*std::vector<const Piece*> TryGetAvailablePieces(const GameState& state, const ColorTheme& color, const PieceType& type)
	{
		std::vector<const Piece*> foundPieces;
		for (const auto& piecePos : state.PiecePositions)
		{
			if (piecePos.second.state == Piece::State::InPlay &&
				piecePos.second.color == color && piecePos.second.pieceType == type)
				foundPieces.push_back(&piecePos.second);
		}
		return foundPieces;
	}*/

	std::vector<PiecePositionData> TryGetPiecesPosition(const GameState& state, const ColorTheme& color, const std::optional<PieceType>& type,
		const std::optional<std::vector<Piece::State>>& targetState)
	{
		std::vector<PiecePositionData> foundPieces;
		bool checkType = type.has_value();
		bool checkState = targetState.has_value() && targetState.value().size() > 0;
		for (const auto& piecePos : state.PiecePositions)
		{
			if (checkType && piecePos.second.m_PieceType != type.value()) continue;
			if (checkState)
			{
				bool pieceStateMatches = false;
				for (const auto& state : targetState.value())
				{
					if (piecePos.second.m_State == state)
					{
						pieceStateMatches = true;
						break;
					}
				}
				if (!pieceStateMatches) continue;
			}

			if (piecePos.second.m_Color == color)
				foundPieces.emplace_back(piecePos.second, piecePos.first);
		}
		return foundPieces;
	}

	/*std::vector<PiecePositionData> TryGetAvailablePiecesPosition(const GameState& state, const ColorTheme& color)
	{
		std::vector<PiecePositionData> positions;
		for (const auto& piecePos : state.PiecePositions)
		{
			if (!piecePos.second.state == Piece::State::InPlay &&
				piecePos.second.color == color) positions.emplace_back(piecePos.second, piecePos.first);
		}
		return positions;
	}*/

	size_t GetAvailablePieces(const GameState& state, ColorTheme& color)
	{
		return TryGetPiecesPosition(state, color, std::nullopt, std::vector<Piece::State>{ Piece::State::InPlay }).size();
	}

	static bool MoveInfoHasData(const std::vector<MoveInfo>& allMoveInfo, const std::function<bool(MoveInfo)> iterationCheck)
	{
		for (const auto& moveInfo : allMoveInfo)
		{
			if (iterationCheck(moveInfo)) return true;
		}
		return false;
	}

	static bool EraseMoveInfoWithData(std::vector<MoveInfo>& allMoveInfo, const std::function<bool(MoveInfo)>& iterationCheck)
	{
		int index = 0;
		for (const auto& moveInfo : allMoveInfo)
		{
			if (iterationCheck(moveInfo))
			{
				auto startIt = allMoveInfo.begin() + index;
				allMoveInfo.erase(startIt);
				return true;
			}
			index++;
		}
		return false;
	}

	//Will check if the position is within bounds of the board
	bool IsWithinBounds(const Utils::Point2DInt& pos)
	{
		if (pos.x < 0 || pos.x >= BOARD_DIMENSION) return false;
		else if (pos.y < 0 || pos.y >= BOARD_DIMENSION) return false;
		return true;
	}

	void ResetBoard(GameState& state)
	{
		/*std::array<Tile, BOARD_DIMENSION> emptyRow = {};
		tiles.fill(emptyRow);*/
		state.PiecePositions.clear();
	}

	static Piece CreatePiece(const ColorTheme& color, const PieceType& pieceType)
	{
		return { color, pieceType };
	}

	//void InitPieces()
	//{
	//	int r = 0;
	//	int c = 0;
	//	
	//	for (int i = 0; i < 2; i++)
	//	{
	//		ColorTheme currentColor = i == 0 ? ColorTheme::Light : ColorTheme::Dark;
	//		if (c >= BOARD_DIMENSION)
	//		{
	//			r++;
	//			c = 0;
	//		}
	//			
	//		for (const auto& pieceType : ALL_PIECE_TYPES)
	//		{
	//			piecePositions.insert({ Utils::Point2DInt(r, c), CreatePiece(currentColor, pieceType)});
	//			c++;
	//
	//			Utils::Log(Utils::LogType::Log, std::format("Adding: {} for pos: {} {} size: {}", ToString(pieceType), 
	//				std::to_string(r),std::to_string(c), piecePositions.size()));
	//
	//			if (c >= BOARD_DIMENSION)
	//			{
	//				r++;
	//				c = 0;
	//			}
	//		}
	//	}
	//}

	struct CheckOrMateResult
	{
		bool IsCheckmate;
		bool IsCheck;
	};

	/// <summary>
	/// Will return a result whether the game state has a piece at posAttemptingCheck
	/// for a check (attack to king) or checkmate (king can not excape direct attack)
	/// </summary>
	/// <param name="state"></param>
	/// <param name="posAttemptingCheck"></param>
	/// <returns></returns>
	static CheckOrMateResult IsCheckOrMateByPieceAt(const GameState& state, const Utils::Point2DInt& posAttemptingCheck)
	{
		const Piece* pieceAttemptingCheck = TryGetPieceAtPosition(state, posAttemptingCheck);

		//Standard chess rules would prohibit a king check or checkmating another king since a king
		//would then be in attack range of another king, meaning your king would not be able to move
		//there since then your king is at risk of check or mate
		if (pieceAttemptingCheck->m_PieceType == PieceType::King) return { false, false };

		const std::vector<MoveInfo> possibleMoves = GetPossibleMovesForPieceAt(state, posAttemptingCheck);

		const Piece* pieceChecked = nullptr;
		bool inCheckResult = false;
		if (pieceAttemptingCheck != nullptr)
		{
			for (const auto& possibleMove : possibleMoves)
			{
				for (const auto& pieceMove : possibleMove.PiecesMoved)
				{
					//If this move is not for this piece we do not consider it
					if (&pieceMove.PieceRef != pieceAttemptingCheck) continue;

					//If the piece at the end of the path is a king of a different color it is a check
					pieceChecked = TryGetPieceAtPosition(state, pieceMove.NewPos);
					if (pieceChecked != nullptr && pieceChecked->m_PieceType == PieceType::King &&
						pieceChecked->m_Color != pieceAttemptingCheck->m_Color)
					{
						inCheckResult = true;
						break;
					}
				}
			}
		}

		bool inCheckmateResult = false;
		if (inCheckResult)
		{
			std::vector<PiecePositionData> kingPositions = TryGetPiecesPosition(state, pieceAttemptingCheck->m_Color ==
				ColorTheme::Dark ? ColorTheme::Light : ColorTheme::Dark, PieceType::King,
				std::vector<Piece::State>{ Piece::State::InPlay });
			if (kingPositions.size() != 1)
			{
				std::string error = std::format("Tried to check for check and checkmate but no king was found!");
				Utils::Log(Utils::LogType::Error, error);
				return { false, false };
			}
			const std::vector<MoveInfo> kingMoves = GetPossibleMovesForPieceAt(state, kingPositions[0].Pos);
			inCheckmateResult = kingMoves.size() == 0;
		}

		return { inCheckmateResult, inCheckResult };
	}

	/// <summary>
	/// Will check if it is a check or checkmate and then update the state. Returns true if it was updated
	/// </summary>
	static bool UpdateCheckOrMate(GameState& state, const Utils::Point2DInt& posAttemptingCheck)
	{
		CheckOrMateResult result = IsCheckOrMateByPieceAt(state, posAttemptingCheck);
		if (result.IsCheck == state.InCheck && result.IsCheckmate == state.InCheckmate) return false;

		state.InCheck = result.IsCheck;
		state.InCheckmate = result.IsCheckmate;
		return true;
	}

	/// <summary>
	/// Will get all the positions that a check or checkmate can be done from 
	/// (AKA a piece is there and can check or checkmate the opposing king)
	/// </summary>
	/// <param name="state"></param>
	/// <param name="color"></param>
	/// <returns></returns>
	static std::vector<PiecePositionData> GetPiecePositionsForcingCheckOrMate(const GameState& state, const ColorTheme& color)
	{
		std::vector<PiecePositionData> checkablePiecePositions;
		for (const auto& piece : state.PiecePositions)
		{
			if (piece.second.m_Color != color) continue;
			CheckOrMateResult result = IsCheckOrMateByPieceAt(state, piece.first);
			if (result.IsCheck || result.IsCheckmate)
				checkablePiecePositions.emplace_back(piece.second, piece.first);
		}

		return checkablePiecePositions;
	}

	static bool TryUpdatePiecePosition(GameState& state, const PiecePositionData& currentData, const Utils::Point2DInt& newPos)
	{
		Utils::Log(std::format("PIECE CHECK BEFORE TRY MOVE {} -> {} found piece: {}", currentData.Pos.ToString(),
			newPos.ToString(), currentData.PieceRef.ToString()));

		if (!TryGetPieceAtPosition(state, currentData.Pos))
		{
			std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
				"but it is not located there", currentData.PieceRef.ToString(), currentData.Pos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return false;
		}

		if (!IsWithinBounds(currentData.Pos))
		{
			std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
				"but current pos is not valid", currentData.PieceRef.ToString(), currentData.Pos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return false;
		}

		if (!IsWithinBounds(newPos))
		{
			std::string error = std::format("Tried to update pos of piece: {} from pos: {} "
				"to pos: {} but new pos is not valid", currentData.PieceRef.ToString(), currentData.Pos.ToString(), newPos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return false;
		}

		std::function<void()> undoCaptureFunc = nullptr;

		//We then have to first check for the piece at new pos before removing to add to captured
		Piece* pieceAtNewPos = TryGetPieceAtPositionMutable(state, newPos);
		if (pieceAtNewPos != nullptr)
		{
			if (pieceAtNewPos->m_Color == currentData.PieceRef.m_Color)
			{
				std::string error = std::format("Tried to place board piece: {} at pos: {} "
					"but piece of same team color exists at new pos: {}",
					currentData.PieceRef.ToString(), newPos.ToString(), currentData.PieceRef.ToString());
				Utils::Log(Utils::LogType::Error, error);
				return false;
			}

			//We make a copy so that we can reuse piece data if we need to undo
			Piece capturedPieceCopy = *pieceAtNewPos;
			pieceAtNewPos->UpdateState(Piece::State::Captured);
			state.CapturedPieces.push_back(capturedPieceCopy);
			state.PiecePositions.erase(newPos);

			undoCaptureFunc = [newPos, &capturedPieceCopy, &state]() -> void
				{
					capturedPieceCopy.UpdateState(Piece::State::InPlay);
					state.CapturedPieces.pop_back(); 
					state.PiecePositions.emplace(newPos, capturedPieceCopy);
				};
		}

		auto pieceMovedIt = state.PiecePositions.find(currentData.Pos);
		bool hasPiece = pieceMovedIt!= state.PiecePositions.end();
		//Note: since we delete the old pos and move to new pos, we have to ensure we copy the data
		//otherwise using the pieceref and ptrs will not work since they are referencing data that will be deleted
		//(and most likely due to them being made from the collection, will point to a different wrong piece)
		
		if (hasPiece)
		{
			Piece movedPieceCopy = pieceMovedIt->second;
			size_t elementsErased= state.PiecePositions.erase(currentData.Pos);
			if (elementsErased == 0)
			{
				std::string error = std::format("Tried to move board piece: {} at pos: {}"
					"but failed to retrieve it and/or remove it from its old position {}",
					currentData.PieceRef.ToString(), newPos.ToString(), currentData.Pos.ToString());
				Utils::Log(Utils::LogType::Error, error);

				if (undoCaptureFunc != nullptr) undoCaptureFunc();
				state.PiecePositions.emplace(currentData.Pos, movedPieceCopy);
				return false;
			}

			state.PiecePositions.emplace(newPos, movedPieceCopy);
			//InvokePieceMoveEvent(state);
		}
		else
		{
			std::string error = std::format("Tried to move board piece: {} at pos: {}"
				"but there are no pieces at that position: {}",
				currentData.PieceRef.ToString(), newPos.ToString(), currentData.Pos.ToString());
			Utils::Log(Utils::LogType::Error, error);

			if (undoCaptureFunc != nullptr) undoCaptureFunc();
			return false;
		}
		PiecePositionMapType oldPos = state.PiecePositions;
		std::vector<Piece> oldCaptured = state.CapturedPieces;

		Utils::Log(std::format("PIECE CHECK TRY MOVE {} -> {} found piece: {}", currentData.Pos.ToString(), newPos.ToString(), currentData.PieceRef.ToString()));
		Utils::Log(std::format("PIECE CHECK Updated piece position of {} from {} -> {}. Old:[pos:{}, captured:{}] New:[pos:{}, captured:{}],",
			currentData.PieceRef.ToString(), currentData.Pos.ToString(), newPos.ToString(), 
			Utils::ToStringIterable(oldPos), Utils::ToStringIterable<std::vector<Piece>, Piece>(oldCaptured), 
			Utils::ToStringIterable(state.PiecePositions), Utils::ToStringIterable<std::vector<Piece>, Piece>(state.CapturedPieces)));
		return true;
	}

	static bool TryUpdatePiecePosition(GameState& state, const PiecePositionData currentData, Utils::Point2DInt newPos, MoveInfo moveInfo)
	{
		if (!TryUpdatePiecePosition(state, currentData, newPos)) return false;

		//Utils::Log(std::format("POOPY DOPZY Adding previous moves with move info: {}", moveInfo.ToString()));
		ColorTheme color = currentData.PieceRef.m_Color;
		if (state.PreviousMoves.find(color) == state.PreviousMoves.end())
		{
			state.PreviousMoves.emplace(color, std::vector<MoveInfo>{moveInfo });
			Utils::Log(std::format("MOVE INFO: ADDING MOVE INFO {} all prev moves: {}", 
				moveInfo.ToString(), Utils::ToStringIterable<std::vector<MoveInfo>,MoveInfo>(state.PreviousMoves.at(color))));
		}
		else
		{
			state.PreviousMoves.at(color).push_back(moveInfo);
			Utils::Log(std::format("MOVE INFO: ADDING MOVE INFO {} all prev moves: {}",
				moveInfo.ToString(), Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(state.PreviousMoves.at(color))));
		}
		return true;
	}

	static std::optional<Piece*> TryCreatePieceAtPos(GameState& state, const ColorTheme& color,
		const PieceType& pieceType, const Utils::Point2DInt& pos)
	{
		if (!IsWithinBounds(pos))
		{
			std::string error = std::format("Tried to create piece: {} {} at pos: {} "
				"but pos is not valid", ToString(color), ToString(pieceType), pos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}

		const Piece* pieceAtTargetPos = TryGetPieceAtPosition(state, pos);
		if (pieceAtTargetPos != nullptr)
		{
			std::string error = std::format("Tried to create piece: {} {} at pos: {} "
				"but piece already exists there!", ToString(color), ToString(pieceType), pos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}

		auto maybePairCreated = state.PiecePositions.emplace(pos, CreatePiece(color, pieceType));
		if (!maybePairCreated.second)
		{
			std::string error = std::format("Tried to create piece: {} {} at pos: {} "
				"but failed to be inserted there!", ToString(color), ToString(pieceType), pos.ToString());
			Utils::Log(Utils::LogType::Error, error);
			return std::nullopt;
		}

		Piece* pieceCreated = &(maybePairCreated.first->second);
		Utils::Log(Utils::LogType::Log, std::format("Creating piece {}", pieceCreated->ToString()));
		return pieceCreated;
		//return std::nullopt;
	}

	//Will place all pieces from default board positions. If the peice exists it will be moved
	//otherwise it will be created and moved
	//NOTE: this func assumes that it is in a default state where pieces are added without considering
	//if those exist since there may be duplicates of peices on default baords and it would mess with it
	static void PlaceDefaultBoardPieces(GameState& state)
	{
		int pieceIndex = 0;
		std::vector<Piece*> allPiecesCreatedOrUpdated;
		for (const auto& initPiecePos : GetDefaultBoardPiecePositions())
		{

			if (!IsWithinBounds(initPiecePos.NewPos))
			{
				std::string error = std::format("Tried to place default board piece: {} at pos: {} "
					"but is invalid pos", ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString());
				Utils::Log(Utils::LogType::Error, error);
				return;
			}
			pieceIndex++;
			Piece* movePiece = nullptr;
			Utils::Point2DInt movePiecePos;

			//We need to look for only pieces that are not undefined (since undefined will be newly created peices
			//and if we need multiple of the same type we dont want to move created pieces) which are ones that were in play
			std::vector<PiecePositionData> availablePieces = TryGetPiecesPosition(state, initPiecePos.Color, initPiecePos.PieceType,
				std::vector<Piece::State>{ Piece::State::Captured, Piece::State::InPlay });
			/*Utils::Log(Utils::LogType::Warning, std::format("For piece {} {} for pos: {} found existing: {}",
				ToString(initPiecePos.Color), ToString(initPiecePos.PieceType),
				initPiecePos.NewPos.ToString(), std::to_string(availablePieces.size())));*/

			if (!availablePieces.empty())
			{
				//We can just take the top one at index 0 since we will update their state anyways so they just dwindle
				//down if we keep updating for the same piece
				Utils::Point2DInt nextAvailablePiecePos = availablePieces[0].Pos;
				movePiecePos = Utils::Point2DInt(nextAvailablePiecePos);

				//We can't just use the ref from available pieces since it is const
				movePiece = TryGetPieceAtPositionMutable(state, movePiecePos);
				if (movePiece == nullptr)
				{
					std::string error = std::format("Tried to place default board piece: {} at pos: {} "
						"but failed to retrieve its current pos:",
						ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString(), nextAvailablePiecePos.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return;
				}
			}
			else
			{

				std::optional<Piece*> maybePieceAtPos =
					TryCreatePieceAtPos(state, initPiecePos.Color, initPiecePos.PieceType, initPiecePos.NewPos);
				if (!maybePieceAtPos.has_value())
				{
					std::string error = std::format("Tried to make default board and create piece: {} {} at pos: {} "
						"but it failed to create it there!",
						ToString(initPiecePos.Color), ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return;
				}

				movePiecePos = Utils::Point2DInt(initPiecePos.NewPos);
				if (maybePieceAtPos.value() == nullptr)
				{
					Utils::Log(Utils::LogType::Error, "Newly created piece is NULL");
					return;
				}
				movePiece = maybePieceAtPos.value();
				//Utils::Log(Utils::LogType::Error, std::format("Move Hpcie created: {}", movePiece->ToString()));
			}

			if (movePiece == nullptr)
			{
				std::string error = std::format("Tried to place default board piece: {} {} at pos: {} "
					"but and it failed to be retrieved or created",
					ToString(initPiecePos.Color), ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString());
				Utils::Log(Utils::LogType::Error, error);
				return;
			}
			allPiecesCreatedOrUpdated.push_back(movePiece);

			//movePiece->UpdateState(Piece::State::InPlay);

			//In order to preserve available positions we add the one we changed from and remove the new one
			//availablePositions.erase(initPiecePos.NewPos);
			//if (movePiecePos!=initPiecePos.NewPos) availablePositions.insert(movePiecePos);

			/*Utils::Log(Utils::LogType::Log, std::format("ADDING piece: {} to pos: {}",
				movePiece->ToString(), initPiecePos.NewPos.ToString()));*/
			/*Utils::Log(Utils::LogType::Log, std::format("ADDING piece: {} to pos: {} available pos left: {}",
				movePiece->ToString(), initPiecePos.NewPos.ToString(), Utils::ToStringIterable<decltype(availablePositions), Utils::Point2DInt>(availablePositions)));*/
		}

		//We update each piece so if they were created or captured we change
		for (const auto& piece : allPiecesCreatedOrUpdated)
		{
			piece->UpdateState(Piece::State::InPlay);
		}
	}

	void CreateDefaultBoard(GameState& state)
	{
		ResetBoard(state);
		//if (piecePositions.empty()) InitPieces();

		PlaceDefaultBoardPieces(state);
	}

	struct CastleInfo
	{
		const bool canCastle;
		const bool isKingSide;
		const bool isQueenSide;

		const Utils::Point2DInt kingSideCastleMove;
		const Utils::Point2DInt queenSideCastleMove;
	};

	/// <summary>
	/// Castleing occurs when the king and rook have not moved yet during the game
	/// the king is not in check, and there are no pieces between rook and king
	/// </summary>
	/// <param name="state"></param>
	/// <param name="color"></param>
	/// <returns></returns>
	static CastleInfo CanCastle(const GameState& state, const ColorTheme& color)
	{
		if (state.InCheck || state.InCheckmate) return { false, false, false };

		//Note: we dont want to check rook because it would check any rook moved and 
		//we might still be able to castle on one side and not on the other
		if (HasMovedPiece(state, color, PieceType::King)) return { false, false, false };
		/*const std::vector<PiecePositionData> rooks = TryGetPiecesPosition(state, color, PieceType::Rook,
			std::vector<Piece::State>{ Piece::State::InPlay });*/

		//TODO: optimize these calls so they are batched together to find pos for both rook and king at the same time
		const std::vector<PiecePositionData> kingPositions = TryGetPiecesPosition(state, color, PieceType::King,
			std::vector<Piece::State>{ Piece::State::InPlay });
		if (kingPositions.empty()) return { false, false, false };
		Utils::Point2DInt kingPos = kingPositions[0].Pos;

		const std::vector<PiecePositionData> rookPositions = TryGetPiecesPosition(state, color, PieceType::Rook,
			std::vector < Piece::State> { Piece::State::InPlay });
		if (rookPositions.empty()) return { false, false, false };

		bool canKingSide = false;
		bool canQueenSide = false;
		bool checkingKingSide = false;
		int spacesMissing = 0;
		Utils::Point2DInt currentPosCheck;
		for (const auto& rookPos : rookPositions)
		{
			if (rookPos.Pos.x != kingPos.x) continue;

			spacesMissing = 0;
			currentPosCheck = kingPos;
			checkingKingSide = rookPos.Pos.y > currentPosCheck.y ? true : false;
			while ((checkingKingSide && currentPosCheck.y >= rookPos.Pos.y) || 
					(!checkingKingSide && currentPosCheck.y <=rookPos.Pos.y))
			{
				//King side goes up the cols queen side goes down
				if (checkingKingSide) currentPosCheck = { currentPosCheck.x, currentPosCheck.y + 1 };
				else currentPosCheck = { currentPosCheck.x, currentPosCheck.y - 1 };

				//Since we do the decrement/incremenet above, it can check to rook pos after loop condition
				if ((checkingKingSide && currentPosCheck.y >= rookPos.Pos.y) ||
					(!checkingKingSide && currentPosCheck.y <= rookPos.Pos.y)) break;

				if (TryGetPieceAtPosition(state, currentPosCheck)==nullptr)
				{
					spacesMissing++;
				}
				//If there is a piece here we can just exit early
				else break;
			}

			//If the spaces between rook and king are all empty it means we can castle
			if (spacesMissing == std::abs(rookPos.Pos.y - kingPos.y) - 1)
			{
				if (checkingKingSide) canKingSide = true;
				else canQueenSide = true;
			}
		}

		Utils::Point2DInt kingSideCastleMove = canKingSide ? Utils::Point2DInt{ kingPos.x, kingPos.y + 2 } : Utils::Point2DInt{};
		Utils::Point2DInt queenSideCastleMove = canQueenSide ? Utils::Point2DInt{ kingPos.x, kingPos.y - 2 } : Utils::Point2DInt{};
		Utils::Log(std::format("King side move: {} queen side: {} kingPos: {}", 
			kingSideCastleMove.ToString(), queenSideCastleMove.ToString(), kingPos.ToString()));
		return { canKingSide || canQueenSide, canKingSide, canQueenSide, kingSideCastleMove, queenSideCastleMove };
	}
	static CastleInfo IsCastleMove(const GameState& state, const PiecePositionData currentData, const Utils::Point2DInt& newPos)
	{
		if (currentData.PieceRef.m_PieceType != PieceType::King) return { false, false, false };

		int startCol = std::min(currentData.Pos.y, newPos.y);
		int endCol = std::max(currentData.Pos.y, newPos.y);
		int delta = endCol - startCol;

		const Piece* outPiece;
		int rookCol = delta > 0 ? BOARD_DIMENSION - 1 : 0;
		//If in the direction moved is NOT a rook at the end, it means we do not have castle chance
		if ((outPiece = TryGetPieceAtPosition(state, { currentData.Pos.x , rookCol })) == nullptr) return { false, false, false };
		if (outPiece == nullptr || outPiece->m_PieceType != PieceType::Rook) return { false, false, false };

		//If we move kingside it is 3, otherwise it is 4 diff
		if (std::abs(delta) != 3 && std::abs(delta) != 4) return { false, false, false };

		for (int i = startCol + 1; i <= endCol - 1; i++)
		{
			//If there is a piece in the way it means we cannot castle
			if ((outPiece = TryGetPieceAtPosition(state,{ currentData.Pos.x, i })) != nullptr)
				return { false, false, false };
		}
		//Delta >0 means moves up, <0 means moves down queen side
		return { true, delta > 0, delta < 0 };
	}

	static const Piece* TryGetCapturePiece(const GameState& state, const PiecePositionData currentData, const Utils::Point2DInt newPos)
	{
		const Piece* pieceAtNewPos = nullptr;
		if ((pieceAtNewPos = TryGetPieceAtPosition(state, newPos)) == nullptr) return nullptr;

		if (!DoesMoveDeltaMatchCaptureMoves(currentData.PieceRef.m_PieceType, currentData.Pos, newPos))
			return nullptr;

		//TODO: are there any other checks to capture a piece
		return pieceAtNewPos;
	}
	static bool IsCapture(const GameState& state, const PiecePositionData currentData, const Utils::Point2DInt newPos)
	{
		return TryGetCapturePiece(state, currentData, newPos) != nullptr;
	}

	std::vector<MoveInfo> GetPossibleMovesForPieceAt(const GameState& state, const Utils::Point2DInt& startPos)
	{
		if (!IsWithinBounds(startPos))
		{
			Utils::Log("Out of bounds");
			return {};
		}
			
		const Piece* movedPiece = nullptr;
		if ((movedPiece = TryGetPieceAtPosition(state, startPos)) == nullptr)
		{
			Utils::Log("Not found");
			return {};
		}

		std::vector<MoveInfo> possibleMoves;
		const auto isDuplicatePos = [&possibleMoves](const Utils::Point2DInt& pos) -> bool
			{
				for (const auto& moves : possibleMoves)
				{
					for (const auto& movePos : moves.PiecesMoved)
					{
						if (movePos.NewPos == pos) return true;
					}
				}
				return false;
			};

		Utils::Point2DInt moveNewPos;
		
		Utils::Log(std::format("MOVE {} DIRS: {}", std::to_string(GetMoveDirsForPiece(movedPiece->m_Color, movedPiece->m_PieceType).size()),
			Utils::ToStringIterable<std::vector<Utils::Vector2D>, Utils::Vector2D>(GetMoveDirsForPiece(movedPiece->m_Color, movedPiece->m_PieceType))));

		const std::vector<Utils::Vector2D> moveDirs = GetMoveDirsForPiece(movedPiece->m_Color, movedPiece->m_PieceType);
		const Piece* pieceAtNewPos = nullptr;
		for (auto& movePos : moveDirs)
		{
			moveNewPos = GetVectorEndPoint(startPos, movePos);
			pieceAtNewPos = TryGetPieceAtPosition(state, moveNewPos);

			
			//TODO: this is an issue with this if a piece can move any amount in a direction since they then might be able to 
			//go though pieces of different opposing color
			bool isNoPieceOrOpposingPieceAtNewPos = pieceAtNewPos == nullptr || pieceAtNewPos->m_Color != movedPiece->m_Color;
			bool canMoveOverOthers = CanPieceMoveOverPieces(movedPiece->m_PieceType);
			bool moveOverPiecesFollowsRules = canMoveOverOthers || 
											 (!canMoveOverOthers && !HasPieceWithinPositionRange(state, startPos, moveNewPos, false));

			Utils::Log(std::format("Is {} (start: {} new pos: {}) within bounds in bounds: {} piece in range: {} "
				"dupliate: {} new pos rules: {} move over rules: {}",
				startPos.ToString(), movePos.ToString(Utils::Vector2D::VectorForm::Component), moveNewPos.ToString(),
				std::to_string(IsWithinBounds(moveNewPos)),
				std::to_string(HasPieceWithinPositionRange(state, startPos, moveNewPos, false)),
				std::to_string(isDuplicatePos(moveNewPos)),
				std::to_string(isNoPieceOrOpposingPieceAtNewPos), std::to_string(moveOverPiecesFollowsRules)));


			if (IsWithinBounds(moveNewPos) && !isDuplicatePos(moveNewPos) && 
				isNoPieceOrOpposingPieceAtNewPos && moveOverPiecesFollowsRules)
			{
				//We have to check if it is a capture because capture moves might not be different
				//from move dirs so we might capture during regualar moves
				//and we can save some time by first making sure if there actually is a piece at the new pos
				SpecialMove specialMove = SpecialMove::None;
				const Piece* capturePiece = nullptr;
				std::optional<PieceTypeInfo> capturedPieceValue = std::nullopt;
				if (pieceAtNewPos != nullptr)
				{
					capturePiece = TryGetCapturePiece(state, PiecePositionData{ *movedPiece, startPos }, moveNewPos);
					specialMove= (capturePiece != nullptr) ? SpecialMove::Capture : SpecialMove::None;
					if (capturePiece != nullptr) capturedPieceValue = PieceTypeInfo(capturePiece->m_Color, capturePiece->m_PieceType);
				}

				possibleMoves.emplace_back(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, moveNewPos)
				},
					"",
					specialMove,
					std::nullopt,
					capturedPieceValue,
					false,
					false
					);

				/*Utils::Log(std::format("RETURNING MOVES:{}",
					Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(possibleMoves)));*/
			}
		}
		
		const std::vector<Utils::Vector2D> captureMoves = GetCaptureMovesForPiece(movedPiece->m_Color, movedPiece->m_PieceType);
		Utils::Log(Utils::LogType::Error, std::format("POSSIBLE {} {} CAPTURES: {}",
			ToString(movedPiece->m_PieceType), std::to_string(possibleMoves.size()), 
			Utils::ToStringIterable<std::vector<Utils::Vector2D>, Utils::Vector2D>(captureMoves)));

		//Since capture moves return the same as move dirs if there are no special capture dirs
		//there is no point repeating those moves if we already checking them for moving (and also check for capturing)
		if (captureMoves != moveDirs)
		{
			for (auto& captureMove : captureMoves)
			{
				moveNewPos = GetVectorEndPoint(startPos, captureMove);
				pieceAtNewPos = TryGetPieceAtPosition(state, moveNewPos);

				bool isOpposingPieceAtNewPos = pieceAtNewPos != nullptr && pieceAtNewPos->m_Color != movedPiece->m_Color;
				if (IsWithinBounds(moveNewPos) && isOpposingPieceAtNewPos &&
					!isDuplicatePos(moveNewPos) && !HasPieceWithinPositionRange(state, startPos, moveNewPos, false))
				{
					possibleMoves.emplace_back(
						std::vector<MovePiecePositionData>
					{
						MovePiecePositionData(*movedPiece, startPos, moveNewPos)
					},
						"",
						SpecialMove::Capture,
						std::nullopt,
						PieceTypeInfo(pieceAtNewPos->m_Color, pieceAtNewPos->m_PieceType),
						false,
						false
						);
				}
			}
		}

		CastleInfo castleInfo = CanCastle(state, movedPiece->m_Color);
		Utils::Log(std::format("Checking {} castle: {} [k:{} move: {}] [q:{} move:{}]", ToString(movedPiece->m_Color),
			std::to_string(castleInfo.canCastle),
			std::to_string(castleInfo.isKingSide), castleInfo.kingSideCastleMove.ToString(),
			std::to_string(castleInfo.isQueenSide), castleInfo.queenSideCastleMove.ToString()));

		if (movedPiece->m_PieceType == PieceType::King && castleInfo.canCastle)
		{
			if (castleInfo.isKingSide && !isDuplicatePos(castleInfo.kingSideCastleMove))
			{
				possibleMoves.emplace_back(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, castleInfo.kingSideCastleMove)
				},
					"",
					SpecialMove::KingSideCastle,
					std::nullopt,
					std::nullopt,
					false,
					false
				);
			}
			else if (castleInfo.isQueenSide && !isDuplicatePos(castleInfo.queenSideCastleMove))
			{
				possibleMoves.emplace_back(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, castleInfo.queenSideCastleMove)
				},
					"",
					SpecialMove::QueenSideCastle,
					std::nullopt,
					std::nullopt,
					false,
					false
				);
			}
			else
			{
				std::string error = std::format("Tried to get moves for piece {} at pos: {} "
					"and was checking for castleing, which resulted in TRUE, but found no side to castle!",
					movedPiece->ToString(), startPos.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return {};
			}
		}

		if (movedPiece->m_PieceType == PieceType::King)
		{
			//Kings also get checked for any positions that are able to be checked
			//and those positions are illegal and are removed
			std::vector<PiecePositionData> checkPositions = GetPiecePositionsForcingCheckOrMate(state, movedPiece->m_Color);
			if (checkPositions.size() != 0)
			{
				for (const auto& checkPos : checkPositions)
				{
					EraseMoveInfoWithData(possibleMoves, [&checkPos, movedPiece](MoveInfo info) -> bool
						{
							for (const auto& move : info.PiecesMoved)
							{
								if (&move.PieceRef != movedPiece) continue;
								if (move.NewPos == checkPos.Pos) return true;
							}
							return false;
						});
				}
			}
		}
		
		Utils::Log(std::format("When getting all possible moves for piece: {} found: {}", movedPiece->ToString(),
			Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(possibleMoves)));
		return possibleMoves;
	}

	//Will check if it is possible to move to that point using a variety of bounds checks,
	//valid moves, and special move checks
	PieceMoveResult TryMove(GameState& state, const Utils::Point2DInt& currentPos, const Utils::Point2DInt& newPos)
	{
		if (!IsWithinBounds(currentPos))
			return { newPos, false, std::format("Tried to move from a place outside the board") };

		const Piece* movedPiece = nullptr;
		if ((movedPiece = TryGetPieceAtPosition(state, currentPos)) == nullptr)
			return { newPos, false, std::format("Position ({}) has no piece", newPos.ToString()) };

		if (!IsWithinBounds(newPos))
			return { newPos, false, std::format("Tried to move to a place outside the board") };

		const std::vector<MoveInfo> possibleMoves = GetPossibleMovesForPieceAt(state, currentPos);
		if (possibleMoves.empty())
			return { newPos, false, std::format("There are no possible moves for this piece") };

		for (const auto& moveInfo : possibleMoves)
		{
			for (const auto& move : moveInfo.PiecesMoved)
			{
				//If the piece moved for this move is not the one we want we skip
				if (&move.PieceRef != movedPiece) continue;

				if (move.NewPos == newPos)
				{
					//Utils::Log(std::format("Trying to update piece position to {} with mvoe info: {}", move.NewPos.ToString(), moveInfo.ToString()));
					TryUpdatePiecePosition(state, PiecePositionData{ *movedPiece, currentPos }, newPos, moveInfo);
					/*Utils::Log(std::format("MOVE INFO: after update peice {} all prev moves: {}",
						moveInfo.ToString(), Utils::ToStringIterable<std::vector<MoveInfo>, MoveInfo>(state.PreviousMoves.at(movedPiece->m_Color))));*/
					//InvokeSuccessfulMoveEvent(state);
					return { newPos, true };
				}
			}
		}
		return { newPos, false, std::format("New pos does not match any pos for this piece") };
	}

	std::string CleanInput(const std::string& input)
	{
		std::string cleaned = Utils::StringUtil(input).Trim().RemoveChar(' ').ToString();
		return cleaned;
	}

	//std::optional<MoveInfo> TryParseMoveInfoFromMove(const ColorTheme& color, const std::string& input)
	//{
	//	std::string cleanedInput = CleanInput(input);
	//	if (cleanedInput == NOTATION_KINGSIDE_CASTLE)
	//	{
	//		MoveInfo kingsideCastleInfo = { .PiecesMoved = {PieceType::King, PieceType::Rook}, 
	//			.SpecialMoveFlags= SpecialMove::KingSideCastle};
	//		return kingsideCastleInfo;
	//	}
	//	else if (cleanedInput == NOTATION_QUEENSIDE_CASTLE)
	//	{
	//		MoveInfo queensideCastleInfo = { .PiecesMoved = {PieceType::King, PieceType::Rook},
	//			.SpecialMoveFlags = SpecialMove::QueenSideCastle };
	//		return queensideCastleInfo;
	//	}
	//
	//	std::optional<PieceType> pieceMoved = TryGetPieceFromNotationSymbol(cleanedInput[0]);
	//	std::optional<Utils::Position2D> optionalCurrentPosInfo;
	//
	//	int colStringIndex = 1;
	//	int rowStringIndex = 2;
	//	bool isCapture = false;
	//	//If we have capture character, we have to increase to the 
	//	//next indices to search for new pos
	//	if (cleanedInput[1] == NOTATION_CAPTURE_CHAR)
	//	{
	//		colStringIndex++;
	//		rowStringIndex++;
	//		isCapture = true;
	//	}
	//	//If same piece can move to a spot we provide disambigiuoty check
	//	//and can provide an extra row
	//	else if (Utils::IsNumber(cleanedInput[colStringIndex]))
	//	{
	//		optionalCurrentPosInfo = { std::stod(&cleanedInput[colStringIndex]), NULL_POS,};
	//		colStringIndex++;
	//		rowStringIndex++;
	//	}
	//	//here is the disambigiuoty check for extra col
	//	else if (Utils::IsLetter(cleanedInput[colStringIndex]) && Utils::IsLetter(cleanedInput[rowStringIndex]))
	//	{
	//		optionalCurrentPosInfo = { NULL_POS, cleanedInput[colStringIndex] - 'a'};
	//		colStringIndex++;
	//		rowStringIndex++;
	//	}
	//
	//	auto possiblePieces = TryGetAvailablePiecesPosition();
	//	
	//	char col = cleanedInput[colStringIndex];
	//	char row = cleanedInput[rowStringIndex];
	//	Utils::Position2D newPos(std::atoi(&row), col - 'a');
	//
	//	std::optional<PieceType> promotePiece = std::nullopt;
	//	if (cleanedInput[rowStringIndex + 1] == NOTATION_PROMOTION_CHAR)
	//	{
	//		promotePiece= TryGetPieceFromNotationSymbol(cleanedInput[rowStringIndex+1]);
	//	}
	//
	//	bool isCheck = cleanedInput.back() == NOTATION_CHECK_CHAR;
	//	bool isCheckmate = cleanedInput.back() == NOTATION_CHECKMATE_CHAR;
	//	
	//	SpecialMove moveFlags = SpecialMove::None;
	//	if (promotePiece.has_value()) moveFlags |= SpecialMove::Promotion;
	//	if (isCapture) moveFlags |= SpecialMove::Capture;
	//
	//	MoveInfo info =
	//	{
	//		pieceMoved != {GetPieceAtPosition()},
	//		optionalCurrentPosInfo,
	//		newPos, 
	//		cleanedInput,
	//		moveFlags,
	//		promotePiece,
	//		isCheck,
	//		isCheckmate
	//	};
	//	return info;
	//}

	const std::vector<MoveInfo> GetPreviousMoves(const GameState& state, const ColorTheme& color)
	{
		if (state.PreviousMoves.size() == 0) return {};
		if (state.PreviousMoves.find(color) == state.PreviousMoves.end()) return {};

		return state.PreviousMoves.at(color);
	}

	const MoveInfo* GetPieceFirstMove(const GameState& state, const ColorTheme& color, const PieceType& type)
	{
		const std::vector<MoveInfo> colorMoves = GetPreviousMoves(state, color);
		if (colorMoves.size() == 0) return nullptr;

		for (const auto& move : colorMoves)
		{
			if (move.PiecesMoved.size() == 0) continue;
			for (const auto& piecesMoved : move.PiecesMoved)
			{
				if (piecesMoved.PieceRef.m_PieceType == type)
				{
					return &move;
				}
			}
		}
		return nullptr;
	}

	bool HasMovedPiece(const GameState& state, const ColorTheme& color, const PieceType& type)
	{
		return GetPieceFirstMove(state, color, type) != nullptr;
	}
}