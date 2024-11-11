#pragma once
#include "Game.h"
#include "ChessSquare.h"

const int pieceSize = 64;

enum ChessPiece {
    NoPiece = 0,
    Pawn = 1,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

class Chess : public Game {
public:
    Chess();
    ~Chess();

    void        setUpBoard() override;
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder& holder) override;  // Uncommented if it's being overridden
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;
    BitHolder&  getHolderAt(const int x, const int y) override { return _grid[y][x]; }
	void        updateAI() override;
    bool        gameHasAI() override { return true; }

private:
    Bit*        PieceForPlayer(const int playerNumber, ChessPiece piece);
    void        initializePiece(int y, int x, int playerNumber, ChessPiece piece);  // Added initializePiece here
    const char  bitToPieceNotation(int row, int column) const;

    // Movement validation functions for each piece type
    bool        isValidPawnMove(Bit& bit, int startX, int startY, int endX, int endY);
    bool        isValidKnightMove(int startX, int startY, int endX, int endY);
    bool        isValidBishopMove(int startX, int startY, int endX, int endY);
    bool        isValidRookMove(int startX, int startY, int endX, int endY);
    bool        isValidQueenMove(int startX, int startY, int endX, int endY);
    bool        isValidKingMove(int startX, int startY, int endX, int endY);

    ChessSquare _grid[8][8];
};