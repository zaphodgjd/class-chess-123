#include <cstring>

#include "GameState.h"
#include "../tools/Logger.h"

const uint8_t MAX_DEPTH = 24; // for AI purposes
// Generate from FEN
GameState::GameState(const bool isBlack, const uint8_t castling,
	const uint8_t enTarget, const uint8_t hClock, const uint16_t fClock) 
	: isBlack(isBlack),
	castlingRights(castling),
	enPassantSquare(enTarget),
	halfClock(hClock),
	clock(fClock) {
	std::memset(state, '0', sizeof(state));
}

// generate next move
GameState::GameState(const GameState& old, const Move& move)
	: isBlack(!old.isBlack),
	// scuffed bit funkery: take old rights, mask out side's bits if castling happened.
	castlingRights(old.castlingRights & ~((move.KingSideCastle()  ? (isBlack ? 0b1000 : 0b0010) : 0) 
	                                    | (move.QueenSideCastle() ? (isBlack ? 0b0100 : 0b0001) : 0))),
	// if double push, then get the square jumped over in turn and mark as EnPassant square.
	enPassantSquare(move.isDoublePush() ? (move.getTo() + (isBlack ? -8 : 8)) : 255), // 255 b/c not a value normally reachable in gameplay
	halfClock(move.isCapture() ? 0 : old.halfClock + 1),
	clock(old.clock + 1) {
	char movingPiece = state[move.getFrom()];
	state[move.getTo()] = movingPiece;
	state[move.getFrom()] = '0';

	if (move.isCastle()) {
		uint8_t offset = isBlack ? 56 : 0;
		uint8_t rookSpot = move.QueenSideCastle() ? 0 : 7 + offset;
		uint8_t targ = (move.QueenSideCastle() ? 3 : 5) + offset;

		state[targ] = state[rookSpot];
		state[rookSpot] = '0';
	} else if (move.getTo() == old.enPassantSquare) { // did en passant happen
		state[movingPiece == 'P' ? (move.getTo() - 8) : (move.getTo() + 8)] = '0';
	} else if (move.isPromotion()) {
		char newPiece;
		switch(move.getFlags() & Move::FlagCodes::Promotion) {
			case Move::FlagCodes::ToQueen:
				newPiece = 'Q';
				break;
			case Move::FlagCodes::ToKnight:
				newPiece = 'K';
				break;
			case Move::FlagCodes::ToRook:
				newPiece = 'R';
				break;
			case Move::FlagCodes::ToBishop:
				newPiece = 'B';
				break;
		}
		newPiece += isBlack ? 32 : 0;
		state[move.getTo()] = newPiece;
	}
}

GameState& GameState::operator=(const GameState& other) {
	if (this != &other) {
		castlingRights = other.castlingRights;
		enPassantSquare = other.enPassantSquare;
		halfClock = other.halfClock;
		clock = other.clock;
		isBlack = other.isBlack;
		std::memcpy(state, other.state, sizeof(state));
	}
	return *this;
}

bool GameState::operator==(const GameState& other) {
	if (this == &other) return true;
	return 	castlingRights == other.castlingRights &&
			enPassantSquare == other.enPassantSquare &&
			halfClock == other.halfClock &&
			clock == other.clock &&
			isBlack == other.isBlack;
}