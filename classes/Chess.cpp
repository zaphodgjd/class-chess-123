#include "Chess.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

using namespace std;

Chess::Chess()
{
}

Chess::~Chess()
{
}

//
// make a chess piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    ChessPiece initGrid[8][8] = 
        {{Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook},
        {Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn},
        {NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece}, 
        {NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece},
        {NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece},
        {NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece, NoPiece},
        {Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn},
        {Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook}
        };

    const int BOARDDIM = 8;
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    int x; int y;

    setNumberOfPlayers(2);
    for (int i = 0; i < BOARDDIM; i++){
        for (int j = 0; j < BOARDDIM; j++){
            x = 64 * j + 100; y = 64 * i + 100;
            const ImVec2 loc = ImVec2(x, y);
            _grid[i][j].initHolder(loc, "boardsquare.png", i, j); 
            ChessPiece initp = initGrid[i][j];
            _grid[i][j].setGameTag((i < 4) ? initp + 128 : initp); // need to add 128 if in black pieces
            if (initp != NoPiece){
                Bit* myBit = PieceForPlayer((i < 4) ? 1 : 0, initp);  // need to check for if black or not
                myBit->setParent(&_grid[i][j]);
                myBit->setPosition(loc);
                _grid[i][j].setBit(myBit);
            }

        }
    }
   
    startGame();
}

//
// about the only thing we need to actually fill out for chess 
//
bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit()) {
        return false;
    }
    return true;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    if (src.bit() == &bit){
        return true;
    }
    // needs to be set
    return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    if (dst.canDropBitAtPoint(&bit, dst.getPosition())){
        return true;
    }
    return false;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    dst.dropBitAtPoint(&bit, dst.getPosition());
    src.setBit(nullptr); 
}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{
    // check to see if either player has won
    return nullptr;
}

bool Chess::checkForDraw()
{
    // check to see if the board is full
    return false;
}

//
// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
//
const char Chess::bitToPieceNotation(int row, int column) const {
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }

    const char* wpieces = { "?PNBRQK" };
    const char* bpieces = { "?pnbrqk" };
    unsigned char notation = '0';
    Bit* bit = _grid[row][column].bit();
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag() & 127];
    } else {
        notation = '0';
    }
    return notation;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            s += bitToPieceNotation(y, x);
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit(PieceForPlayer(playerNumber - 1, Pawn));
            } else {
                _grid[y][x].setBit(nullptr);
            }
        }
    }
}


//
// this is the function that will be called by the AI
//
void Chess::updateAI() 
{
}