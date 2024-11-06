#pragma once
#include <string>
#include <vector>

std::vector<int>  pawnMoves(int position, std::string boardState,  int color);
std::vector<int>  rookMoves(int position, std::string boardState,  int color);
std::vector<int>  bishopMoves(int position, std::string boardState, int color);
std::vector<int>  knightMoves(int position, std::string boardState,  int color);
std::vector<int>  queenMoves(int position, std::string boardState, int color);
std::vector<int>  kingMoves(int position, std::string boardState,  int color);

std::vector<int>  generateMoves(int position, std::string boardState);
