#include "BitHolder.h"
#include "Bit.h"

BitHolder::~BitHolder()
{
}

//
// const version checks for bit without picking it up from the holder
//
Bit *BitHolder::bit() const
{
	return _bit;
}

Bit *BitHolder::bit()
{
	if (_bit && _bit->getParent() != this && !_bit->getPickedUp())
	{
		_bit = nullptr;
	}
	return _bit;
}

void BitHolder::setBit(Bit *abit)
{
	if (abit != (void *)bit())
	{
		if (_bit)
		{
			delete _bit;
			_bit = nullptr;
		}
		_bit = abit;
		if (_bit)
		{
			_bit->setParent(this);
		}
	}
}

void BitHolder::destroyBit()
{
	if (_bit)
	{
		delete _bit;
		_bit = nullptr;
	}
}

Bit *BitHolder::canDragBit(Bit *bit)
{
	if (bit->getParent() == this && bit->friendly())
	{
		return bit;
	}
	return nullptr;
}

void BitHolder::cancelDragBit(Bit *bit)
{
	setBit(bit);
}

void BitHolder::draggedBitTo(Bit *bit, BitHolder *dst)
{
	setBit(nullptr);
}

bool BitHolder::canDropBitAtPoint(Bit *bit, const ImVec2 &point)
{
	return true;
}

void BitHolder::willNotDropBit(Bit *bit)
{
}

bool BitHolder::dropBitAtPoint(Bit *bit, const ImVec2 &point)
{
	return false;
}

void BitHolder::initHolder(const ImVec2 &position, const ImVec4 &color, const char *spriteName)
{
	setPosition(position.x, position.y);
	setColor(color.x, color.y, color.z, color.w);
	setSize(0, 0);
	setScale(1.0f);
	setLocalZOrder(0);
	setHighlighted(false);
	setGameTag(0);
	setBit(nullptr);
	LoadTextureFromFile(spriteName);
}
