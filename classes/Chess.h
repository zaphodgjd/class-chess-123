#pragma once
#include "Game.h"
#include "ChessSquare.h"

#include <list>

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

//
// the main game class
//
class Chess : public Game
{
public:
    Chess();
    ~Chess();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder& holder) override;
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    void        stopGame() override;
    BitHolder& getHolderAt(const int x, const int y) override { return _grid[y][x]; }

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
    void        QuickPlacePeice(int x, int y, ChessPiece type, int color);
    bool        bitFromToHelper(int sX, int sY, int type, BitHolder& dst);
    void        generateMoveList();
    bool        getPosAttacked(int pX, int pY);
private:
    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    const char  bitToPieceNotation(int row, int column) const;

    ChessSquare _grid[8][8];

    //0b111000000000 src x of possible move
    //0b000111000000 src y of possible move
    //0b000000111000 dst x of possible move
    //0b000000000111 dst y of possible move
    std::list<int>  _moveList = {};
    //0b1100 Black Castle Possible (Both Ways)
    //0b1010 Queenside Castle Possible (Both Colors)
    char        _castleRights = 0b1111;
    char        _enpassant = -1;
};