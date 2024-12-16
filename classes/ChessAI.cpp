#include "ChessAI.h"
#include <limits>
#include <algorithm>


ChessAI::ChessAI() {}

std::pair<int, int> ChessAI::findBestMove(const ChessSquare grid[8][8], uint64_t wPieces, uint64_t bPieces, 
                                          ChessSquare* enPassant, bool castlingRights[4], int halfmoveClock, bool isWhiteTurn) {
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};

    std::vector<std::pair<int, int>> legalMoves = generateLegalMoves(grid, wPieces, bPieces, isWhiteTurn);

    for (const auto& move : legalMoves) {
        // Copy the board to simulate the move
        ChessSquare simulatedGrid[8][8];
        std::copy(&grid[0][0], &grid[0][0] + 64, &simulatedGrid[0][0]);

        uint64_t wPiecesSim = wPieces, bPiecesSim = bPieces;
        ChessSquare* enPassantSim = enPassant;
        bool castlingRightsSim[4];
        std::copy(castlingRights, castlingRights + 4, castlingRightsSim);

        // Apply the move
        int srcX = move.first % 8, srcY = move.first / 8;
        int dstX = move.second % 8, dstY = move.second / 8;
        simulatedGrid[dstY][dstX].setBit(simulatedGrid[srcY][srcX].bit());
        simulatedGrid[srcY][srcX].setBit(nullptr);

        int score = -negamax(simulatedGrid, 3, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 
                             !isWhiteTurn, wPiecesSim, bPiecesSim, enPassantSim, castlingRightsSim);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int ChessAI::negamax(const ChessSquare grid[8][8], int depth, int alpha, int beta, bool isMaximizingPlayer, 
                     uint64_t wPieces, uint64_t bPieces, ChessSquare* enPassant, bool castlingRights[4]) {
    if (depth == 0) {
        return evaluateBoard(grid, wPieces, bPieces, isMaximizingPlayer);
    }

    int maxEval = std::numeric_limits<int>::min();

    std::vector<std::pair<int, int>> legalMoves = generateLegalMoves(grid, wPieces, bPieces, isMaximizingPlayer);

    for (const auto& move : legalMoves) {
        ChessSquare simulatedGrid[8][8];
        std::copy(&grid[0][0], &grid[0][0] + 64, &simulatedGrid[0][0]);

        // Apply the move and recurse
        int srcX = move.first % 8, srcY = move.first / 8;
        int dstX = move.second % 8, dstY = move.second / 8;
        simulatedGrid[dstY][dstX].setBit(simulatedGrid[srcY][srcX].bit());
        simulatedGrid[srcY][srcX].setBit(nullptr);

        int eval = -negamax(simulatedGrid, depth - 1, -beta, -alpha, !isMaximizingPlayer, 
                            wPieces, bPieces, enPassant, castlingRights);

        maxEval = std::max(maxEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) break;
    }

    return maxEval;
}

int ChessAI::evaluateBoard(const ChessSquare grid[8][8], uint64_t wPieces, uint64_t bPieces, bool isWhiteTurn) const {
    static const int pieceValues[] = {0, 1, 3, 3, 5, 9, 100}; // Pawn, Knight, Bishop, Rook, Queen, King

    int score = 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const Bit* bit = grid[y][x].bit();
            if (bit) {
                int gameTag = bit->gameTag();
                int pieceType = gameTag % 128;
                bool isWhite = gameTag < 128;

                int value = pieceValues[pieceType];
                score += (isWhite == isWhiteTurn ? value : -value);
            }
        }
    }

    return score;
}

std::vector<std::pair<int, int>> ChessAI::generateLegalMoves(const ChessSquare grid[8][8], uint64_t wPieces, 
                                                             uint64_t bPieces, bool isWhiteTurn) const {
    std::vector<std::pair<int, int>> moves;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const Bit* bit = grid[y][x].bit();
            if (bit) {
                int gameTag = bit->gameTag();
                if ((gameTag < 128) == isWhiteTurn) {
                    std::vector<int> pieceMoves = getPieceMoves(x, y, gameTag, grid, isWhiteTurn);
                    for (int move : pieceMoves) {
                        moves.emplace_back(y * 8 + x, move);
                    }
                }
            }
        }
    }

    return moves;
}

std::vector<int> ChessAI::getPieceMoves(int x, int y, int gameTag, const ChessSquare grid[8][8], bool isWhite) const {
    // Reuse the `Chess::getRookMoves`, `Chess::getKnightMoves`, etc., logic here or adapt it as necessary.
    return {};
}