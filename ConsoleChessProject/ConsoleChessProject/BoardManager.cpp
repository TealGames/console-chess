#include <format>
#include <queue>
#include <vector>
#include <unordered_set>
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

namespace Board
{
	MoveResult::MoveResult(const Utils::Point2DInt& pos, const bool isValid, const std::string& info)
		:AttemptedPositions({ pos }), IsValidMove(isValid), Info(info) {}

	MoveResult::MoveResult(const std::vector<Utils::Point2DInt>& positions, const bool isValid, const std::string& info)
		:AttemptedPositions(positions), IsValidMove(isValid), Info(info) {}

	/*using PiecePositionMapType = std::unordered_map<Utils::Point2DInt, Piece>;
	static PiecePositionMapType piecePositions;
	static std::unordered_map<ColorTheme, std::vector<MoveInfo>> previousMoves;

	static bool inCheckmate;
	static bool inCheck;*/
	static std::vector<Utils::Point2DInt> allPossiblePositions;

	static std::vector<Utils::Point2DInt> GetAllPositionsOnBoard()
	{
		if (!allPossiblePositions.empty()) return allPossiblePositions;

		for (int r = 0; r < BOARD_DIMENSION; r++)
		{
			for (int c = 0; c < BOARD_DIMENSION; c++)
			{
				allPossiblePositions.emplace_back(r, c);
			}
		}
		std::sort(allPossiblePositions.begin(), allPossiblePositions.end());
		return allPossiblePositions;
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
		int xCurrentDelta = 0;
		int yCurrentDelta = 0;

		while (currentPosition != endPosition)
		{
			//If we go perfect diagonal we increase both
			if (xTotalDelta == yTotalDelta)
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
			if (checkType && piecePos.second.pieceType != type.value()) continue;
			if (checkState)
			{
				bool pieceStateMatches = false;
				for (const auto& state : targetState.value())
				{
					if (piecePos.second.state == state)
					{
						pieceStateMatches = true;
						break;
					}
				}
				if (!pieceStateMatches) continue;
			}

			if (piecePos.second.color == color)
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

	static bool EraseMoveInfoWithData(std::vector<MoveInfo>& allMoveInfo, const std::function<bool(MoveInfo)> iterationCheck)
	{
		int index = 0;
		for (const auto& moveInfo : allMoveInfo)
		{
			if (iterationCheck(moveInfo))
			{
				allMoveInfo.erase(allMoveInfo.begin() + index);
				return true;
			}
			index++;
		}
		return false;
	}

	static bool EraseMoveInfoWithData(std::unordered_set<MoveInfo>& allMoveInfo, const std::function<bool(MoveInfo)> iterationCheck)
	{
		int index = 0;
		for (const auto& moveInfo : allMoveInfo)
		{
			if (iterationCheck(moveInfo))
			{
				int removedElements= allMoveInfo.erase(moveInfo);
				return removedElements > 0;
			}
			index++;
		}
		return false;
	}

	//Will check if the position is within bounds of the board
	bool IsWithinBounds(const Utils::Point2DInt& pos)
	{
		if (pos.x < 0 || pos.y >= BOARD_DIMENSION) return false;
		else if (pos.x < 0 || pos.y >= BOARD_DIMENSION) return false;
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
	static CheckOrMateResult IsCheckOrMateAtPiece(const GameState& state, const Utils::Point2DInt& posAttemptingCheck)
	{
		const Piece* pieceAttemptingCheck = TryGetPieceAtPosition(state, posAttemptingCheck);
		const std::unordered_set<MoveInfo> possibleMoves = GetPossibleMovesForPieceAt(state, posAttemptingCheck);

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
					if (pieceChecked != nullptr && pieceChecked->pieceType == PieceType::King &&
						pieceChecked->color != pieceAttemptingCheck->color)
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
			std::vector<PiecePositionData> kingPositions = TryGetPiecesPosition(state, pieceAttemptingCheck->color ==
				ColorTheme::Dark ? ColorTheme::Light : ColorTheme::Dark, PieceType::King,
				std::vector<Piece::State>{ Piece::State::InPlay });
			if (kingPositions.size() != 1)
			{
				std::string error = std::format("Tried to check for check and checkmate but no king was found!");
				Utils::Log(Utils::LogType::Error, error);
				return { false, false };
			}
			const std::unordered_set<MoveInfo> kingMoves = GetPossibleMovesForPieceAt(state, kingPositions[0].Pos);
			inCheckmateResult = kingMoves.size() == 0;
		}

		return {inCheckmateResult, inCheckResult};
	}

	/// <summary>
	/// Will check if it is a check or checkmate and then updte the state. Returns true if it was updated
	/// </summary>
	static bool UpdateCheckOrMate(GameState& state, const Utils::Point2DInt& posAttemptingCheck)
	{
		CheckOrMateResult result = IsCheckOrMateAtPiece(state, posAttemptingCheck);
		if (result.IsCheck == state.InCheck && result.IsCheckmate == state.InCheckmate) return false;

		state.InCheck = result.IsCheck;
		state.InCheckmate = result.IsCheckmate;
		return true;
	}

	static std::vector<PiecePositionData> GetCheckablePositions(const GameState& state, const ColorTheme& color)
	{
		std::vector<PiecePositionData> checkablePiecePositions;
		for (const auto& piece : state.PiecePositions)
		{
			if (piece.second.color != color) continue;
			CheckOrMateResult result = IsCheckOrMateAtPiece(state, piece.first);
			if (result.IsCheck || result.IsCheckmate)
				checkablePiecePositions.emplace_back(piece.second, piece.first);
		}

		return checkablePiecePositions;
	}

	static bool TryUpdatePiecePosition(GameState& state, const PiecePositionData& currentData, const Utils::Point2DInt& newPos)
	{
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

		bool hasPiece = state.PiecePositions.find(currentData.Pos) != state.PiecePositions.end();
		if (hasPiece)
		{
			size_t elementsErased = state.PiecePositions.erase(currentData.Pos);
			if (elementsErased == 0)
			{
				std::string error = std::format("Tried to place board piece: {} at pos: {} "
					"but failed to retrieve it and/or remove it from its old position {}",
					currentData.PieceRef.ToString(), newPos.ToString(), currentData.Pos.ToString());
				Utils::Log(Utils::LogType::Error, error);
				return false;
			}
		}
		state.PiecePositions.emplace(newPos, currentData.PieceRef);
		return true;
	}

	static bool TryUpdatePiecePosition(GameState& state, const PiecePositionData currentData, Utils::Point2DInt newPos, MoveInfo moveInfo)
	{
		if (!TryUpdatePiecePosition(state, currentData, newPos)) return false;

		ColorTheme color = currentData.PieceRef.color;
		if (state.PreviousMoves.find(color) == state.PreviousMoves.end())
		{
			state.PreviousMoves.emplace(color, std::vector<MoveInfo>{moveInfo });
		}
		else
		{
			state.PreviousMoves.at(color).push_back(moveInfo);
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
		/*
		std::vector<Utils::Point2DInt> allPossiblePositions = GetAllPositionsOnBoard();
		std::vector<Utils::Point2DInt> currentPiecePositions= Utils::GetKeysFromMap<Utils::Point2DInt, Piece>
			(piecePositions.begin(), piecePositions.end());

		std::unordered_set<Utils::Point2DInt> availablePositions;
		if (piecePositions.empty()) availablePositions = std::unordered_set<Utils::Point2DInt>
														(allPossiblePositions.begin(), allPossiblePositions.end());
		else availablePositions = Utils::GetUnorderedIntersection(currentPiecePositions, allPossiblePositions);
		*/

		int pieceIndex = 0;
		std::vector<Piece*> allPiecesCreatedOrUpdated;
		for (const auto& initPiecePos : GetDefaultBoardPiecePositions())
		{
			/*Utils::Log(Utils::LogType::Log, std::format("Board piece: {} {} for {}",
				ToString(color), ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString()));
			continue;*/
			/*Utils::Log(Utils::LogType::Log, std::format("DEFAULT board pos: {} {} {}",
				ToString(color), ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString()));
			continue;*/

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
			Utils::Log(Utils::LogType::Warning, std::format("For piece {} {} for pos: {} found existing: {}",
				ToString(initPiecePos.Color), ToString(initPiecePos.PieceType),
				initPiecePos.NewPos.ToString(), std::to_string(availablePieces.size())));

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

				/*if (movePiecePos != initPiecePos.NewPos &&
					!TryUpdatePiecePosition(PiecePositionData{ *movePiece, movePiecePos }, initPiecePos.NewPos))
				{
					std::string error = std::format("Tried to create default board and move board piece: {} to pos: {} "
						"but failed to update its position",
						movePiece->ToString(), initPiecePos.NewPos.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return;
				}*/
			}
			else
			{

				/*if (availablePositions.empty())
				{
					std::string error = std::format("Tried to place default board piece: {} {} at pos: {} "
						"but no available pos left for created piece",
						ToString(color), ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return;
				}*/
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

				/*Utils::Point2DInt availablePos = *availablePositions.begin();
				Utils::Log(Utils::LogType::Log, std::format("Got available pos for {} {} is {}. all available: {}",
					ToString(color), ToString(initPiecePos.PieceType), ToString(availablePos),
					Utils::ToStringIterable<decltype(availablePositions), Utils::Point2DInt>(availablePositions)));

				auto pairAtPosIt = piecePositions.find(availablePos);
				if (pairAtPosIt != piecePositions.end())
				{
					std::string error = std::format("Tried to place default board piece: {} {} at pos: {} "
						"but there already exists a peice: {} at that pos",
						ToString(color), ToString(initPiecePos.PieceType), availab, pairAtPosIt->second.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return;
				}

				auto insertedPieceIt = piecePositions.insert({ availablePos, CreatePiece(color, initPiecePos.PieceType) });

				if (!insertedPieceIt.second)
				{
					std::string error = std::format("Tried to place default board piece: {} {} at pos: {} "
						"but failed to create the piece at that point",
						ToString(color), ToString(initPiecePos.PieceType), initPiecePos.NewPos.ToString(), pairAtPosIt->second.ToString());
					Utils::Log(Utils::LogType::Error, error);
					return;
				}*/

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

			Utils::Log(Utils::LogType::Log, std::format("ADDING piece: {} to pos: {}",
				movePiece->ToString(), initPiecePos.NewPos.ToString()));
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

	static CastleInfo CanCastle(const GameState& state, const ColorTheme& color)
	{
		//Note: we dont want to check rook because it would check any rook moved and 
		//we might still be able to castle on one side and not on the other
		if (HasMovedPiece(state, color, PieceType::King)) return { false, false, false };
		/*const std::vector<PiecePositionData> rooks = TryGetPiecesPosition(state, color, PieceType::Rook,
			std::vector<Piece::State>{ Piece::State::InPlay });*/

		//TODO: optimize these calls so they are batched together to find pos for both rook and king at the same time
		const std::vector<PiecePositionData> kingPositions = TryGetPiecesPosition(state, color, PieceType::King,
			std::vector<Piece::State>{ Piece::State::InPlay });
		if (kingPositions.size() != 1) return { false, false, false };
		Utils::Point2DInt kingPos = kingPositions[0].Pos;

		const std::vector<PiecePositionData> rookPositions = TryGetPiecesPosition(state, color, PieceType::Rook,
			std::vector < Piece::State> { Piece::State::InPlay });
		if (rookPositions.size() <= 0 || rookPositions.size() > 2) return { false, false, false };

		bool canKingSide = true;
		bool canQueenSide = true;
		bool checkingKingSide = false;
		Utils::Point2DInt currentPosCheck;
		for (const auto& rookPos : rookPositions)
		{
			if (rookPos.Pos.x != kingPos.x) continue;

			currentPosCheck = kingPos;
			checkingKingSide = rookPos.Pos.y > currentPosCheck.y ? true : false;
			while (currentPosCheck.y != rookPos.Pos.y)
			{
				//King side goes up the cols queen side goes down
				if (checkingKingSide) currentPosCheck = { currentPosCheck.x, currentPosCheck.y + 1 };
				else currentPosCheck = { currentPosCheck.x, currentPosCheck.y - 1 };

				//Since we do the decrement/incremenet above, it can check to rook pos after loop condition
				if (currentPosCheck.y >= rookPos.Pos.y) break;

				if (TryGetPieceAtPosition(state, currentPosCheck))
				{
					if (checkingKingSide) canKingSide = false;
					else canQueenSide = false;
					break;
				}
			}
		}

		Utils::Point2DInt kingSideCastleMove = canKingSide ? Utils::Point2DInt{ kingPos.x, kingPos.y + 2 } : Utils::Point2DInt{};
		Utils::Point2DInt queenSideCastleMove = canKingSide ? Utils::Point2DInt{ kingPos.x, kingPos.y - 2 } : Utils::Point2DInt{};
		return { canKingSide || canQueenSide, canKingSide, canQueenSide, kingSideCastleMove, queenSideCastleMove };
	}
	static CastleInfo IsCastleMove(const GameState& state, const PiecePositionData currentData, const Utils::Point2DInt& newPos)
	{
		if (currentData.PieceRef.pieceType != PieceType::King) return { false, false, false };

		int startCol = std::min(currentData.Pos.y, newPos.y);
		int endCol = std::max(currentData.Pos.y, newPos.y);
		int delta = endCol - startCol;

		const Piece* outPiece;
		int rookCol = delta > 0 ? BOARD_DIMENSION - 1 : 0;
		//If in the direction moved is NOT a rook at the end, it means we do not have castle chance
		if ((outPiece = TryGetPieceAtPosition(state, { currentData.Pos.x , rookCol })) == nullptr) return { false, false, false };
		if (outPiece == nullptr || outPiece->pieceType != PieceType::Rook) return { false, false, false };

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

		if (!DoesMoveDeltaMatchCaptureMoves(currentData.PieceRef.pieceType, currentData.Pos, newPos))
			return nullptr;

		//TODO: are there any other checks to capture a piece
		return pieceAtNewPos;
	}
	static bool IsCapture(const GameState& state, const PiecePositionData currentData, const Utils::Point2DInt newPos)
	{
		return TryGetCapturePiece(state, currentData, newPos) != nullptr;
	}

	std::unordered_set<MoveInfo> GetPossibleMovesForPieceAt(const GameState& state, const Utils::Point2DInt& startPos)
	{
		if (!IsWithinBounds(startPos))
		{
			//Utils::Log("Out of bounds");
			return {};
		}
			
		const Piece* movedPiece = nullptr;
		if ((movedPiece = TryGetPieceAtPosition(state, startPos)) == nullptr)
		{
			//Utils::Log("Not found");
			return {};
		}

		std::unordered_set<MoveInfo> possibleMoves;

		Utils::Point2DInt moveNewPos;
		std::string start = moveNewPos.ToString();
		moveNewPos= Utils::Point2DInt(5, 6);
		std::string end = moveNewPos.ToString();
		Utils::Log(std::format("Test before: {} after {}", start, end));
		
		//Utils::Log(std::format("MOVE DIRS: {}", std::to_string(GetMoveDirsForPiece(movedPiece->pieceType).size())));
		for (auto& movePos : GetMoveDirsForPiece(movedPiece->pieceType))
		{
			moveNewPos = GetVectorEndPoint(startPos, movePos);
			/*Utils::Log(std::format("Is {} (start: {} new pos: {}) within bounds move: {} piece in range: {}", 
				startPos.ToString(), movePos.ToString(Utils::Vector2D::VectorForm::Component), moveNewPos.ToString(), 
				std::to_string(IsWithinBounds(moveNewPos)), 
				std::to_string(HasPieceWithinPositionRange(state, startPos, moveNewPos, false))));*/
			if (IsWithinBounds(moveNewPos) && !HasPieceWithinPositionRange(state, startPos, moveNewPos, false))
			{
				//We have to check if it is a capture because capture moves might not be different
				//from move dirs so we might capture during regualar moves
				SpecialMove specialMove = TryGetCapturePiece(state, PiecePositionData{ *movedPiece, startPos }, moveNewPos) ?
					SpecialMove::Capture : SpecialMove::None;

				possibleMoves.emplace(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, moveNewPos)
				},
					"",
					specialMove,
					std::nullopt,
					false,
					false
				);
			}
		}

		const std::vector<Utils::Vector2D> captureMoves = GetCaptureMovesForPiece(movedPiece->pieceType);
		for (auto& captureMove : captureMoves)
		{
			moveNewPos = GetVectorEndPoint(startPos, captureMove);
			if (IsWithinBounds(moveNewPos) && TryGetPieceAtPosition(state, moveNewPos)!=nullptr &&
				!HasPieceWithinPositionRange(state, startPos, moveNewPos, false))
			{
				possibleMoves.emplace(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, moveNewPos)
				},
					"",
					SpecialMove::Capture,
					std::nullopt,
					false,
					false
				);
			}
		}

		CastleInfo castleInfo = CanCastle(state, movedPiece->color);
		if (movedPiece->pieceType == PieceType::King && castleInfo.canCastle)
		{
			if (castleInfo.isKingSide)
			{
				possibleMoves.emplace(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, castleInfo.kingSideCastleMove)
				},
					"",
					SpecialMove::Capture,
					std::nullopt,
					false,
					false
				);
			}
			else if (castleInfo.isQueenSide)
			{
				possibleMoves.emplace(
					std::vector<MovePiecePositionData>
				{
					MovePiecePositionData(*movedPiece, startPos, castleInfo.queenSideCastleMove)
				},
					"",
					SpecialMove::Capture,
					std::nullopt,
					false,
					false
				);
			}

			//Kings also get checked for any positions that are able to be checked
			//and those positions are illegal and are removed
			std::vector<PiecePositionData> checkPositions = GetCheckablePositions(state, movedPiece->color);
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

		return possibleMoves;
	}

	//Will check if it is possible to move to that point using a variety of bounds checks,
	//valid moves, and special move checks
	MoveResult TryMove(GameState& state, const Utils::Point2DInt& currentPos, const Utils::Point2DInt& newPos)
	{
		if (!IsWithinBounds(currentPos))
			return { newPos, false, std::format("Tried to move from a place outside the board") };

		const Piece* movedPiece = nullptr;
		if ((movedPiece = TryGetPieceAtPosition(state, currentPos)) == nullptr)
			return { newPos, false, std::format("Position ({}) has no piece", newPos.ToString()) };

		if (!IsWithinBounds(newPos))
			return { newPos, false, std::format("Tried to move to a place outside the board") };

		const std::unordered_set<MoveInfo> possibleMoves = GetPossibleMovesForPieceAt(state, currentPos);
		if (!possibleMoves.size() == 0)
			return { newPos, false, std::format("There are no possible moves for this piece") };

		for (const auto& moveInfo : possibleMoves)
		{
			for (const auto& move : moveInfo.PiecesMoved)
			{
				//If the piece moved for this move is not the one we want we skip
				if (&move.PieceRef != movedPiece) continue;

				if (move.NewPos == newPos)
				{
					TryUpdatePiecePosition(state, PiecePositionData{ *movedPiece, currentPos }, newPos, moveInfo);
					return { newPos, true };
				}
			}
		}
		return { newPos, false, std::format("New pos does not match any pos for this piece") };

		//Piece* movedPiece;
		//if (!HasPieceAtPosition(currentPos, movedPiece))
		//	return { newPos, false, std::format("Position ({}) has no piece", newPos.ToString()) };
		//
		//if (!IsWithinBounds(newPos))
		//	return { newPos, false, std::format("Position ({}) is out of bounds", newPos.ToString()) };

		////Based on the castle info we assume to add the rook position 
		////based on if it is kingside or queenside
		//PiecePositionData currentPosData = { movedPiece, currentPos };
		//if (CastleInfo castleInfo= IsCastleMove(currentPosData, newPos); castleInfo.canCastle)
		//{
		//	std::vector<Utils::Position2D> rookKingMoves = { newPos };

		//	if (castleInfo.isKingSide)
		//		rookKingMoves.emplace_back(newPos.x, newPos.y-1);
		//	else if (castleInfo.isQueenSide)
		//		rookKingMoves.emplace_back(newPos.x, newPos.y+1);
		//	else 
		//	{
		//		std::string error = std::format("Tried to move piece: {} at pos: {} "
		//			"but failed to update its position: {} to castle move since it is neither kingside nor queenside",
		//			currentPosData.piece->ToString(), currentPosData.pos.ToString(), newPos.ToString());
		//		Utils::Log(Utils::LogType::Error, error);
		//		return {newPos, false};
		//	}
		//	return { rookKingMoves, true };
		//}

		////Update position 
		//Piece* takenPiece = nullptr;
		//if (IsCapture(currentPosData, newPos, takenPiece))
		//{
		//	if (!TryUpdatePiecePosition({ movedPiece, currentPos }, newPos))
		//	{
		//		return { newPos, false, std::format("Tried to place default board piece: {} at pos: {} "
		//			"due to capture but failed to update its position {}",
		//			movedPiece->ToString(), currentPos.ToString(), newPos.ToString()) };
		//	}
		//	takenPiece->UpdateState(Piece::State::Captured);
		//	return { newPos, true };
		//}

		//if (DoesMoveDeltaMatchPieceMoves(movedPiece->pieceType, currentPos, newPos))
		//{
		//	if (!TryUpdatePiecePosition({ movedPiece, currentPos }, newPos))
		//	{
		//		return { newPos, false, std::format("Tried to place default board piece: {} at pos: {} "
		//			"due to move delta but failed to update its position {}",
		//			movedPiece->ToString(), currentPos.ToString(), newPos.ToString()) };
		//	}
		//	return { newPos, true };
		//}
		//	

		////TODO: add special move checks like castleing, taking, etc.

		//return { newPos, false, std::format("Move Delta from {} -> {} does not match any possible moves for {}",
		//		currentPos.ToString(), newPos.ToString(), ToString(movedPiece->pieceType)) };
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

	const std::vector<MoveInfo>& GetPreviousMoves(const GameState& state, const ColorTheme& color)
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
				if (piecesMoved.PieceRef.pieceType == type)
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