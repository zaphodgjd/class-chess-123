#include "GenMoves.h"

#define WHITE 0
#define BLACK 1

std::vector<int>  pawnMoves(int position, std::string boardState, int color){
	std::vector<int> moves;
	char piece = boardState[position];
	if (piece == *"P"){
		if (boardState[position + 8] == *"0"){

			moves.push_back(position + 8);
			if ((position/8) == 1){
				moves.push_back(position + 16);
			}
		}
		if (boardState[position + 7] != *"0"){
			moves.push_back(position + 7);
		}
		if (boardState[position + 9] != *"0"){
			moves.push_back(position + 9);
		}
	}
	else {
		if (boardState[position - 8] == *"0"){
			moves.push_back(position - 8);
			if ((position/8) == 6){
				moves.push_back(position - 16);
			}
		}
		if (boardState[position - 7] != *"0"){
			moves.push_back(position - 7);
		}
		if (boardState[position - 9] != *"0"){
			moves.push_back(position - 9);
		}
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
std::vector<int>  knightMoves(int position, std::string boardState, int color){
	std::vector<int> moves;
	if (position%8 != 0){
		moves.push_back(position - 17);
		moves.push_back(position + 15);
	}
	if (position%8 != 8){
		moves.push_back(position - 15);
		moves.push_back(position + 17);
	}
	if (position%8 >= 2){
		moves.push_back(position - 10);
		moves.push_back(position + 6);
	}
	if (position%8 <= 5){
		moves.push_back(position + 10);
		moves.push_back(position - 6);
	}
	return moves;
}

std::vector<int>  queenMoves(int position, std::string boardState, int color){
	std::vector<int> rook = rookMoves(position, boardState, color);
	std::vector<int> bishop = bishopMoves(position, boardState, color);
	rook.insert(rook.end(), bishop.begin(), bishop.end());
	return rook;
}
std::vector<int>  kingMoves(int position, std::string boardState, int color){
	std::vector<int> moves;
	if (color == 1 && position == 4){
		if (boardState[0] == *"R" && boardState[1] == *"0" && boardState[2] == *"0" && boardState[3] == *"0" ){
			moves.push_back(128);
		}
		if (boardState[7] == *"R" && boardState[6] == *"0" && boardState[5] == *"0" ){
			moves.push_back(129);
		}
	}
	if (color == 0 && position == 60){
		if (boardState[56] == *"r" && boardState[57] == *"0" && boardState[58] == *"0" && boardState[59] == *"0" ){
			moves.push_back(128);
		}
		if (boardState[63] == *"r" && boardState[62] == *"0" && boardState[61] == *"0" ){
			moves.push_back(129);
		}
	}
	
	moves.push_back(position + 8);
	moves.push_back(position - 8);
	if (position%8 > 0){
		moves.push_back(position - 9);
		moves.push_back(position - 1);
		moves.push_back(position + 7);
	}
	if (position%8 < 7){
		moves.push_back(position - 7);
		moves.push_back(position + 1);
		moves.push_back(position + 9);
	}

	return moves;
}
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
	} else if (piece == *"n" || piece == *"N"){
		moves = knightMoves(position, boardState, color);
	} else if (piece == *"q" || piece == *"Q"){
		moves = queenMoves(position, boardState, color);
	} else if (piece == *"k" || piece == *"K"){
		moves = kingMoves(position, boardState, color);
	}
	return moves;
	
}
