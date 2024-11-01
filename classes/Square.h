#pragma once

#include "BitHolder.h"

class Square : public BitHolder
{
public:
    Square() : BitHolder() { _column = 0; _row = 0; }
	// initialize the holder with a position, color, and a sprite
	void	initHolder(const ImVec2 &position, const char *spriteName, const int column, const int row);
private:
    int _column;
    int _row;
};
