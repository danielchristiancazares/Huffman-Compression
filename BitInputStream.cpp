#include "BitInputStream.hpp"

/* 
   void BitInputStream::fill()
   Description: Retrieves 8 bits from the input stream.
   Parameters: None
   Return value: None
*/
void BitInputStream::fill() {
    buf = this->readByte();
    bufi = 0;
}

/*
   int BitInputStream::readBit()
   Description: Reads 1 bit from the 8 bit buffer.
   Parameters: None
   Return value: int bit - 0 or 1 bit retrieved from buffer, -1 on EOF.
*/
int BitInputStream::readBit() {
    /* If the buffer is empty, retrieve next byte. */
    if( bufi > 7 ) {
        int byte = this->readByte();
        /* Check for EOF */
        if( byte == -1 ) {
            return -1;
        }
        buf = byte;
        bufi = 0;
    }
    int bit = buf[ 7 - bufi ];
    ++bufi;
    return bit;
}

/*
   int BitInputStream::readBit()
   Description: Reads into the 8 bit buffer.
   Parameters: None
   Return value: int bit - 0 or 1 bit retrieved from buffer.
*/
int BitInputStream::readByte() {
    return in.get();
}
