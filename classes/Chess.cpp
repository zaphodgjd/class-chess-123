#include <cmath>
#include "Chess.h"
#include "../tools/Logger.h"
#include <map>

Chess::Chess() {
	// precompute dist.
	// may be possible to do this fancier w/ constexpr
	for (int file = 0; file < 8; file++) {
		for (int rank = 0; rank < 8; rank++) {
			int north = 7 - rank;
			int south = rank;
			int west  = file;
			int east  = 7 - file;

			int i = rank * 8 + file;
			dist[i][0] = north;
			dist[i][1] = east;
			dist[i][2] = south;
			dist[i][3] = west;
			dist[i][4] = std::min(north, west);
			dist[i][5] = std::min(north, east),
			dist[i][6] = std::min(south, east),
			dist[i][7] = std::min(south, west);
		}
	}
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
			_grid[file * 8 + rank].initHolder((ImVec2(rank * 64 + 50, (_gameOps.Y - file) * 64 + 50)),
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

const int dir[8] = {8, 1, -8, -1, 7, 9, -7, -9};
std::vector<std::pair<int, int>> Chess::moveGenerator() const {
	std::vector<std::pair<int, int>> moves;
	for (auto square : pieces) {
		const int piece = square.bit()->gameTag();
		const int index = square.getRow() * 8 + square.getColumn();

		switch (piece) {
			case ChessPiece::Bishop:
			case ChessPiece::Rook:
			case ChessPiece::Queen: {
				// rotate around the piece and discover new spots the sliding pieces can move to.
				int start = piece == ChessPiece::Bishop ? 4 : 0;
				int end   = piece == ChessPiece::Rook   ? 4 : 8;
				for (int i = start; i < end; i++) {
					for (int k = 0; k < dist[index][i]; k++) {
						int targ = start + dir[i] * (k + 1);
						Bit* bit = _grid[targ].bit();

						if (bit) {
							if (bit->friendly(square.bit())) {
								break;
							} else {
								moves.push_back(std::make_pair(index, targ));
								break;
							}
						}

						moves.push_back(std::make_pair(index, targ));
					}
				}
				break; }
			case ChessPiece::Pawn: {
				// determining if we can move two places ahead.
				int moveDir = piece & 8 ? dir[2] : dir[0];
				bool clean  = piece & 8 ? (square.getRow() == 6) : (square.getRow() == 2);

				int targ = index + moveDir;
				if (!_grid[targ].bit()) {
					moves.push_back(std::make_pair(index, targ));
					targ += moveDir;
					if (clean && !_grid[targ].bit()) {
						moves.push_back(std::make_pair(index, targ));
					}
				}
			
				targ = index + moveDir;
				for (int dirIndex : {3, 1}) {
					const int nTarg = targ + dir[dirIndex];
					// First case:  is nTarg to the left of targ?
					// Second case: is nTarg to the right of targ?
					bool lValid = (dirIndex == 3) && (nTarg % _gameOps.X) < (targ  % _gameOps.X);
					bool rValid = (dirIndex == 1) && (targ  % _gameOps.X) < (nTarg % _gameOps.X);
					if (lValid || rValid) {
						Bit* bit = _grid[nTarg].bit();
						if (bit && !bit->friendly(square.bit())) {
							moves.push_back(std::make_pair(index, nTarg));
						}
					}
				}
				break; }
			case ChessPiece::Knight: {
				const int row = square.getRow();
				const int col = square.getColumn();
				const int moveDirs[8][2] = {
					{-2, -1}, {-2, 1}, {2, -1}, {2, 1},
					{-1, -2}, {-1, 2}, {1, -2}, {1, 2}
				};

				for (const auto& move : moveDirs) {
					const int nRow = row + move[0];
					const int nCol = col + move[1];

					if (nRow >= 0 && nRow < 8 && nCol >= 0 && nCol < 8) {
						Bit* bit = _grid[nRow * 8 + nCol].bit();
						if (!bit || (bit && !bit->friendly(square.bit()))) {
							moves.push_back(std::make_pair(index, nRow * 8 + nCol));
						}
					}
				}
				break; }
			case ChessPiece::King: {
				for (int i = 0; i < 8; i++) {
					int targ = index + dir[i];
					Bit* bit = _grid[targ].bit();

					if (bit) {
						if (bit->friendly(square.bit())) {
							break;
						} else {
							moves.push_back(std::make_pair(index, targ));
							break;
						}
					}

					moves.push_back(std::make_pair(index, targ));
				}
				break; }
			default:
				break;
		}
	}

	return moves;
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

// state strings
std::string Chess::initialStateString() {
	return stateString();
}

// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
std::string Chess::stateString() {
	std::string s;
	for (int i = 0; i < _gameOps.size; i++) {
		s += _grid[i].getNotation();
	}

	return s;
}

// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
void Chess::setStateString(const std::string &s) {
	for (int i = 0; i < _gameOps.size; i++) {
		if (s[i] == '0') continue;
		_grid[i].setBit(PieceForPlayer(s[i]));
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
				_grid[file * 8 + rank].setBit(PieceForPlayer(isBlack, piece));
				rank++;
			}
		}
	}
}

// this is the function that will be called by the AI
void Chess::updateAI() {

}