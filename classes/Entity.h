#pragma once

class Entity
{
public:
    // type of entity
    enum EntityType {
        EntityNone,
        EntityPlayer,
        EntitySprite,
        EntityBit,
        EntityBitHolder
    };

    Entity() : _entityType(EntityNone), _parent(nullptr){};
    Entity(EntityType type) : _entityType(type) {};

    EntityType getEntityType() {return _entityType; }
    
    // set the parent for the Entity
    void setParent(Entity *parent) { _parent = parent; }
    // get the parent
    Entity *getParent() { return _parent; }

    // final cleanup of the entity
    void removeFromParentAndCleanup() {
        _parent = nullptr; 
    }
    // release the sprite from the list being drawn if count has reached zero

protected:
    EntityType _entityType;
    Entity *_parent;
};
