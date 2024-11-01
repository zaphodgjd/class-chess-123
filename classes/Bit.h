#pragma once

#include "Sprite.h"

class Player;
class BitHolder;

//
// these aren't used yet but will be used for dragging pieces
//
#define kPickedUpScale 1.2f
#define kPickedUpOpacity 255

enum bitz
{
	kBoardZ = 0,
	kPieceZ = 3,
	kPickupUpZ = 9920,
	kMovingZ = 9930
};

class Bit : public Sprite
{
public:
	Bit() : Sprite()
	{
		_pickedUp = false;
		_owner = nullptr;
		_gameTag = 0;
		_entityType = EntityBit;
		_moving = false;
	};

	~Bit();

	// helper functions
	bool getPickedUp();
	void setPickedUp(bool yes);

	// am I in a holder? nullptr if I'm not.
	BitHolder *getHolder();
	// which player owns me
	Player *getOwner();
	void setOwner(Player *player) { _owner = player; };
	// helper functions
	bool friendly();
	bool unfriendly();
	// game defined game tags
	const int gameTag() { return _gameTag; };
	void setGameTag(int tag) { _gameTag = tag; };
	// move to a position
	void moveTo(const ImVec2 &point);
	void update();
	void setOpacity(float opacity){};
	bool getMoving() { return _moving; };

private:
	int _restingZ;
	float _restingTransform;
	bool _pickedUp;
	Player *_owner;
	int _gameTag;
	ImVec2 _destinationPosition;
	ImVec2 _destinationStep;
	bool _moving;
};
