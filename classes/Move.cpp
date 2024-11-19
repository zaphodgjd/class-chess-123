#include "Move.h"

Move::Move(uint8_t from, uint8_t to) {
	move = ((from & squareMask) << 6) | (to & squareMask);
}

Move::Move(uint8_t from, uint8_t to, uint8_t flags) {
	move = (flags << 12) | ((from & squareMask) << 6) | (to & squareMask);
}

Move& Move::operator=(const Move& other) {
	if (this != &other) {
		move = other.move;
	}

	return *this;
}

void Move::setTo(uint8_t to) { 
	move &= ~squareMask;
	move |= to & squareMask;
}

void Move::setFrom(uint8_t from) { 
	// from mask
	move &= ~0b111111000000;
	move |= (from & squareMask) << 6;
}

void Move::setFlags(uint8_t flag) {
	move = (move & ~0xfff) & (flag << 12) | move & 0xfff;
}

// supposed to be safer.
void Move::setFlagBits(uint8_t flag) {
	move = (move & ~0xfff) & (flag << 12) | move & 0xfff;
}

void Move::toggleFlags(uint8_t flag) {
	move = ((move & ~0xfff) ^ (flag << 12)) | move & 0xfff;
}