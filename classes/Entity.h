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

    Entity() : _entityType(EntityNone), _parent(nullptr), _retainCount(0) {};
    Entity(EntityType type) : _entityType(type) {};

    EntityType getEntityType() {return _entityType; }
    
    // set the parent for the Entity
    void setParent(Entity *parent) { _parent = parent; }
    // get the parent
    Entity *getParent() { return _parent; }

    // final cleanup of the entity
    void removeFromParentAndCleanup(bool cleanup) {
        _parent = nullptr; 
        if (cleanup) {
            delete this; 
        }
    }
    // release the sprite from the list being drawn if count has reached zero
    void release() { _retainCount--; if (_retainCount <= 0) removeFromParentAndCleanup(true); }
    // release the sprite from the list being drawn
    void retain() { _retainCount++;}

protected:
    EntityType _entityType;
    Entity *_parent;
    // set the retain count
    int _retainCount;
};
