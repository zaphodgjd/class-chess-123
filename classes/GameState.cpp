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

}

GameState& GameState::operator=(const GameState& other) {
	if (this != &other) {
		castlingRights = other.castlingRights;
		enPassantSquare = other.enPassantSquare;
		halfClock = other.halfClock;
		clock = other.clock;
		isBlack = other.isBlack;
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