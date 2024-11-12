#include <cmath>
#include "Chess.h"
#include "../tools/Logger.h"

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
			_dist[i][0] = north;
			_dist[i][1] = east;
			_dist[i][2] = south;
			_dist[i][3] = west;
			_dist[i][4] = std::min(north, east);
			_dist[i][5] = std::min(south, east),
			_dist[i][6] = std::min(south, west),
			_dist[i][7] = std::min(north, west);
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
ChessBit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece) {
	const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };
	ChessBit* bit = new ChessBit();

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
ChessBit* Chess::PieceForPlayer(const char piece) {
	return PieceForPlayer((int)!std::isupper(piece), pieceFromSymbol.at(std::tolower(piece)));
}

void Chess::setUpBoard() {
	setNumberOfPlayers(2);

	// It upsets me greatly that, as far as I can tell, a double loop is unfortunately required.
	for (int file = _gameOps.Y - 1; file >= 0; file--) {
		for (int rank = 0; rank < _gameOps.X; rank++) {
			// Unfortunately the _gameOps.Y - y part is neccesary to get this to display properly.
			_grid[file * 8 + rank].initHolder((ImVec2(rank * 64 + 50, (_gameOps.Y - file) * 64 + 50)),
									"chess/square.png", rank, file);
			// game tag init to 0
			// notation is set later.
		}
	}

	if (gameHasAI()) {
		setAIPlayer(_gameOps.AIPlayer);
	}

	// Seems like a good idea to start the game using Fen notation, so I can easily try different states for testing.
	setStateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	startGame();
	moveGenerator();
}

const int dir[8] = {8, 1, -8, -1, 9, -7, -9, 7};
void Chess::moveGenerator() {
	// this isn't optimised the best; in the future we'll want to use bitboards instead.
	_moves.clear();
	for (ChessSquare& square : _grid) {
		// we only do half of the moves b/c we'll have to recalculate all moves next turn anyway
		ChessBit* subject = square.bit();
		if (!subject || subject->getOwner() != getCurrentPlayer()) continue;
		const int  piece = subject->gameTag() & 7;
		const bool black = subject->gameTag() & 8;
		const int  index = square.getRow() * 8 + square.getColumn();

		switch (piece) {
			case ChessPiece::Bishop:
			case ChessPiece::Rook:
			case ChessPiece::Queen: {
				// rotate around the piece and discover new spots the sliding pieces can move to.
				int start = piece == ChessPiece::Bishop ? 4 : 0;
				int end   = piece == ChessPiece::Rook   ? 4 : 8;
				for (int i = start; i < end; i++) {
					for (int k = 0; k < _dist[index][i]; k++) {
						int targ = index + dir[i] * (k + 1);
						ChessBit* bit = _grid[targ].bit();

						if (bit) {
							if (bit->isAlly(square.bit())) {
								break;
							} else {
								_moves[index].push_back(targ);
								break;
							}
						}

						_moves[index].push_back(targ);
					}
				}
				break; }
			case ChessPiece::Pawn: {
				// determining if we can move two places ahead.
				int moveDir = black ? dir[2] : dir[0];
				bool clean  = black ? (square.getRow() == 6) : (square.getRow() == 1);

				int targ = index + moveDir;
				if (!_grid[targ].bit()) {
					_moves[index].push_back(targ);
					targ += moveDir;
					if (clean && !_grid[targ].bit()) {
						_moves[index].push_back(targ);
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
						ChessBit* bit = _grid[nTarg].bit();
						if (bit && !bit->isAlly(square.bit())) {
							_moves[index].push_back(nTarg);
						}
					}
				}
				break; }
			// for future: consider pre-calculating all possible moves for knight: procedural is fine for now.
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
						ChessBit* bit = _grid[nRow * 8 + nCol].bit();
						if (!bit || (bit && !bit->isAlly(square.bit()))) {
							_moves[index].push_back(nRow * 8 + nCol);
						}
					}
				}
				break; }
			case ChessPiece::King: {
				for (int i = 0; i < 8; i++) {
					if (_dist[index][i] < 1) continue;
					int targ = index + dir[i];
					ChessBit* bit = _grid[targ].bit();

					if (bit) {
						if (bit->isAlly(square.bit())) {
							continue;
						} else {
							_moves[index].push_back(targ);
							continue;
						}
					}

					_moves[index].push_back(targ);
				}
				break; }
			default:
				break;
		}
	}
}

// nothing for chess
// Consider adding support for clicking on highlighted positions to allow insta moving. Could be cool.
bool Chess::actionForEmptyHolder(BitHolder &holder) {
	return false;
}

bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src) {
	// un-lit the squares when clicking on a new square.
	clearPositionHighlights();

	ChessSquare& srcSquare = static_cast<ChessSquare&>(src);
	bool canMove = false;
	const int i = srcSquare.getIndex();

	if (_moves.count(i)) {
		canMove = true;
		for (int attacking : _moves[i]) {
			_grid[attacking].setMoveHighlighted(true);
			_litSquare.push(&_grid[attacking]);
			Loggy.log("Pushed to lit: " + std::to_string(attacking));
		}
	}
	return canMove;
}

// Is the piece allowed to move here?
bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
	ChessSquare& srcSquare = static_cast<ChessSquare&>(src);
	ChessSquare& dstSquare = static_cast<ChessSquare&>(dst);
	const int i = srcSquare.getIndex();
	const int j = dstSquare.getIndex();
	for (int pos : _moves[i]) {
		if (pos == j) {
			return true;
		}
	}

	return false;
}

// borrow graeme's code; note, game calls this function and unless we want to call base we'll need to specifically end turn here.
void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
	// call base.
	Game::bitMovedFromTo(bit, src, dst);
	clearPositionHighlights();
	moveGenerator();
}

inline void Chess::clearPositionHighlights() {
	while (!_litSquare.empty()) {
		_litSquare.top()->setMoveHighlighted(false);
		_litSquare.pop();
	}
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
void Chess::setStateFromFEN(const std::string &fen) {
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