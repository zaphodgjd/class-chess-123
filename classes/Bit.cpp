
#include "Bit.h"
#include "BitHolder.h"

Bit::~Bit()
{
}

BitHolder *Bit::getHolder()
{
	// Look for my nearest ancestor that's a BitHolder:
	for (Entity *layer = getParent(); layer; layer = layer->getParent())
	{
		if (layer->getEntityType() == EntityBitHolder)
			return (BitHolder *)layer;
		else if (layer->getEntityType() == EntityBit)
			return nullptr;
	}
	return nullptr;
}

void Bit::setPickedUp(bool up)
{
	if (up != _pickedUp)
	{
		float opacity = 0.0f;
		float scale = 1.0f;
		float rotation = 0.0f;
		int z;

		if (up)
		{
			opacity = kPickedUpOpacity;
			z = bitz::kPickupUpZ;
			_restingZ = getLocalZOrder();
			_restingTransform = getRotation();
			scale = kPickedUpScale;
		}
		else
		{
			opacity = 1.0f;
			z = getLocalZOrder(); // _restingZ;
			if (z == bitz::kPickupUpZ)
			{
				z = _restingZ;
			}
			rotation = _restingTransform;
			scale = 1.0f;
		}
		setScale(scale); // todo: animate this
		setLocalZOrder(z);
		setOpacity(opacity);
		setRotation(rotation);
		_pickedUp = up;
	}
}

bool Bit::friendly()
{
	return true;
}

bool Bit::unfriendly()
{
	return !friendly();
}

bool Bit::getPickedUp()
{
	return _pickedUp;
}

Player *Bit::getOwner()
{
	return _owner;
}

void Bit::moveTo(const ImVec2 &point)
{
	_destinationPosition = point;
	// work out the step so we move same step each update
	ImVec2 delta = ImVec2(_destinationPosition.x - getPosition().x, _destinationPosition.y - getPosition().y);
	_destinationStep = ImVec2(delta.x * 0.05f, delta.y * 0.05f);
	_moving = true;
}

void Bit::update()
{
	if (!_moving)
	{
		return;
	}
	ImVec2 pos = getPosition();
	ImVec2 delta = ImVec2(_destinationPosition.x - pos.x, _destinationPosition.y - pos.y);
	if (fabs(delta.x) >= 0.1f || fabs(delta.y) > 0.1f)
	{
		ImVec2 newPosition = ImVec2(pos.x + _destinationStep.x, pos.y + _destinationStep.y);
		setPosition(newPosition);
	}
	else
	{
		setPosition(_destinationPosition);
		_moving = false;
	}
}