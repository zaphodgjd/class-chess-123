#include <cmath>
#include <stdexcept>
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
	
	// TODO: replace _state with vector & manage it like it's a stack internally.
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

Move* Chess::MoveForPositions(const int i, const int j) {
	for (unsigned int k = 0; k < _moves[i].size(); k++) {
		if (_moves[i][k].getTo() == j) {
			return &_moves[i][k];
		}
	}

	return nullptr;
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
	setStateString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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
		const uint8_t	piece = subject->gameTag() & 7;
		const bool 		black = subject->gameTag() & 8;
		const uint8_t	index = square.getIndex();
		_moves[index].reserve(31);
		int flag = 0;
		// average number of moves per turn
		// https://chess.stackexchange.com/questions/23135/what-is-the-average-number-of-legal-moves-per-turn#24325

		// unless I know a value will be sent to the move constructor,
		// i'm not going to bother making everything a uint b/c readability & highlighting
		switch (piece) {
			case ChessPiece::Rook:
				// awesome logic that'll fall through
				if (index == 0 || index == 56) {
					flag |= Move::FlagCodes::QCastle;
				} else if (index == 7 || index == 63) {
					flag |= Move::FlagCodes::KCastle;
				}
				[[fallthrough]];
			case ChessPiece::Bishop:
			case ChessPiece::Queen: {
				// rotate around the piece and discover new spots the sliding pieces can move to.
				int start = piece == ChessPiece::Bishop ? 4 : 0;
				int end   = piece == ChessPiece::Rook   ? 4 : 8;
				for (int i = start; i < end; i++) {
					for (int k = 0; k < _dist[index][i]; k++) {
						uint8_t targ = index + dir[i] * (k + 1);
						ChessBit* bit = _grid[targ].bit();

						if (bit) {
							if (bit->isAlly(square.bit())) {
								break;
							} else {
								// TODO: calculate flags properly to update
								_moves[index].emplace_back(index, targ, Move::FlagCodes::Capture);
								break;
							}
						}

						_moves[index].emplace_back(index, targ, flag);
					}
				}
				break; }
			case ChessPiece::Pawn: {
				// determining if we can move two places ahead.
				int moveDir = black ? dir[2] : dir[0];
				bool canDPush = black ? (square.getRow() == 6) : (square.getRow() == 1);

				int targ = index + moveDir;
				bool canPromote = targ == (black ? (index % 8) : (index % 8) + 56);
				if (!_grid[targ].bit()) {
					if (canPromote) {
						for (int i = 0; i < 4; i++) {
							_moves[index].emplace_back(index, targ, Move::FlagCodes::ToQueen << i);
						}
					} else {
						_moves[index].emplace_back(index, targ);
						targ += moveDir;
						if (canDPush && !_grid[targ].bit()) {
							// TODO: double check this code, b/c I am VERY TIRED as I'm writing this and there is 100% an oversight here.
							_moves[index].emplace_back(index, targ, Move::FlagCodes::DoublePush);
						}
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
						bool enPassant = _state.top().getEnPassantSquare() == nTarg;
						bool capture   = bit && !bit->isAlly(square.bit());
						// if enpassant square is specified, then we know it's a legal move b/c en passant square is set on previous turn.
						if (enPassant || capture) {
							// Capture + Promotion
							if (canPromote) {
								for (int i = 0; i < 4; i++) {
									_moves[index].emplace_back(index, nTarg, (Move::FlagCodes::ToQueen << i) | Move::FlagCodes::Capture);
								}
							} else {
								_moves[index].emplace_back(index, nTarg, Move::FlagCodes::Capture);
							}
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

						// this will break if I change the flag for capturing later.
						bool canCapture = bit && !bit->isAlly(square.bit());
						if (!bit || canCapture) {
							_moves[index].emplace_back(index, nRow * 8 + nCol, canCapture);
						}
					}
				}
				break; }
			case ChessPiece::King: {
				// TODO: Castling
				for (int i = 0; i < 8; i++) {
					if (_dist[index][i] < 1) continue;
					int targ = index + dir[i];
					ChessBit* bit = _grid[targ].bit();

					if (bit) {
						if (bit->isAlly(square.bit())) {
							continue;
						} else {
							_moves[index].emplace_back(index, targ, Move::FlagCodes::Castling | Move::FlagCodes::Capture);
							continue;
						}
					}
					_moves[index].emplace_back(index, targ, Move::FlagCodes::Castling);
				}

				uint8_t rights = _state.top().getCastlingRights();
				// Queenside
				if ((rights & black ? 0b0100 : 0b0001) != 0) {
					if (!_grid[index - 1].bit() && !_grid[index - 2].bit() && !_grid[index - 3].bit()) {
						_moves[index].emplace_back(index, index - 2, Move::FlagCodes::QCastle);
					}
				}
				// Kingside
				if ((rights & black ? 0b1000 : 0b0010) != 0) {
					if (!_grid[index + 1].bit() && !_grid[index + 2].bit()) {
						_moves[index].emplace_back(index, index + 2, Move::FlagCodes::KCastle);
					}
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
		for (Move move : _moves[i]) {
			uint8_t attacking = move.getTo();
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
	const uint8_t i = srcSquare.getIndex();
	const uint8_t j = dstSquare.getIndex();
	for (Move move : _moves[i]) {
		if (move.getTo() == j) {
			return true;
		}
	}

	return false;
}

// borrow graeme's code; note, game calls this function and unless we want to call base we'll need to specifically end turn here.
void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
	ChessSquare& srcSquare = static_cast<ChessSquare&>(src);
	ChessSquare& dstSquare = static_cast<ChessSquare&>(dst);

	// get the move being played
	const uint8_t i = srcSquare.getIndex();
	const uint8_t j = dstSquare.getIndex();

	// this line currently garauntees that we'll auto turn into a queen b/c queen promotion option is always pushed first.
	// eventually when I make a gui for it, we'll need to revise this to handle there being multiple "moves" for a single position.
	Move* move = MoveForPositions(i, j);

	if (!move) {
		throw std::runtime_error("Illegal Move attempted ft: " + std::to_string(i) + " " + std::to_string(j));
	}

	// EnPassant Check
	if (_state.top().getEnPassantSquare() == j) {
		_grid[j + (_state.top().isBlackTurn() ? 8 : -8)].destroyBit();
		// increment score.
	} else if (move->isCastle() && ((bit.gameTag() & ChessPiece::King) == ChessPiece::King)) { // castle
		uint8_t offset = _state.top().isBlackTurn() ? 56 : 0;
		uint8_t rookSpot = (move->QueenSideCastle() ? 0 : 7) + offset;
		uint8_t targ = (move->QueenSideCastle() ? 3 : 5) + offset;
		_grid[targ].setBit(_grid[rookSpot].bit());
		_grid[rookSpot].setBit(nullptr);
	} else if (move->isPromotion()) {
		// todo, but for the moment b/c of how our move is selected, queen will be only "move" we can make.
		int newPiece = 0;
		switch(move->getFlags() & Move::FlagCodes::Promotion) {
			case Move::FlagCodes::ToQueen:
				newPiece = ChessPiece::Queen;
				break;
			case Move::FlagCodes::ToKnight:
				newPiece = ChessPiece::Knight;
				break;
			case Move::FlagCodes::ToRook:
				newPiece = ChessPiece::Rook;
				break;
			case Move::FlagCodes::ToBishop:
				newPiece = ChessPiece::Bishop;
				break;
		}

		// awesome cast
		dstSquare.setBit(PieceForPlayer(_state.top().isBlackTurn(), (ChessPiece)newPiece));
	}

	// check if we took a rook
	_state.emplace(_state.top(), *move);
	if (j == 63 || j == 56 || j == 0 || j == 7) {
		uint8_t flag = _state.top().getCastlingRights();
		if (j == 56 || j == 0) {
			flag &= _state.top().isBlackTurn() ? ~0b0001 : ~0b0100;
		} else if (j == 63 || j == 7) {
			flag &= _state.top().isBlackTurn() ? ~0b0010 : ~0b1000;
		}
		_state.top().setCastlingRights(flag);
	}

	// do some check to prompt the UI to select a promotion.

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
	uint8_t emptyCount;

	int file = 7, rank = 0;
	for (int i = 0; i < _gameOps.size; i++) {
		char piece = _grid[file * 8 + rank].getPieceNotation();
		rank++;

		if (piece == '0') { // Empty square
			emptyCount++;
		} else {
			if (emptyCount > 0) {
				s += std::to_string(emptyCount); // Append the count of empty squares
				emptyCount = 0; // Reset count
			}
			s += piece; // Append the piece notation
		}
		
		// Handle row breaks for FEN notation
		if ((i + 1) % 8 == 0) {
			if (emptyCount > 0) {
				s += std::to_string(emptyCount); // Append remaining empty squares at end of row
				emptyCount = 0;
			}
			if (i != (_gameOps.size - 1U)) {
				s += '/'; // Add row separator
				rank = 0;
				file--;
			}
		}
	}

	s += getCurrentPlayer()->playerNumber() ? " b " : " w ";
	std::string castlingRights;
	{
		uint8_t rights = _state.top().getCastlingRights();
		if (rights != 0) {
			if (rights & 0b1000) castlingRights += 'K';
			if (rights & 0b0100) castlingRights += 'Q';
			if (rights & 0b0010) castlingRights += 'k';
			if (rights & 0b0001) castlingRights += 'q';
		} else {
			castlingRights += '-';
		}
	}
	s += castlingRights;

	{
		uint8_t enP = _state.top().getEnPassantSquare();
		if (enP < 64) {
			s += ' ' + _grid[enP].getPositionNotation() + ' ';
		} else {
			s += " - ";
		}
	}

	s += std::to_string((int)_state.top().getHalfClock()) + ' ' + std::to_string((int)_state.top().getClock());

	return s;
}

// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
// modified from Sebastian Lague's Coding Adventure on Chess. 2:37
void Chess::setStateString(const std::string& fen) {
	size_t i = 0;
	{ int file = 7, rank = 0;
	for (; i < fen.size(); i++) {
		const char symbol = fen[i];
		if (symbol == ' ') { // terminating when reaching turn indicator
			break;
		}

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
	}}

	// extract the game state part of FEN
	i++;
	bool isBlack = (fen[i] == 'b');
	i += 2;

	uint8_t castling = 0;
	while (i < fen.size() && fen[i] != ' ') {
		switch (fen[i++]) {
			case 'K': castling |= 1 << 3; break;
			case 'Q': castling |= 1 << 2; break;
			case 'k': castling |= 1 << 1; break;
			case 'q': castling |= 1; break;
			case '-': castling  = 0; break;
		}
	}
	i++;

	uint8_t enTarget = 255;
	if (fen[i] != '-') {
		int col	= fen[i++] - 'a';
		int row	= fen[i++] - '1';

		// Combine both to form a unique 8-bit value (8 * row + column)
		enTarget = (row << 3) | col;
	}
	i++;
	
	uint8_t  hClock = 0;
	uint16_t fClock = 0;
	while (std::isdigit(fen[++i])) { hClock = hClock * 10 + (fen[i] - '0'); }
	while (std::isdigit(fen[++i])) { fClock = fClock * 10 + (fen[i] - '0'); }

	_state.emplace(fen, isBlack, castling, enTarget, hClock, fClock);
}

// this is the function that will be called by the AI
void Chess::updateAI() {

}