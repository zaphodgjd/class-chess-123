#pragma once
#include "bit.h"

class ChessBit : public Bit {
    public:
    bool isAlly(ChessBit* other) const { return other->_owner == this->_owner; }

    bool friendly()   const override { return Bit::friendly(); }
    bool unfriendly() const override { return Bit::unfriendly(); }
};