#pragma once

#include <cstdint>
#include <string>

#include "Move.h"

class GameState {
	public:
	// Generate from FEN
	GameState(const std::string& fen, const bool isBlack, const uint8_t castling,
		const uint8_t enTarget, const uint8_t hClock, const uint16_t fClock);
	
	// generate next move
	GameState(const GameState& old, const Move& move);

	GameState& operator=(const GameState& other);
	bool operator==(const GameState& other);

	bool isBlackTurn() const { return isBlack; }
	uint8_t getEnPassantSquare()	const { return enPassantSquare; }
	uint8_t getCastlingRights()	const { return castlingRights; }
	uint8_t getHalfClock()		const { return halfClock; }
	uint16_t getClock() const { return clock; }
	void setCastlingRights(const uint8_t rights) { castlingRights = rights; }
	const char* getState() const { return state; }

	// TODO: init state w/ proper values. For later!

	protected:
	//Bitboard bitboard;
	bool isBlack;
	uint8_t castlingRights : 4; // KQkq
	// FOR FUTURE SELF: I am not capping this b/c I need an easy to check null value (ex, 255)
	uint8_t enPassantSquare;
	uint8_t halfClock : 6;
	uint16_t clock;
	//const uint64_t hash;
	char state[64];

	// TODO: add a way of tracking board state (graeme recommends a 64 bit char array)

	// if i want to support undoing, it may be a good idea to add a string to these (or an associated class) that keeps track
	// of FEN so I can reload from FEN.
};

// https://www.chessprogramming.org/Bitboards
// 99% sure these are correct starting values
struct Bitboard {
	uint64_t wPawns		= 0x000000000000FF00;
	uint64_t bPawns		= 0x00FF000000000000;
	uint64_t wKnight	= 0x0000000000000042;
	uint64_t bKnight	= 0x4200000000000000;
	uint64_t wBishop	= 0x0000000000000024;
	uint64_t bBishop	= 0x2400000000000000;
	uint64_t wRook		= 0x0000000000000081;
	uint64_t bRook		= 0x8100000000000000;
	uint64_t wQueen		= 0x0000000000000008;
	uint64_t bQueen		= 0x0800000000000000;
	uint64_t wKing		= 0x0000000000000010;
	uint64_t bKing		= 0x1000000000000000;
};