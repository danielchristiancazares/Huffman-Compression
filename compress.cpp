#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <algorithm>

#include "HCNode.hpp"
#include "HCTree.hpp"

/*  We take in 2 arguments in addition to the program name
    (e.g. ./compress arg1 arg2)
    argv[0] is ./compress

    argv[1] is arg1: This is the input file name.

    argv[2] is arg2: This is the output file name.
*/

int main( int argc, char *argv[] ) {
    /* Ensure there are the proper number of arguments. */
    if( argc != 3 ) {
        cerr << "Invalid number of arguments.\n" << flush;
        return EXIT_FAILURE;
    }

    ifstream fin;
    ofstream fout;

    /* Attempt to open the input file with the given name. */
    fin.open( argv[1], ios::binary );

    if( !fin.is_open() ) {
        return EXIT_FAILURE;
    }

    cout << "Reading from file \"" << argv[1] << "\"... ";

    fin.clear();
    
    vector<int> freqs( 256, 0 );
    byte cnext = 0;
    size_t fsize = 0;
    unsigned int symbols = 0;

    BitInputStream bis( fin );
    /* Read in the frequencies. */
    while( ( cnext = fin.get() ) ) {
        if( fin.eof() ) {
            break;
        }
        freqs[ ( int ) cnext ] += 1;
        ++fsize;
        if( freqs[ ( int ) cnext ] == 1 ) {
            ++symbols;
        }
    }
    cout << "done.\n";

    cout << "Found " << symbols << " unique symbols in input "
         << "file of size " << fsize << " bytes.\n";

    cout << "Building Huffman code tree... ";     
    
    /* Do not continue if the file read in was empty. */
    HCTree hufftree;
    if( fsize != 0 ) {
        hufftree.build( freqs );
    }
  
    cout << "done.\n";
    
    /* Open the output file. */
    fout.open( argv[2], ios::binary );
    if( !fout.is_open() ) {
        return EXIT_FAILURE;
    }

    BitOutputStream bos( fout );
    
    /* Write out the frequencies. */
    for( int i = 0; i < 256; ++i) {
        fout.write( ( char * ) &freqs[i] , sizeof( int ) );
    }
    
    /* Clear the stream buffer */
    fin.clear();
    
    /* Return to the beginning of the stream buffer */
    fin.seekg( 0, ios::beg );

    /* Read in each character. */
    for( size_t i = 0; i < fsize; ++i ) {
        cnext = fin.get();
        hufftree.encode( cnext, bos );
    }

    bos.flush();
    fout.close();
    fin.close();

    return EXIT_SUCCESS;
}


