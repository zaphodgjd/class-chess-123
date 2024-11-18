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