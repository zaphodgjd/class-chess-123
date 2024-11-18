#include "GameState.h"

const uint8_t MAX_DEPTH = 24; // for AI purposes

GameState::GameState() {
	// do nothing
}

// Generate from FEN
GameState::GameState(const bool isBlack, const uint8_t castling,
	const uint8_t enTarget, const uint8_t hClock, const uint16_t fClock) 
	: isBlack(isBlack),
	castlingRights(castling),
	enPassantSquare(enTarget),
	halfClock(hClock),
	clock(fClock),
	stackPtr(0) {
		stateStack.resize(MAX_DEPTH);
		stateStack[0] = *this;
}

// generate next move
GameState::GameState(const GameState& old, const Move& move)
	: isBlack(!old.isBlack),
	// scuffed bit funkery: take old rights, mask out side's bits if castling happened.
	castlingRights(old.castlingRights & ~((move.KingSideCastle() || move.QueenSideCastle()) ? 0b11 : 0b00) << !isBlack * 2),
	// if double push, then get the square jumped over in turn and mark as EnPassant square.
	enPassantSquare(move.isDoublePush() ? (move.getTo() + (isBlack ? 8 : -8)) : 255), // 255 b/c not a value normally reachable in gameplay
	halfClock(move.isCapture() ? 0 : old.halfClock + 1),
	clock(old.clock + 1),
	stackPtr(old.stackPtr + 1),
	stateStack(old.stateStack) {
		stateStack[stackPtr] = *this;
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

bool GameState::pushMove(const Move& move) {
	if (stackPtr < MAX_DEPTH) {
		if (stateStack.size() == 0) {
			stateStack.resize(MAX_DEPTH);
		}

		stateStack.emplace_back(*this, move);
		++stackPtr;
		*this = stateStack[stackPtr];
		return true;
	} else {
		// Handle error: stack overflow
		return false;
	}
}

bool GameState::popState() {
	if (stackPtr > 0) {
		--stackPtr;
		*this = stateStack[stackPtr];
		return true;
	} else {
		// Handle error: stack underflow
		return false;
	}
}