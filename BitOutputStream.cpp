#include "BitOutputStream.hpp"

/*
    void BitOutputStream::writeBit( int bit );
    Description: Write the LSB to the ostream.
    Parameters: int bit - Bit to be written.
    Return Value: None
*/
void BitOutputStream::writeBit( int bit ) {
    /* If the buffer contains 8 bits, then we write
       to the ostream. */
    if( bufi > 7 ) {
        flush();
    }

    /* We bitmask the argument with 1 to get LSB. */
    bit &= 1;
    
    /* We place the extracted LSB in the buffer and 
       increase index. */
    buf[ 7 - bufi ] = bit;
    ++bufi;
}

/*
    void BitOutputStream::flush();
    Description: Write buffer to ostream.
    Parameters: None
    Return Value: None
*/
void BitOutputStream::flush() {
    /* Only write if there are bits in the buffer */
    if( bufi > 0 ) {
        out.write( (char *) &buf, 1 );
    }
    out.flush();
    buf = bufi = 0;
}


