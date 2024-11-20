#include "Chess.h"


const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

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
    ChessPiece backRowArray[8] = {Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook};
    std::string Fen = std::string("5k2/8/8/8/8/8/8/4K2R w K - 0 1");
    std::unordered_map<std::string, int> mappings = {
    {"r", Rook},
    {"n", Knight},
    {"b", Bishop},
    {"q", Queen},
    {"k", King},
    {"p", Pawn},
    {"R", Rook + 128},
    {"N", Knight + 128},
    {"B", Bishop + 128},
    {"Q", Queen + 128},
    {"K", King + 128},
    {"P", Pawn + 128}
    };
    std::unordered_map<std::string, int> quickConvert = {
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {"5", 5},
    {"6", 6},
    {"7", 7},
    {"8", 8}
    };
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++){
            _grid[x][y].initHolder(ImVec2(50*x +100, 50*y + 100), "square.png", x, y);
        }
    }
    int row = 0;
    int input = 0;
    for (int i = 0; i < Fen.length(); ++i) {
        int absolutePos = i + row;
        if (absolutePos < 64){
            char curChar = Fen.at(i);
            std::string curCharStr(1, curChar);
            if (mappings.count(curCharStr) == 0){
                if (curChar == *"/"){
                    row -= 1;
                } else if (quickConvert[curCharStr] <= 8){
                    row += quickConvert[curCharStr] - 1;
                }
            } else {
                int piece = mappings[curCharStr];
                Bit *bit = PieceForPlayer((piece < 128) ? 1:0, ChessPiece(piece % 128));
                BitHolder& holder = getHolderAt(absolutePos / 8, absolutePos % 8);
                bit->setPosition(holder.getPosition());
                bit->setGameTag(piece);
                holder.setBit(bit);
            }
        } else {
            char curChar = Fen.at(i);
            if (curChar == *" "){
            input +=1;
            }
            else if (input == 1){
                if (curChar == *"b"){
                    endTurn();
                }
                _canCastle[0] = 0;
                _canCastle[1] = 0;
                _canCastle[2] = 0;
                _canCastle[3] = 0;
            }
            else if(input == 2){
                if (curChar == *"Q"){
                    _canCastle[0] = 1;
                }
                if (curChar == *"K"){
                    _canCastle[1] = 1;
                }
                if (curChar == *"q"){
                    _canCastle[2] = 1;
                }
                if (curChar == *"K"){
                    _canCastle[3] = 1;
                }
            }
            else if (input == 3){
                if (curChar == *"-"){ 
                } else {
                    int charToInt = (int)curChar - 97;
                    _enPassant = charToInt * 8;
                }
                input += 1;
            }
            else if (input == 4){
                int num = curChar - '0';
                _enPassant += num;
            }
            else if (input == 5){
                int num = curChar - '0';
                _halfMoves = num;
            }
            else if (input == 6){
                int num = curChar - '0';
                _gameOptions.currentTurnNo = num;
            }
        }
    }

    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return true;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst, Player* player)
{
    ChessPiece ChessPieceArray[7] = {NoPiece, Pawn, Knight, Bishop, Rook, Queen, King};
    if (player->playerNumber() != bit.getOwner()->playerNumber()){
        return false;
    }
    ChessPiece piece = ChessPieceArray[bit.gameTag() % 8];
    std::vector<int> possibleMoves = generateMoves(((src.getRow()*8) + src.getColumn()), stateString());

    //Castling Logic
    if (player->playerNumber() == 1 && piece == King){
        if (_canCastle[2] == 1 && ((dst.getRow() * 8 ) + dst.getColumn()) == 2){
            if (std::count(possibleMoves.begin(), possibleMoves.end(), 128) >= 1){
                return true;
            }
        }
        if (_canCastle[3] == 1 && ((dst.getRow() * 8 ) + dst.getColumn()) == 6){
            if (std::count(possibleMoves.begin(), possibleMoves.end(), 129) >= 1){
                return true;
            }
        }
    }
    if (player->playerNumber() == 0 && piece == King){
        if (_canCastle[0] == 1 && (((dst.getRow() * 8 ) + dst.getColumn()) == 58)){
            if (std::count(possibleMoves.begin(), possibleMoves.end(), 128) >= 1){
                return true;
            }
        }
        if (_canCastle[1] == 1 && (((dst.getRow() * 8 ) + dst.getColumn()) == 62)){
            if (std::count(possibleMoves.begin(), possibleMoves.end(), 129) >= 1){
                return true;
            }
        }
    }
    if (((dst.getRow() * 8 ) + dst.getColumn() == _enPassant) && piece == Pawn){
        if (player->playerNumber() == 0){
            if(src.getColumn() == dst.getColumn() + 1|| src.getColumn() == dst.getColumn() - 1){
                if(src.getRow() == dst.getRow() + 1){  
                    return true;
                }
            }   
        }
        if (player->playerNumber() == 1){
            if(src.getColumn() == dst.getColumn() + 1|| src.getColumn() == dst.getColumn() - 1){
                if(src.getRow() == dst.getRow() - 1){  
                    return true;
                }
            }   
        }
    }






    if (std::count(possibleMoves.begin(), possibleMoves.end(), (((dst.getRow() * 8 ) + dst.getColumn()))) >= 1){
        return true;
    }
        
    return false;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    Player* player = bit.getOwner();
    int piece = bit.gameTag();

    //King castling logic
    if (piece == King || piece == King + 128){
        if (player->playerNumber() == 1){
           if (_canCastle[2] == 1 && dst.getColumn() == 2){
                BitHolder &holder = getHolderAt(0, 0);
                Bit *castle = holder.bit();
                BitHolder &castleHolder = getHolderAt(0,3);
                castle->setPosition(castleHolder.getPosition());
                castleHolder.setBit(castle);
           }
           if (_canCastle[3] == 1 && dst.getColumn() == 6){
                BitHolder &holder = getHolderAt(0, 7);
                Bit *castle = holder.bit();
                BitHolder &castleHolder = getHolderAt(0,5);
                castle->setPosition(castleHolder.getPosition());
                castleHolder.setBit(castle);
           }
            _canCastle[2] = 0;
            _canCastle[3] = 0;
        } if(player->playerNumber() == 0){
            if (_canCastle[0] == 1 && dst.getColumn() == 2){
                BitHolder &holder = getHolderAt(7, 0);
                Bit *castle = holder.bit();
                BitHolder &castleHolder = getHolderAt(7,3);
                castle->setPosition(castleHolder.getPosition());
                castleHolder.setBit(castle);
           }
           if (_canCastle[1] == 1 && dst.getColumn() == 6){
                BitHolder &holder = getHolderAt(7, 7);
                Bit *castle = holder.bit();
                BitHolder &castleHolder = getHolderAt(7,5);
                castle->setPosition(castleHolder.getPosition());
                castleHolder.setBit(castle);
           }
            _canCastle[0] = 0;
            _canCastle[1] = 0;
        }
    }
    
    //Rook castling logic

    if (piece == Rook){
        if (src.getColumn() == 0 && src.getRow() == 0){
            _canCastle[2] = 0; 
        }
        if (src.getColumn() == 7 && src.getRow() == 0){
            _canCastle[3] = 0; 
        }
    }
    if (piece == Rook + 128){
        if (src.getColumn() == 0 && src.getRow() == 7){
            _canCastle[0] = 0; 
        }
        if (src.getColumn() == 7 && src.getRow() == 7){
            _canCastle[1] = 0; 
        }
    }
    if (piece % 128 == Pawn && (((dst.getRow()*8) + dst.getColumn()) == _enPassant)){
        int playerValue = (player->playerNumber() == 0) ? 1:-1;
        BitHolder &holder = getHolderAt (dst.getRow() + playerValue, dst.getColumn());
        holder.destroyBit();
    }

    if (piece % 128 == Pawn && abs(dst.getRow() - src.getRow()) == 2){
        _enPassant = (dst.getRow() * 8) + dst.getColumn();
        if (player->playerNumber() == 0){
            _enPassant += 8;
        } else{
            _enPassant -= 8;
        }
    } else {
        _enPassant = 0;
    }
    
    


    //Promotion
    if (piece == Pawn && dst.getRow() == 7){
        Bit *bit = PieceForPlayer(1, Queen);
        bit->setPosition(dst.getPosition());
        bit->setGameTag(Queen);
        dst.setBit(bit);
    }
    if (piece == Pawn + 128 && dst.getRow() == 0){
        Bit *bit = PieceForPlayer(0, Queen);
        bit->setPosition(dst.getPosition());
        bit->setGameTag(Queen + 128);
        dst.setBit(bit);
    }
    
    endTurn();
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
            s += bitToPieceNotation(x, y);
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

