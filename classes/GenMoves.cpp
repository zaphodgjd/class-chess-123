#include "GenMoves.h"

#define WHITE 0
#define BLACK 1

std::vector<int>  pawnMoves(int position, std::string boardState, int color){
	std::vector<int> moves;
	char piece = boardState[position];
	if (piece == *"P"){
		moves.push_back(position+=8);
	}
	else {
		moves.push_back(position-=8);
	}
	return moves;
}
std::vector<int>  rookMoves(int position, std::string boardState, int color){
	std::vector<int> moves;
	char piece = boardState[position];
	for (int x = position + 8; x < 64; x+= 8){
		if (boardState[x] != *"0"){
			moves.push_back(x);
			break;
		}
		moves.push_back(x);
	}
	for (int x = position - 8; x >= 0; x-= 8){
		if (boardState[x] != *"0"){
			moves.push_back(x);
			break;
		}
		moves.push_back(x);
	}
	for (int x = (position%8) + 1, y = 1; x < 8; x += 1, y++){
		if (boardState[position + y] != *"0"){
			moves.push_back(position + y);
			break;
		}
		moves.push_back(position + y);
	}
	for (int x = (position%8) - 1, y = 1; x >= 0; x -= 1, y++){
		if (boardState[position - y] != *"0"){
			moves.push_back(position - y);
			break;
		}
		moves.push_back(position - y);
	}

	return moves;
}
std::vector<int>  bishopMoves(int position, std::string boardState, int color){
	std::vector<int> moves;
	char piece = boardState[position];
	for (int x = (position%8) + 1, y = 1; x < 8; x += 1, y++){
		if (boardState[position + (y*9)] != *"0"){
			moves.push_back(position + (y*9));
			break;
		}
		moves.push_back(position + (y*9));
	}
	for (int x = (position%8) - 1, y = 1; x >= 0; x -= 1, y++){
		if (boardState[position - (y*9)] != *"0"){
			moves.push_back(position - (y*9));
			break;
		}
		moves.push_back(position - (y*9));
	}
	for (int x = (position%8) + 1, y = 1; x < 8; x += 1, y++){
		if (boardState[position - (y*7)] != *"0"){
			moves.push_back(position - (y*7));
			break;
		}
		moves.push_back(position - (y*7));
	}
	for (int x = (position%8) - 1, y = 1; x >= 0; x -= 1, y++){
		if (boardState[position + (y*7)] != *"0"){
			moves.push_back(position + (y*7));
			break;
		}
		moves.push_back(position + (y*7));
	}
	return moves;
}
std::vector<int>  knightMoves(int position, std::string boardState, int color);
std::vector<int>  queenMoves(int position, std::string boardState, int color){
	std::vector<int> rook = rookMoves(position, boardState, color);
	std::vector<int> bishop = bishopMoves(position, boardState, color);
	rook.insert(rook.end(), bishop.begin(), bishop.end());
	return rook;
}
std::vector<int>  kingMoves(int position, std::string boardState, int color);
std::vector<int>  generateMoves(int position, std::string boardState){
	char piece = boardState[position];
	int color = isupper(piece);
	std::vector<int> moves;
	if (piece == *"p" || piece == *"P"){
		moves = pawnMoves(position, boardState, color);
	} else if (piece == *"r" || piece == *"R"){
		moves = rookMoves(position, boardState, color);
	} else if (piece == *"b" || piece == *"B"){
		moves = bishopMoves(position, boardState, color);
	} /*else if (piece == *"n" || piece == *"N"){
		moves = knightMoves(position, boardState, color);
	} else if (piece == *"q" || piece == *"Q"){
		moves = queenMoves(position, boardState, color);
	} else if (piece == *"k" || piece == *"K"){
		moves = kingMoves(position, boardState, color);
	}*/
	return queenMoves(position, boardState, color);
	
}
