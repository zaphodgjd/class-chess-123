#pragma once
#include "bit.h"

class ChessBit : public Bit {
    public:
    /*  I'm not including this in Bit.h b/c in some contexts an ally may not neccesairly have the same
        owner, and it would be up to bit's children to implement it case by case. Some games may not
        even have the concept of an ally, so it seems best to keep it in chess land */
    bool isAlly(ChessBit* other) const { return other->_owner == this->_owner; }

    bool friendly()   const override { return Bit::friendly(); }
    bool unfriendly() const override { return Bit::unfriendly(); }
};