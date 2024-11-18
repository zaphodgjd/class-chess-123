#pragma once
#include <cstdint>

// I'm ultimately conflicted on what size I should store my moves as.
// Since I'm not aiming to do bitboards for this leg of the project (maybe get to in the future?)
// max efficientcy isn't neccesary, and I'd like to have the wiggle room.

// implementaiton borrowed from w/ modificaitons https://www.chessprogramming.org/Encoding_Moves

const uint8_t squareMask = 63U;
class Move {
	public:
	enum FlagCodes {
		// don't need to keep track of EnPassant b/c Double Push already tracks that
		// I included some flags in here for convenience later.
		Capture    = 0b00000001,
		DoublePush = 0b00000010,
		Castling   = 0b00001100,
		KCastle    = 0b00000100,
		QCastle    = 0b00001000,
		Promotion  = 0b11110000,
		ToQueen    = 0b00010000,
		ToKnight   = 0b00100000,
		ToRook     = 0b01000000,
		ToBishop   = 0b10000000,
		// not checking
	};

	Move(uint8_t, uint8_t);
	Move(uint8_t, uint8_t, uint8_t);
	Move(const Move& other) : move(other.move) {}

	Move& operator=(const Move&);
	bool operator==(const Move& a) const { return move == a.move; }
	bool operator!=(const Move& a) const { return move != a.move; }

	uint8_t getTo()	const { return move & squareMask; }
	uint8_t getFrom()  const { return (move >> 6) & squareMask; }
	uint8_t getFlags() const { return (move >> 12); }

	void setTo(uint8_t);
	void setFrom(uint8_t);

	// really wish C++ had C# styled lambdas right now
	bool isCapture()		const { return (move & FlagCodes::Capture)		!= 0; }
	bool isQuiet()			const { return (move & ~FlagCodes::Capture)		!= 0; }
	bool isCastle()			const { return (move & FlagCodes::Castling)		!= 0; }
	bool KingSideCastle()	const { return (move & FlagCodes::KCastle)		!= 0; }
	bool QueenSideCastle()	const { return (move & FlagCodes::QCastle)		!= 0; }
	bool isPromotion()		const { return (move & FlagCodes::Promotion)	!= 0; }
	bool isDoublePush()		const { return (move & FlagCodes::DoublePush)	!= 0; }

	// Future proofing
	uint16_t getButterflyIndex() const { return move & 0x0fff; }

	protected:
	uint32_t move;
};