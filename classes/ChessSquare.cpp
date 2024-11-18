#include "ChessSquare.h"
#include "Chess.h"
#include "Bit.h"
#include "../tools/Logger.h"

// logging cords is common enough that that making a helper feels justified
std::string formatCords(const float a, const float b) {
	std::ostringstream out;
	out << "(" << std::fixed << std::setprecision(2) << a 
	   << ", " << std::fixed << std::setprecision(2) << b << ")";
	return out.str();
}

std::string formatCords(const int a, const int b) {
	return "(" + std::to_string(a) + ", " + std::to_string(b) + ")";
}

inline char generateNotation(ChessBit* abit) {
	if (abit) {
		const char* w = { "PNBRQK" };
		const char* b = { "pnbrqk" };
		// get the non-coloured piece
		int piece = abit->gameTag() & 7;
		return 8 & abit->gameTag() ? b[piece - 1] : w[piece - 1];
	}
	return '0';
}


void ChessSquare::setBit(Bit* abit) {
	// consider a static_cast IF safe
	ChessBit* bit = dynamic_cast<ChessBit*>(abit);
	if (abit && bit == nullptr) { // abit exists but is not ChessBit
		Loggy.log(Logger::ERROR, "ChessSquare::setBit -- abit must be type ChessBit!");
		throw std::invalid_argument("abit must be type ChessBit!");
	}

	BitHolder::setBit(bit);
	_pieceNotation = generateNotation(bit);
}

void ChessSquare::initHolder(const ImVec2 &position, const char *spriteName, const int column, const int row) {
	_column = column;
	_row = row;
	_posNotation = std::string(1, 'a' + column) + std::to_string(row + 1);
	int odd = (column + row) % 2;
	ImVec4 color = odd ? ImVec4(0.93, 0.93, 0.84, 1.0) : ImVec4(0.48, 0.58, 0.36, 1.0);
	BitHolder::initHolder(position, color, spriteName);
	Loggy.log("Holder created at: " + formatCords(column, row) 
							+ " -- " + formatCords(position.x, position.y));
	setSize(pieceSize, pieceSize);
}

bool ChessSquare::canDropBitAtPoint(Bit *newbit, const ImVec2 &point) {
	if (bit() == nullptr)
		return true;
	// dynamic cast is unneeded b/c gameTag() is not a virtual funciton.

	// xor the gametags to see if we have opposing colors
	if (((bit()->gameTag() & 8) ^ (newbit->gameTag() & 8)) >= 8)
		return true;
	return false;
}

bool ChessSquare::dropBitAtPoint(Bit *newbit, const ImVec2 &point) {
	ChessBit* nBit = dynamic_cast<ChessBit*>(newbit);
	if (nBit == nullptr) {
		Loggy.log(Logger::ERROR, "ChessSquare::dropBitAtPoint -- newbit must be type ChessBit!");
		throw std::invalid_argument("newbit must be type ChessBit!");
	}

	if (bit() == nullptr) {
		setBit(nBit);
		nBit->setParent(this);
		nBit->moveTo(getPosition());
		return true;
	}
	// we're taking a piece!
	if ((bit()->gameTag() ^ nBit->gameTag()) >= 8) {
		setBit(nBit);
		nBit->setParent(this);
		nBit->moveTo(getPosition());
		return true;
	}
	return false;
}

std::string ChessSquare::indexToPosNotation(uint8_t index) {
	int row = index / 8;
	int col = index % 8;
	return std::string(1, 'a' + col) + std::to_string(row + 1);
}

void ChessSquare::setMoveHighlighted(bool highlighted) {
	int odd = (_column + _row) % 2;
	_color = odd ? ImVec4(0.93, 0.93, 0.84, 1.0) : ImVec4(0.48, 0.58, 0.36, 1.0);
	if (highlighted) {
		_color = odd ? ImVec4(0.48, 0.58, 0.36, 1.0) : ImVec4(0.93, 0.93, 0.84, 1.0);
		_color = Lerp(_color, ImVec4(0.75, 0.79, 0.30, 1.0), 0.75);
	}
}