#pragma once
#include "ChessSquare.h"
#include <vector>
#include <utility>

class ChessAI {
public:
    ChessAI();

    // Method to find the best move for the AI
    std::pair<int, int> findBestMove(const ChessSquare grid[8][8], uint64_t wPieces, uint64_t bPieces, 
                                     ChessSquare* enPassant, bool castlingRights[4], int halfmoveClock, bool isWhiteTurn);

private:
    // Core methods for AI logic
    int negamax(const ChessSquare grid[8][8], int depth, int alpha, int beta, bool isMaximizingPlayer, 
                uint64_t wPieces, uint64_t bPieces, ChessSquare* enPassant, bool castlingRights[4]);
    int evaluateBoard(const ChessSquare grid[8][8], uint64_t wPieces, uint64_t bPieces, bool isWhiteTurn) const;
    std::vector<std::pair<int, int>> generateLegalMoves(const ChessSquare grid[8][8], uint64_t wPieces, 
                                                        uint64_t bPieces, bool isWhiteTurn) const;

    // Helper methods for move generation
    std::vector<int> getPieceMoves(int x, int y, int gameTag, const ChessSquare grid[8][8], bool isWhite) const;
};