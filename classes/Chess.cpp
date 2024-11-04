#include <cmath>
#include "Chess.h"
#include "../tools/Logger.h"

Chess::Chess() {

}

Chess::~Chess() {

}

// make a chess piece for the player
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece) {
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

	for (int y = _gameOps.Y - 1; y >= 0; y--) {
		for (int x = 0; x < _gameOps.X; x++) {
			_grid[y * 8 + x].initHolder((ImVec2(x * 64 + 50, y * 64 + 50)), "square.png", x, y);
		}
	}

	if (gameHasAI()) {
		setAIPlayer(_gameOps.AIPlayer);
	}

	startGame();
}

bool Chess::actionForEmptyHolder(BitHolder &holder) {
	return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src) {
	return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
	return false;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
	
}

// free all the memory used by the game on the heap
void Chess::stopGame() {

}

Player* Chess::checkForWinner() {
	// check to see if either player has won
	return nullptr;
}

bool Chess::checkForDraw() {
	// check to see if the board is full
	return false;
}

// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
const char Chess::bitToPieceNotation(int row, int column) const {
	return bitToPieceNotation(row * column);
}

const char Chess::bitToPieceNotation(int i) const {
	if (i < 0 || i > _gameOps.size) {
		return '0';
	}

	// we represent White as uppercase and Black as lower case. 
	const char* w = { "?PNBRQK" };
	const char* b = { "?pnbrqk" };
	unsigned char notation = '0';

	// todo: figure out what the 128, 127 masking is doing.
	Bit* bit = _grid[i].bit();
	if (bit) {
		notation = bit->gameTag() < 128 ? w[bit->gameTag()] : b[bit->gameTag() & 127];
	} else {
		notation = '0';
	}

	return notation;
}

// state strings
std::string Chess::initialStateString() {
	return stateString();
}

// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
std::string Chess::stateString() {
	std::string s;
	for (int i = 0; i < _gameOps.size; i++) {
		s += bitToPieceNotation(i);
	}
	for (int y = 0; y < _gameOps.Y; y++) {
		for (int x = 0; x < _gameOps.X; x++) {
			s += bitToPieceNotation(y, x);
		}
	}
	return s;
}

// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
void Chess::setStateString(const std::string &s) {
	for (int i = 0; i < _gameOps.size; i++) {
		int playerNumber = s[i] - '0';
		if (playerNumber) {
			_grid[i].setBit(PieceForPlayer(playerNumber - 1, Pawn));
		} else {
			_grid[i].setBit(nullptr);
		}
	}
}

// this is the function that will be called by the AI
void Chess::updateAI() {

}