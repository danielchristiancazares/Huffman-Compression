#include <fstream>
#include <queue>
#include <stack>
#include <iostream>

#include "HCTree.hpp"

/* Helper function to recursively delete tree nodes */
void HCTree::deleteTree(HCNode* node) {
    if (node == nullptr) {
        return;
    }
    deleteTree(node->c0);
    deleteTree(node->c1);
    delete node;
}

/* Destructor for HCTree */
HCTree::~HCTree() {
    deleteTree(root);
}

/* build() for HCTree */
void HCTree::build( const vector<int>& freqs) {
    std::priority_queue< HCNode*,
                         std::vector<HCNode*>,
                         HCNodePtrComp > queue;

    /* Create a new frequency and symbol from freqs. */
    for( size_t i = 0; i < freqs.size(); ++i ) {
        if( freqs[i] != 0 ) {
            leaves[i] = new HCNode( freqs[i], i );
            queue.push( leaves[i] );
        }
    }


    /* If freqs is empty, then root is null. */
    if( queue.empty() ) {
        this->root = NULL;
        return;
    }

    /* The priority queue contains symbols from the file. */
    while( queue.size() > 1 ) {
        HCNode* first = queue.top();
        queue.pop();

        HCNode* sec = queue.top();
        queue.pop();
        
        HCNode* parent = new HCNode( ( first->count + sec->count ),
                                     first->symbol,
                                     first,
                                     sec );

        first->p = parent;
        sec->p = parent;

        queue.push( parent );
    }
    
    this->root = queue.top();
}

void HCTree::encode( byte symbol, ofstream& out ) const {
    /* Check if symbol exists as a leaf. */
    if( this->leaves[symbol] == nullptr ) {
        return;
    }

    HCNode* node = this->leaves[symbol];

    /* Special case: single symbol (root is a leaf, no parent) */
    if( node->p == nullptr ) {
        out << '0';
        return;
    }

    vector<int> code;

    while( node->p ) {
        if( node == node->p->c0 ) {
            code.push_back( 0 );
        }
        else {
            code.push_back( 1 );
        }
        node = node->p;
    }

    while( !code.empty() ) {
        out << code.back();
        code.pop_back();
    }
}

void HCTree::encode( byte symbol, BitOutputStream& out) const {
    /* Check if symbol exists as a leaf. */
    if( this->leaves[symbol] == nullptr ) {
        return;
    }

    HCNode* node = this->leaves[symbol];

    /* Special case: single symbol (root is a leaf, no parent) */
    if( node->p == nullptr ) {
        out.writeBit( 0 );
        return;
    }

    vector<int> code;

    while( node->p ) {
        if( node == node->p->c0 ) {
            code.push_back( 0 );
        } else {
            code.push_back( 1 );
        }
        node = node->p;
    }

    for( vector<int>::reverse_iterator it = code.rbegin(); it != code.rend(); ++it ) {
        out.writeBit( *it );
    }
}

int HCTree::decode( BitInputStream& in ) const {
    HCNode *node = root;
    bitset<1> bit;

    while( node->c0 && node->c1 ) {
        bit = in.readBit();
        if( bit == 1 ) {
            node = node->c1;
        } else {
            node = node->c0;
        }
    }

    return node->symbol;
}

int HCTree::decode( ifstream& in ) const {
    HCNode *node = root;
    bitset<1> bit;

    while( node->c0 && node->c1 ) {
        bit = in.get();
        bit &= 1;
        if( bit == 1 ) {
            node = node->c1;
        } else {
            node = node->c0;
        }
    }

    return node->symbol;
}
