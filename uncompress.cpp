#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>

#include "HCTree.hpp"

int main( int argc, char *argv[] ) {
    if( argc != 3 ) {
        cerr << "Invalid number of arguments.\n";
        return EXIT_FAILURE;
    }

    ifstream fin;
    ofstream fout;

    fin.open( argv[1], ios::binary );

    /* Check to see if the input file opened correctly. */
    if( !fin.is_open() ) {
        return EXIT_FAILURE;
    }

    vector<int> freqs( 256, 0 );
    int fsize = 0;
    HCTree hufftree;

    fin.clear();


    /* Read in the header. */
    for( size_t i = 0; i < freqs.size(); ++i ) {
        fin.read( (char *) &freqs[i], sizeof( int ) );
        fsize += freqs[i];
    }

    /* Build the Huffman tree. */
    if( fsize != 0 ) {
        hufftree.build( freqs );
    }

    fout.open( argv[2], ios::binary );
    
    BitInputStream bis( fin );

    byte tmp = 0;  

    for( int i = 0; i < fsize; ++i ) {
        tmp = hufftree.decode( bis );
        fout.write( (char *) &tmp, 1);
    } 

    fin.close();
    fout.close();

    return EXIT_SUCCESS;
}
