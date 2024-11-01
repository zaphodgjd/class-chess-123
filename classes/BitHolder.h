#pragma once
#include "Sprite.h"

class Bit;

class BitHolder : public Sprite
{
public:
	BitHolder() : Sprite() { _bit = nullptr; _gameTag = 0; };
	~BitHolder();

	// current piece or nullptr if empty
	Bit		*bit() const;
	Bit		*bit();
	// set the current piece
	void	setBit(Bit* bit);
	// destroy the current piece, triggering any associated animations
	void	destroyBit();
	// gametag can be used by games for any purpose
	int		gameTag() { return _gameTag; };
	// set the gametag
	void	setGameTag(int tag) { _gameTag = tag; };
	// convenience function to see if the holder is empty
	virtual bool	empty() { return _bit == nullptr; };

	// can you drag this bit from this holder? if not, return a different bit to drag instead, or nullptr if not allowed
	// cancelDragBit or draggedBitTo must be called next
	// an example of this would be return the bit if it can move to a different holder, or nullptr if it can't move at all
	virtual Bit		*canDragBit(Bit *bit);

	// can this piece be moved here? if not, return false
	// either willNotDropBit or dropBitAtPoint must be called next
	// an example of this would be an empty holder that only accepts kings
	virtual bool	canDropBitAtPoint(Bit *bit, const ImVec2& point);

	// cancel a pending drag, returning anything that needs returned
	virtual void	cancelDragBit(Bit *bit);

	// call when the drag is finished to score and so forth
	virtual void	draggedBitTo(Bit *bit, BitHolder *dst);

	// cancels a pending drop, returning anything that needs returned
	virtual void	willNotDropBit(Bit *bit);

	// finsihes up the drop
	virtual bool	dropBitAtPoint(Bit *bit, const ImVec2& point);

	// initialize the holder with a position, color, and a sprite
	virtual void	initHolder(const ImVec2 &position, const ImVec4 &color, const char *spriteName);

protected:
	Bit		*_bit;
	int		_gameTag;
};

