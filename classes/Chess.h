#pragma once
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
	King	= 6,
	White	= 1 << 3,
	Black	= 1 << 4
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
	bool		actionForEmptyHolder(BitHolder& holder) override;
	bool		canBitMoveFrom(Bit& bit, BitHolder& src) override;
	bool		canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
	void		bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

	void		stopGame() override;
	BitHolder&	getHolderAt(const int x, const int y) override { return _grid[y * 8 + x]; }
    BitHolder&	getHolderAt(const int i) override { return _grid[i]; }

	void		updateAI() override;
	bool		gameHasAI() override { return true; }

private:
	Bit* 		PieceForPlayer(const int playerNumber, ChessPiece piece);
	const char	bitToPieceNotation(int row, int column) const;
    const char	bitToPieceNotation(int i) const;

	ChessSquare	_grid[64];
	uint64_t wPawns = 0x000000000000FF00;
	uint64_t bPawns = 0x00FF000000000000;

};