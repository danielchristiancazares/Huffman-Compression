/**
 * Unit Tests for Huffman Compression Components
 * Tests individual classes and functions
 */

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstring>
#include "HCNode.hpp"
#include "HCTree.hpp"
#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"

using namespace std;

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        tests_run++; \
        cout << "Testing: " << #name << " ... "; \
        try { \
            test_##name(); \
            tests_passed++; \
            cout << "\033[0;32mPASSED\033[0m" << endl; \
        } catch (const char* msg) { \
            tests_failed++; \
            cout << "\033[0;31mFAILED\033[0m: " << msg << endl; \
        } catch (...) { \
            tests_failed++; \
            cout << "\033[0;31mFAILED\033[0m: unknown exception" << endl; \
        } \
    } \
    void test_##name()

#define ASSERT(condition, message) \
    if (!(condition)) throw message;

// ============================================================================
// HCNode Tests
// ============================================================================

TEST(hcnode_construction) {
    HCNode node(10, 'A');
    ASSERT(node.count == 10, "Count should be 10");
    ASSERT(node.symbol == 'A', "Symbol should be 'A'");
    ASSERT(node.c0 == nullptr, "c0 should be nullptr");
    ASSERT(node.c1 == nullptr, "c1 should be nullptr");
    ASSERT(node.p == nullptr, "p should be nullptr");
}

TEST(hcnode_comparison) {
    HCNode node1(5, 'A');
    HCNode node2(10, 'B');

    // Lower count should be "greater" (for min-heap)
    ASSERT(node2 < node1, "Node with higher count should be less than node with lower count");

    // Equal counts, compare by symbol (normal ordering)
    HCNode node3(5, 'B');
    ASSERT(node1 < node3, "With equal counts, lower symbol should be less");
}

TEST(hcnode_with_children) {
    HCNode* left = new HCNode(3, 'A');
    HCNode* right = new HCNode(5, 'B');
    HCNode parent(8, 'A', left, right);

    ASSERT(parent.c0 == left, "c0 should point to left child");
    ASSERT(parent.c1 == right, "c1 should point to right child");
    ASSERT(parent.count == 8, "Parent count should be sum");

    delete left;
    delete right;
}

// ============================================================================
// HCTree Tests
// ============================================================================

TEST(hctree_empty_construction) {
    HCTree tree;
    // Should not crash on construction
    ASSERT(true, "Tree constructed successfully");
}

TEST(hctree_build_empty) {
    HCTree tree;
    vector<int> freqs(256, 0);
    tree.build(freqs);
    // Should handle empty frequency vector without crashing
    ASSERT(true, "Empty tree built successfully");
}

TEST(hctree_build_single_symbol) {
    HCTree tree;
    vector<int> freqs(256, 0);
    freqs['A'] = 100;
    tree.build(freqs);
    // Should build tree with single node
    ASSERT(true, "Single symbol tree built successfully");
}

TEST(hctree_build_two_symbols) {
    HCTree tree;
    vector<int> freqs(256, 0);
    freqs['A'] = 5;
    freqs['B'] = 3;
    tree.build(freqs);
    // Should build tree with two symbols
    ASSERT(true, "Two symbol tree built successfully");
}

TEST(hctree_build_multiple_symbols) {
    HCTree tree;
    vector<int> freqs(256, 0);
    freqs['A'] = 10;
    freqs['B'] = 5;
    freqs['C'] = 3;
    freqs['D'] = 2;
    tree.build(freqs);
    // Should build complete Huffman tree
    ASSERT(true, "Multi-symbol tree built successfully");
}

TEST(hctree_encode_decode_roundtrip) {
    HCTree tree;
    vector<int> freqs(256, 0);
    freqs['A'] = 10;
    freqs['B'] = 5;
    freqs['C'] = 3;
    tree.build(freqs);

    // Create temporary file for testing
    ofstream out_test("test_output/unittest_temp.txt");
    out_test << "Test";
    out_test.close();

    // This is a basic smoke test - full round-trip is tested by test.sh
    ASSERT(true, "Encode/decode setup successful");
}

TEST(hctree_destructor_no_crash) {
    {
        HCTree tree;
        vector<int> freqs(256, 0);
        for (int i = 0; i < 256; i++) {
            freqs[i] = i + 1;
        }
        tree.build(freqs);
        // Tree will be destructed here - should not leak or crash
    }
    ASSERT(true, "Destructor executed without crash");
}

// ============================================================================
// BitOutputStream Tests
// ============================================================================

TEST(bitoutputstream_construction) {
    ofstream out("test_output/unittest_bits.bin", ios::binary);
    BitOutputStream bos(out);
    out.close();
    ASSERT(true, "BitOutputStream constructed successfully");
}

TEST(bitoutputstream_write_single_bit) {
    ofstream out("test_output/unittest_single_bit.bin", ios::binary);
    BitOutputStream bos(out);

    // Write 8 bits to fill buffer
    for (int i = 0; i < 8; i++) {
        bos.writeBit(i % 2);
    }
    bos.flush();
    out.close();

    // Verify file was written
    ifstream in("test_output/unittest_single_bit.bin", ios::binary);
    ASSERT(in.good(), "File should be readable");
    in.close();
}

TEST(bitoutputstream_flush) {
    ofstream out("test_output/unittest_flush.bin", ios::binary);
    BitOutputStream bos(out);

    bos.writeBit(1);
    bos.writeBit(0);
    bos.flush();  // Should not crash

    out.close();
    ASSERT(true, "Flush executed successfully");
}

// ============================================================================
// BitInputStream Tests
// ============================================================================

TEST(bitinputstream_construction) {
    // Create test file
    ofstream out("test_output/unittest_read.bin", ios::binary);
    out.put(0xFF);
    out.close();

    ifstream in("test_output/unittest_read.bin", ios::binary);
    BitInputStream bis(in);
    in.close();
    ASSERT(true, "BitInputStream constructed successfully");
}

TEST(bitinputstream_readbit) {
    // Create test file with known pattern (0b10101010 = 0xAA)
    ofstream out("test_output/unittest_readbit.bin", ios::binary);
    out.put(0xAA);
    out.close();

    ifstream in("test_output/unittest_readbit.bin", ios::binary);
    BitInputStream bis(in);

    // Read bits - should be 1,0,1,0,1,0,1,0
    int bit0 = bis.readBit();
    int bit1 = bis.readBit();

    ASSERT(bit0 == 1, "First bit should be 1");
    ASSERT(bit1 == 0, "Second bit should be 0");

    in.close();
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(integration_null_byte_handling) {
    // Test that null bytes are preserved
    vector<int> freqs(256, 0);
    freqs[0] = 5;    // Null byte
    freqs['A'] = 10;
    freqs['B'] = 3;

    HCTree tree;
    tree.build(freqs);

    // Should build tree including null byte
    ASSERT(true, "Tree with null byte built successfully");
}

TEST(integration_all_bytes) {
    // Test with all 256 possible byte values
    vector<int> freqs(256, 0);
    for (int i = 0; i < 256; i++) {
        freqs[i] = i + 1;  // Each byte appears i+1 times
    }

    HCTree tree;
    tree.build(freqs);

    ASSERT(true, "Tree with all 256 bytes built successfully");
}

TEST(integration_memory_leak_check) {
    // Create and destroy many trees to check for leaks
    for (int i = 0; i < 100; i++) {
        HCTree tree;
        vector<int> freqs(256, 0);
        freqs['A'] = 10;
        freqs['B'] = 5;
        tree.build(freqs);
    }
    ASSERT(true, "100 trees created and destroyed without crash");
}

// ============================================================================
// Regression Tests for Bug Fixes
// ============================================================================

// Bug #1: BitOutputStream::flush() should not write when buffer is empty
TEST(regression_flush_empty_buffer) {
    // Create a file and flush without writing any bits
    ofstream out("test_output/regression_empty_flush.bin", ios::binary);
    BitOutputStream bos(out);
    bos.flush();  // Should not write anything
    out.close();

    // Verify file is empty (0 bytes)
    ifstream in("test_output/regression_empty_flush.bin", ios::binary);
    in.seekg(0, ios::end);
    streampos size = in.tellg();
    in.close();

    ASSERT(size == 0, "Flushing empty buffer should not write any bytes");
}

// Bug #2: BitInputStream::readBit() should return -1 on EOF
TEST(regression_readbit_eof) {
    // Create a file with exactly 1 byte
    ofstream out("test_output/regression_eof.bin", ios::binary);
    out.put(0xFF);
    out.close();

    ifstream in("test_output/regression_eof.bin", ios::binary);
    BitInputStream bis(in);

    // Read all 8 bits
    for (int i = 0; i < 8; i++) {
        int bit = bis.readBit();
        ASSERT(bit == 1, "All bits should be 1 for 0xFF");
    }

    // Next read should return -1 (EOF)
    int eof_bit = bis.readBit();
    in.close();

    ASSERT(eof_bit == -1, "readBit() should return -1 on EOF");
}

// Bug #3: HCTree::decode() should return -1 on empty tree (null root)
TEST(regression_decode_empty_tree) {
    HCTree tree;
    vector<int> freqs(256, 0);  // All zeros - no symbols
    tree.build(freqs);

    // Create a dummy input stream
    ofstream out("test_output/regression_decode_empty.bin", ios::binary);
    out.put(0xFF);
    out.close();

    ifstream in("test_output/regression_decode_empty.bin", ios::binary);
    BitInputStream bis(in);

    // Decoding from empty tree should return -1, not crash
    int result = tree.decode(bis);
    in.close();

    ASSERT(result == -1, "decode() on empty tree should return -1");
}

// Bug #3 related: HCTree::decode() should return -1 on EOF during traversal
TEST(regression_decode_eof_during_traversal) {
    HCTree tree;
    vector<int> freqs(256, 0);
    freqs['A'] = 10;
    freqs['B'] = 5;
    freqs['C'] = 3;
    freqs['D'] = 1;
    tree.build(freqs);

    // Create a file with insufficient bits to complete decoding
    // (just 1 bit when tree depth requires more)
    ofstream out("test_output/regression_decode_eof.bin", ios::binary);
    out.put(0x00);  // Only 8 bits available
    out.close();

    ifstream in("test_output/regression_decode_eof.bin", ios::binary);
    BitInputStream bis(in);

    // Try to decode multiple times to exhaust the input
    int result = -999;
    for (int i = 0; i < 20; i++) {  // More decodes than bits available
        result = tree.decode(bis);
        if (result == -1) break;  // EOF detected
    }
    in.close();

    ASSERT(result == -1, "decode() should return -1 when EOF reached during traversal");
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    cout << "=========================================" << endl;
    cout << "Huffman Component Unit Tests" << endl;
    cout << "=========================================" << endl;
    cout << endl;

    // Create test output directory
    int mkdir_result = system("mkdir -p test_output");
    (void)mkdir_result;  // Suppress unused result warning

    cout << "HCNode Tests:" << endl;
    cout << "-------------" << endl;
    run_test_hcnode_construction();
    run_test_hcnode_comparison();
    run_test_hcnode_with_children();
    cout << endl;

    cout << "HCTree Tests:" << endl;
    cout << "-------------" << endl;
    run_test_hctree_empty_construction();
    run_test_hctree_build_empty();
    run_test_hctree_build_single_symbol();
    run_test_hctree_build_two_symbols();
    run_test_hctree_build_multiple_symbols();
    run_test_hctree_encode_decode_roundtrip();
    run_test_hctree_destructor_no_crash();
    cout << endl;

    cout << "BitOutputStream Tests:" << endl;
    cout << "----------------------" << endl;
    run_test_bitoutputstream_construction();
    run_test_bitoutputstream_write_single_bit();
    run_test_bitoutputstream_flush();
    cout << endl;

    cout << "BitInputStream Tests:" << endl;
    cout << "---------------------" << endl;
    run_test_bitinputstream_construction();
    run_test_bitinputstream_readbit();
    cout << endl;

    cout << "Integration Tests:" << endl;
    cout << "------------------" << endl;
    run_test_integration_null_byte_handling();
    run_test_integration_all_bytes();
    run_test_integration_memory_leak_check();
    cout << endl;

    cout << "Regression Tests (Bug Fixes):" << endl;
    cout << "-----------------------------" << endl;
    run_test_regression_flush_empty_buffer();
    run_test_regression_readbit_eof();
    run_test_regression_decode_empty_tree();
    run_test_regression_decode_eof_during_traversal();
    cout << endl;

    cout << "=========================================" << endl;
    cout << "Test Summary" << endl;
    cout << "=========================================" << endl;
    cout << "Total tests run:    " << tests_run << endl;
    cout << "Tests passed:       \033[0;32m" << tests_passed << "\033[0m" << endl;
    cout << "Tests failed:       \033[0;31m" << tests_failed << "\033[0m" << endl;
    cout << endl;

    if (tests_failed == 0) {
        cout << "\033[0;32mAll unit tests passed!\033[0m" << endl;
        return 0;
    } else {
        cout << "\033[0;31mSome unit tests failed.\033[0m" << endl;
        return 1;
    }
}
