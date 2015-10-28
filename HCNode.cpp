#include "HCNode.hpp"

bool HCNode::operator<( const HCNode& other ) const {
    if( this->count != other.count ) {
        return this->count > other.count;
    }
    return this->symbol < other.symbol;
}
