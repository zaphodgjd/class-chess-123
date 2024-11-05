#pragma once

#include "BitHolder.h"

class ChessSquare : public BitHolder {
public:
	ChessSquare() : BitHolder() {
		_column = 0;
		_row = 0;
		_notation = '0'; // init to false
	}

	void setBit(Bit *bit) override;

	// initialize the holder with a position, color, and a sprite
	void initHolder(const ImVec2 &position, const char *spriteName, const int column, const int row);
	bool canDropBitAtPoint(Bit *bit, const ImVec2 &point) override;
	bool dropBitAtPoint(Bit *bit, const ImVec2 &point) override;

	char getNotation() { return _notation; }
	void setMoveHighlighted(bool highlight);

	// I'm confused by what the point of this is.
	int getDistance(const ChessSquare &other) {
		int columnDistance = abs(_column - other._column);
		int rowDistance = abs(_row - other._row);
		return columnDistance > rowDistance ? columnDistance : rowDistance;
	}

	int getColumn() { return _column; }
	int getRow() { return _row; }
	int getIndex() { return _row * 8 + _column; }

private:
	ImVec4 Lerp(ImVec4 a, ImVec4 b, float t) {
		return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
	}
	int _column;
	int _row;
	char _notation;
};