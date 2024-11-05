#include <cmath>
#include "Chess.h"
#include "../tools/Logger.h"
#include <map>

Chess::Chess() {

}

Chess::~Chess() {

}

const std::map<char, ChessPiece> pieceFromSymbol = {
	{'p', ChessPiece::Pawn},
	{'n', ChessPiece::Knight},
	{'b', ChessPiece::Bishop},
	{'r', ChessPiece::Rook},
	{'q', ChessPiece::Queen},
	{'k', ChessPiece::King}
};

// make a chess piece for the player
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece) {
	const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };
	Bit* bit = new Bit();

	// we could maybe cache this to make things simpler.
	const char* pieceName = pieces[piece - 1];
	std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
	bit->LoadTextureFromFile(spritePath.c_str());
	bit->setOwner(getPlayerAt(playerNumber));

	/*	Sebastian opted to use the 4th and 5th bit to denote if a piece is black or white,
		but this seems like a bit of an oversight on his part, and it arguably makes more sense
		in the context of this code to simply use the 4th bit to denote the color of a piece.
		*/
	bit->setGameTag(playerNumber << 3 | piece);
	bit->setSize(pieceSize, pieceSize);

	return bit;
}

// we DON'T error check here b/c of overhead. Check it yourself!
Bit* Chess::PieceForPlayer(const char piece) {
	return PieceForPlayer((int)!std::isupper(piece), pieceFromSymbol.at(std::tolower(piece)));
}

void Chess::setUpBoard() {
	setNumberOfPlayers(2);

	// It upsets me greatly that, as far as I can tell, a double loop is unfortunately required.
	for (int file = _gameOps.Y - 1; file >= 0; file--) {
		for (int rank = 0; rank < _gameOps.X; rank++) {
			// Unfortunately the _gameOps.Y - y part is neccesary to get this to display properly.
			_grid[file][rank].initHolder((ImVec2(rank * 64 + 50, (_gameOps.Y - file) * 64 + 50)),
									"square.png", rank, file);
			// game tag init to 0
			// notation is set later.
		}
	}

	if (gameHasAI()) {
		setAIPlayer(_gameOps.AIPlayer);
	}

	// Seems like a good idea to start the game using Fen notation, so I can easily try different states for testing.
	setStateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	//startGame();
}

bool Chess::actionForEmptyHolder(BitHolder &holder) {
	return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src) {
	return true;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
	return true;
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
const char Chess::bitToPieceNotation(int rank, int file) const {
	if (rank < 0 || file < 0 || rank > _gameOps.X || file > _gameOps.Y) {
		return '0';
	}

	// we represent White as uppercase and Black as lower case. 
	const char* w = { "PNBRQK" };
	const char* b = { "pnbrqk" };
	unsigned char notation = '0';

	Bit* bit = _grid[file][rank].bit();
	if (bit) {
		// get the actual piece
		int piece = bit->gameTag() & 7;
		notation = 8 & bit->gameTag() ? b[piece - 1] : w[piece - 1];
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
	for (int file = 0; file < _gameOps.Y; file++) {
		for (int rank = 0; rank < _gameOps.X; rank++) {
			s += bitToPieceNotation(rank, file);
		}
	}

	return s;
}

// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
void Chess::setStateString(const std::string &s) {
	for (int file = 0; file < _gameOps.Y; file++) {
		for (int rank = 0; rank < _gameOps.X; rank++) {
			if (s[file * 8 + rank] == '0') continue; // checking here to avoid overhead downstream.
			_grid[file][rank].setBit(PieceForPlayer(s[file * 8 + rank]));
		}
	}
	// BitHolders are init'd with a null ref on bit, so there's no point in setting that unless neccesary.
}

// lifted from Sebastian Lague's Coding Adventure on Chess. 2:37
void Chess::setStateFromFen(const std::string &fen) {
	int file = 7, rank = 0;
	for (const char symbol : fen) {
		if (symbol == '/') {
			rank = 0;
			file--;
		} else {
			// this is for the gap syntax.
			if (std::isdigit(symbol)) {
				rank += symbol - '0';
			} else { // there is a piece here
				// b/c white is considered as "0" elsewhere in the code, it makes
				// more sense to specifically check ifBlack, even if FEN has it the
				// other way around.
				int isBlack = !std::isupper(symbol);
				ChessPiece piece = pieceFromSymbol.at(std::tolower(symbol));
				_grid[file][rank].setBit(PieceForPlayer(isBlack, piece));
				rank++;
			}
		}
	}
}

// this is the function that will be called by the AI
void Chess::updateAI() {

}