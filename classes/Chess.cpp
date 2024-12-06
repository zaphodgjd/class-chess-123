#include "Chess.h"
#include <vector>

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

void Chess::setUpBoard() {
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    // Initialize the grid with squares
    for (int y = 0; y < _gameOptions.rowY; ++y) {
        for (int x = 0; x < _gameOptions.rowX; ++x) {
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * (7 - y) + pieceSize));
            _grid[y][x].initHolder(position, "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0); // Empty square initially
        }
    }

    // Reset bitboards
    wPieces = 0;
    bPieces = 0;

    // Use FEN to set up the board
    std::string initialFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    FENtoBoard(initialFEN);

    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    std::string piecePlacement, activeColor = "w", castlingString = "KQkq", enPassantString = "-";
    _halfmoveClock = 0;
    int fullmoveNumber = 1;

    // Parse the FEN string
    std::istringstream iss(fen);
    iss >> piecePlacement >> activeColor >> castlingString >> enPassantString >> _halfmoveClock >> fullmoveNumber;

    // Set the turn based on fullmove number and active color
    _gameOptions.currentTurnNo = (fullmoveNumber - 1) * 2 + (activeColor == "b" ? 1 : 0);

    // Parse castling rights
    for (int i = 0; i < 4; i++) _castling[i] = false;
    if (castlingString != "-") {
        for (char c : castlingString) {
            if (c == 'K') _castling[0] = true;
            else if (c == 'Q') _castling[1] = true;
            else if (c == 'k') _castling[2] = true;
            else if (c == 'q') _castling[3] = true;
        }
    }

    // Parse en passant square
    if (enPassantString != "-") {
        int file = enPassantString[0] - 'a';
        int rank = enPassantString[1] - '1';
        _enPassant = &_grid[rank][file];
    } else {
        _enPassant = nullptr;
    }

    // Place pieces based on the piece placement part of the FEN
    static const std::unordered_map<char, ChessPiece> pieceMap = {
        {'p', Pawn}, {'r', Rook}, {'n', Knight}, {'b', Bishop}, {'q', Queen}, {'k', King}
    };

    int row = 7, col = 0;
    for (char c : piecePlacement) {
        if (isdigit(c)) {
            col += c - '0'; // Empty squares
        } else if (c == '/') {
            row--; col = 0; // Move to the next row
        } else {
            bool isWhite = isupper(c);
            ChessPiece piece = pieceMap.at(tolower(c));

            Bit* bit = PieceForPlayer(isWhite ? 0 : 1, piece);
            bit->setPosition(_grid[row][col].getPosition());
            bit->setParent(&_grid[row][col]);
            _grid[row][col].setBit(bit);

            int gameTag = piece + (isWhite ? 0 : 128);
            bit->setGameTag(gameTag);
            _grid[row][col].setGameTag(gameTag);

            // Update the bitboards
            uint64_t bitPos = 1ULL << (row * 8 + col);
            if (isWhite) wPieces |= bitPos;
            else bPieces |= bitPos;

            col++;
        }
    }
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
    return true;
}
bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
    ChessSquare* srcSquare = dynamic_cast<ChessSquare*>(&src);
    ChessSquare* dstSquare = dynamic_cast<ChessSquare*>(&dst);

    if (!srcSquare || !dstSquare) return false; // Ensure proper casting

    int srcX = srcSquare->getColumn();
    int srcY = srcSquare->getRow();
    int dstX = dstSquare->getColumn();
    int dstY = dstSquare->getRow();

    int gameTag = bit.gameTag();
    bool isWhite = gameTag < 128;

    // Ensure it's the player's turn and the piece belongs to the current player
    if (isWhite != (getCurrentPlayer()->playerNumber() == 0)) return false;

    std::vector<int> validMoves;

    // Get valid moves for the piece
    switch (gameTag % 128) {
        case Rook: validMoves = getRookMoves(srcX, srcY, isWhite); break;
        case Bishop: validMoves = getBishopMoves(srcX, srcY, isWhite); break;
        case Queen:
            validMoves = getRookMoves(srcX, srcY, isWhite);
            {
                auto bishopMoves = getBishopMoves(srcX, srcY, isWhite);
                validMoves.insert(validMoves.end(), bishopMoves.begin(), bishopMoves.end());
            }
            break;
        case Knight: validMoves = getKnightMoves(srcX, srcY, isWhite); break;
        case King:
            validMoves = getKingMoves(srcX, srcY, isWhite);
            handleCastlingMoves(srcX, srcY, isWhite, validMoves);
            break;
        case Pawn:
            validMoves = getPawnMoves(srcX, srcY, isWhite);
            handleEnPassantMoves(srcX, srcY, dstX, dstY, isWhite, validMoves);
            break;
        default: return false;
    }

    int target = dstY * 8 + dstX;

    // Ensure the move is valid
    return std::find(validMoves.begin(), validMoves.end(), target) != validMoves.end();
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    ChessSquare* srcSquare = dynamic_cast<ChessSquare*>(&src);
    ChessSquare* dstSquare = dynamic_cast<ChessSquare*>(&dst);

    int gameTag = bit.gameTag();
    bool isWhite = gameTag < 128;

    int srcX = srcSquare->getColumn();
    int srcY = srcSquare->getRow();
    int dstX = dstSquare->getColumn();
    int dstY = dstSquare->getRow();

   // En passant capture
    if (gameTag % 128 == Pawn && _enPassant) {
        int enPassantRow = _enPassant->getRow();
        int enPassantCol = _enPassant->getColumn();
        if (dstX == enPassantCol && dstY == enPassantRow) {
            // Remove the captured pawn
            int capturedPawnRow = isWhite ? enPassantRow - 1 : enPassantRow + 1;
            _grid[capturedPawnRow][enPassantCol].destroyBit();
        }
        _enPassant = nullptr;
    }

// Update the en passant square for the next move
    if (gameTag % 128 == Pawn && abs(dstY - srcY) == 2) {
        int enPassantRow = (srcY + dstY) / 2; // Square between the starting and ending rows
        _enPassant = &_grid[enPassantRow][dstX];
    } else {
        _enPassant = nullptr;
    }

    // Pawn promotion
    if (gameTag % 128 == Pawn) {
        int promotionRow = isWhite ? 7 : 0;
        if (dstY == promotionRow) {
            Bit* newBit = PieceForPlayer(isWhite ? 0 : 1, Queen);
            newBit->setPosition(dstSquare->getPosition());
            newBit->setParent(dstSquare);
            newBit->setGameTag(Queen + (isWhite ? 0 : 128));
            dstSquare->setBit(newBit);
            return;
        }
    }

// Update castling rights and move the rook for castling
if (gameTag % 128 == King && abs(dstX - srcX) == 2) {
    handleCastlingRookMove(srcX, dstX, isWhite);
}

// General move updates
_grid[srcY][srcX].setGameTag(0); // Clear the source square
_grid[dstY][dstX].setGameTag(gameTag); // Set the destination square
_grid[dstY][dstX].setBit(&bit); // Assign the piece to the destination square
bit.setParent(&_grid[dstY][dstX]);
bit.setPosition(_grid[dstY][dstX].getPosition());

    // General move updates
    _grid[srcY][srcX].setGameTag(0); // Clear the source square
    _grid[dstY][dstX].setGameTag(gameTag); // Set the destination square

  //  updatePieces(wPieces, true); // Update white bitboard
  //  updatePieces(bPieces, false); // Update black bitboard

    endTurn();
}

void Chess::updatePieces(uint64_t& pieces, bool forWhite) {
    pieces = 0; // Clear current bitboard
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int gameTag = _grid[y][x].gameTag();
            bool isPieceWhite = (gameTag > 0 && gameTag < 128);
            if ((forWhite && isPieceWhite) || (!forWhite && gameTag > 128)) {
                uint64_t bitPosition = 1ULL << (y * 8 + x);
                pieces |= bitPosition; // Add piece position to bitboard
            }
        }
    }
}

std::vector<int> Chess::getRookMoves(int x, int y, bool isWhite) {
    std::vector<int> moves;
    int dx[] = {1, -1, 0, 0}; // Directions for rook (horizontal and vertical)
    int dy[] = {0, 0, 1, -1};

    for (int dir = 0; dir < 4; ++dir) {
        int nx = x, ny = y;
        while (true) {
            nx += dx[dir];
            ny += dy[dir];

            if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) break; // Out of bounds

            Bit* bit = _grid[ny][nx].bit();
            if (bit) {
                if ((bit->gameTag() < 128) != isWhite) moves.push_back(ny * 8 + nx); // Enemy piece
                break; // Blocked
            }
            moves.push_back(ny * 8 + nx); // Empty square
        }
    }
    return moves;
}
std::vector<int> Chess::getBishopMoves(int x, int y, bool isWhite) {
    std::vector<int> moves;
    int dx[] = {1, 1, -1, -1}; // Directions for bishop (diagonals)
    int dy[] = {1, -1, 1, -1};

    for (int dir = 0; dir < 4; ++dir) {
        int nx = x, ny = y;
        while (true) {
            nx += dx[dir];
            ny += dy[dir];

            if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) break; // Out of bounds

            Bit* bit = _grid[ny][nx].bit();
            if (bit) {
                if ((bit->gameTag() < 128) != isWhite) moves.push_back(ny * 8 + nx); // Enemy piece
                break; // Blocked
            }
            moves.push_back(ny * 8 + nx); // Empty square
        }
    }
    return moves;
}
std::vector<int> Chess::getKnightMoves(int x, int y, bool isWhite) {
    std::vector<int> moves;
    int dx[] = {2, 2, -2, -2, 1, -1, 1, -1}; // Knight jumps
    int dy[] = {1, -1, 1, -1, 2, 2, -2, -2};

    for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) continue; // Out of bounds

        Bit* bit = _grid[ny][nx].bit();
        if (!bit || (bit->gameTag() < 128) != isWhite) moves.push_back(ny * 8 + nx); // Empty or enemy
    }
    return moves;
}
std::vector<int> Chess::getPawnMoves(int x, int y, bool isWhite) {
    std::vector<int> moves;
    int forward = isWhite ? 1 : -1; // White moves down (+1), Black moves up (-1)

    // Single forward move
    if (y + forward >= 0 && y + forward < 8 && !_grid[y + forward][x].bit()) {
        moves.push_back((y + forward) * 8 + x);
    }

    // Double forward move (only on initial rank)
    if ((isWhite && y == 1) || (!isWhite && y == 6)) {
        if (!_grid[y + forward][x].bit() && !_grid[y + 2 * forward][x].bit()) {
            moves.push_back((y + 2 * forward) * 8 + x);
        }
    }

    // Diagonal captures
    for (int dx : {-1, 1}) { // Check diagonals
        int nx = x + dx;
        int ny = y + forward;
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            Bit* bit = _grid[ny][nx].bit();
            if (bit && (bit->gameTag() < 128) != isWhite) { // Enemy piece
                moves.push_back(ny * 8 + nx);
            }
        }
    }

    return moves;
}
std::vector<int> Chess::getKingMoves(int x, int y, bool isWhite) {
    std::vector<int> moves;
    int dx[] = {1, -1, 0, 0, 1, -1, 1, -1}; // Adjacent squares
    int dy[] = {0, 0, 1, -1, 1, -1, -1, 1};

    for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) continue; // Out of bounds

        Bit* bit = _grid[ny][nx].bit();
        if (!bit || (bit->gameTag() < 128) != isWhite) moves.push_back(ny * 8 + nx); // Empty or enemy
    }
    return moves;
}

void Chess::handleCastlingMoves(int srcX, int srcY, bool isWhite, std::vector<int>& validMoves) {
    int playerRow = isWhite ? 0 : 7;

    if (srcX == 4 && srcY == playerRow) {
        // King-side castling
        if (_castling[isWhite ? 0 : 2] &&
            _grid[playerRow][5].empty() &&
            _grid[playerRow][6].empty() &&
            !isSquareUnderAttack(playerRow, 4, !isWhite) &&
            !isSquareUnderAttack(playerRow, 5, !isWhite) &&
            !isSquareUnderAttack(playerRow, 6, !isWhite)) {
            validMoves.push_back(playerRow * 8 + 6);
        }

        // Queen-side castling
        if (_castling[isWhite ? 1 : 3] &&
            _grid[playerRow][3].empty() &&
            _grid[playerRow][2].empty() &&
            _grid[playerRow][1].empty() &&
            !isSquareUnderAttack(playerRow, 4, !isWhite) &&
            !isSquareUnderAttack(playerRow, 3, !isWhite) &&
            !isSquareUnderAttack(playerRow, 2, !isWhite)) {
            validMoves.push_back(playerRow * 8 + 2);
        }
    }
}

bool Chess::isSquareUnderAttack(int row, int col, bool byWhite) {
    // Iterate through all squares on the board
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Bit* bit = _grid[y][x].bit();
            if (bit && ((bit->gameTag() < 128) == byWhite)) {
                // Get valid moves for the current piece
                std::vector<int> moves;
                switch (bit->gameTag() % 128) {
                    case Rook: moves = getRookMoves(x, y, byWhite); break;
                    case Bishop: moves = getBishopMoves(x, y, byWhite); break;
                    case Queen:
                        moves = getRookMoves(x, y, byWhite);
                        {
                            auto bishopMoves = getBishopMoves(x, y, byWhite);
                            moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
                        }
                        break;
                    case Knight: moves = getKnightMoves(x, y, byWhite); break;
                    case Pawn: moves = getPawnMoves(x, y, byWhite); break;
                    case King: moves = getKingMoves(x, y, byWhite); break;
                    default: continue; // No valid moves for this piece
                }

                // Check if the square (row, col) is in the valid moves
                if (std::find(moves.begin(), moves.end(), row * 8 + col) != moves.end()) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Chess::handleEnPassantMoves(int srcX, int srcY, int dstX, int dstY, bool isWhite, std::vector<int>& validMoves) {
    if (_enPassant) {
        int enPassantRow = _enPassant->getRow();
        int enPassantCol = _enPassant->getColumn();

        // Check if the move lands on the en passant square
        if (dstX == enPassantCol && dstY == enPassantRow) {
            validMoves.push_back(enPassantRow * 8 + enPassantCol);
        }
    }
}

void Chess::handleCastlingRookMove(int srcX, int dstX, bool isWhite) {
    int playerRow = isWhite ? 0 : 7;

    if (dstX == 6) { // King-side castling
        Bit* rook = _grid[playerRow][7].bit(); // Get the rook at the corner
        if (rook) {
            // Move the rook to its new position
            _grid[playerRow][5].setBit(rook); // Place the rook in the correct new position
            rook->setPosition(_grid[playerRow][5].getPosition());
            rook->setParent(&_grid[playerRow][5]);
            _grid[playerRow][5].setGameTag(rook->gameTag()); // Update the game tag for the destination square
            _grid[playerRow][7].setGameTag(0); // Clear the old square's game tag
            _grid[playerRow][7].setBit(nullptr); // Explicitly clear the source square's bit
        }
    } else if (dstX == 2) { // Queen-side castling
        Bit* rook = _grid[playerRow][0].bit(); // Get the rook at the corner
        if (rook) {
            // Move the rook to its new position
            _grid[playerRow][3].setBit(rook); // Place the rook in the correct new position
            rook->setPosition(_grid[playerRow][3].getPosition());
            rook->setParent(&_grid[playerRow][3]);
            _grid[playerRow][3].setGameTag(rook->gameTag()); // Update the game tag for the destination square
            _grid[playerRow][0].setGameTag(0); // Clear the old square's game tag
            _grid[playerRow][0].setBit(nullptr); // Explicitly clear the source square's bit
        }
    }
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
