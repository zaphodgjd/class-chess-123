#pragma once

#include <vector>
#include <unordered_map>
#include <stack>

#include "Game.h"
#include "ChessSquare.h"

const int pieceSize = 64;

enum ChessPiece {
	NoPiece	= 0,
	Pawn	= 1,
	Knight	= 2,
	Bishop	= 3,
	Rook	= 4,
	Queen	= 5,
	King	= 6
};

// the main game class
class Chess : public Game {
public:
	Chess();
	~Chess();

	// set up the board
	void		setUpBoard() override;

	Player*		checkForWinner() override;
	bool		checkForDraw() override;
	std::string	initialStateString() override;
	std::string	stateString() override;
	void		setStateString(const std::string &s) override;
	void		setStateFromFEN(const std::string &fen);
	bool		actionForEmptyHolder(BitHolder& holder) override;
	bool		canBitMoveFrom(Bit& bit, BitHolder& src) override;
	bool		canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
	void		bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

	void moveGenerator();

	void		stopGame() override;
	BitHolder&	getHolderAt(const int x, const int y) override { return _grid[y * 8 + x]; }

	void		updateAI() override;
	bool		gameHasAI() override { return false; }

private:
	ChessBit* 		PieceForPlayer(const int playerNumber, ChessPiece piece);
	ChessBit* 		PieceForPlayer(const char piece);
	const char	bitToPieceNotation(int rank, int file) const;
    const char	bitToPieceNotation(int i) const;

	// distances at a given position to the board's boundries. North, East, South, West, NE, SE, SW, NW
	int _dist[64][8];
	ChessSquare	_grid[64];
	std::unordered_map<int, std::vector<int>> _moves;
	// I don't need to use a stack, a vector would be perfectly fine, but a stack is syntactically simpler.
	std::stack<ChessSquare*> _litSquare;
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