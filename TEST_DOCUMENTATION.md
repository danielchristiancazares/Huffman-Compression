# Huffman Compression Test Suite Documentation

## Overview

This document describes the comprehensive test suite for the Huffman compression implementation, including all edge cases and bug fixes that are verified by the tests.

## Running Tests

```bash
# Run full test suite
make test

# Run test suite with verbose output
make test-verbose

# Clean test artifacts
make clean
```

## Test Coverage

### Edge Case Tests

#### 1. Empty File Test
- **Test Name**: `empty_file`
- **Input**: `input_files/empty` (0 bytes)
- **Purpose**: Verifies that empty files are handled correctly without crashing
- **Expected**: Successful compression and decompression with identical output
- **Bug Coverage**: Ensures tree building handles empty frequency vectors

#### 2. Single Unique Symbol Test
- **Test Name**: `single_unique_symbol`
- **Input**: `input_files/justA` (8000 bytes, all 'A')
- **Purpose**: Tests the critical edge case of files with only one unique symbol
- **Expected**: Proper encoding/decoding when Huffman tree is just a single leaf node
- **Bug Coverage**: **Bug #7** - Single symbol file edge case where root has no parent

#### 3. Single Byte Test
- **Test Name**: `single_byte`
- **Input**: Single character 'X'
- **Purpose**: Minimal test case for single symbol encoding
- **Expected**: Correct round-trip compression/decompression
- **Bug Coverage**: **Bug #7** - Verifies encode() handles node->p == nullptr

#### 4. Two Symbols Test
- **Test Name**: `two_symbols`
- **Input**: "ABABABABAB"
- **Purpose**: Simplest multi-symbol case (optimal Huffman tree has height 1)
- **Expected**: Perfect compression with 1 bit per symbol
- **Bug Coverage**: Basic tree building and traversal

### Binary File Tests (Null Byte Handling)

These tests specifically verify **Bug #1** fix: binary file corruption when null bytes (0x00) are present.

#### 5. Binary with Nulls Test
- **Test Name**: `binary_with_nulls`
- **Input**: `input_files/binary.dat` (53,435 bytes with null bytes)
- **Purpose**: Real-world binary file with embedded null bytes
- **Expected**: All bytes including nulls are preserved
- **Bug Coverage**: **Bug #1** - Primary test for null byte handling in compress.cpp

#### 6. Null Bytes Only Test
- **Test Name**: `null_bytes_only`
- **Input**: File containing only null bytes
- **Purpose**: Extreme case of all nulls
- **Expected**: Correct handling of file with only 0x00 bytes
- **Bug Coverage**: **Bug #1** - Ensures loop doesn't exit on null byte

#### 7. Null at Start Test
- **Test Name**: `null_at_start`
- **Input**: "\x00ABC"
- **Purpose**: Verifies null byte at beginning doesn't cause early loop exit
- **Expected**: All 4 bytes preserved
- **Bug Coverage**: **Bug #1** - Null byte position test

#### 8. Null at End Test
- **Test Name**: `null_at_end`
- **Input**: "ABC\x00"
- **Purpose**: Verifies null byte at end is included
- **Expected**: All 4 bytes preserved
- **Bug Coverage**: **Bug #1** - Null byte position test

#### 9. Many Nulls Test
- **Test Name**: `many_nulls`
- **Input**: Five consecutive null bytes
- **Purpose**: Multiple consecutive nulls
- **Expected**: All 5 null bytes preserved
- **Bug Coverage**: **Bug #1** - Multiple null bytes test

#### 10. All 256 Bytes Test
- **Test Name**: `all_256_bytes`
- **Input**: File containing bytes 0x00 through 0xFF
- **Purpose**: Comprehensive test of all possible byte values
- **Expected**: Perfect preservation of all 256 unique byte values
- **Bug Coverage**: **Bug #1**, **Bug #3**, **Bug #4** - Full alphabet coverage

### Small File Tests

#### 11. Hello World Test
- **Test Name**: `hello_world`
- **Input**: "Hello, World!\n"
- **Purpose**: Simple text compression test
- **Expected**: Correct compression and decompression
- **Bug Coverage**: Basic functionality test

#### 12. Three Symbols Test
- **Test Name**: `three_symbols`
- **Input**: `input_files/justABC` (3,500 bytes)
- **Purpose**: Small alphabet compression
- **Expected**: Efficient compression with 3-symbol tree
- **Bug Coverage**: Small tree traversal

### Large File Tests

#### 13. Lots of A Test
- **Test Name**: `lots_of_A`
- **Input**: `input_files/lotsofA` (70,027 bytes)
- **Purpose**: Skewed distribution (mostly 'A' with some other characters)
- **Expected**: High compression ratio due to skewed distribution
- **Bug Coverage**: Large file with unbalanced tree

#### 14. Picture of Dorian Gray Test
- **Test Name**: `dorian_gray`
- **Input**: `input_files/pictureofdoriangray.txt` (461,898 bytes)
- **Purpose**: Medium-large text file
- **Expected**: Correct compression of full novel
- **Bug Coverage**: Real-world text file, stress test

#### 15. The Prince Test
- **Test Name**: `the_prince`
- **Input**: `input_files/theprince.txt` (305,864 bytes)
- **Purpose**: Medium text file
- **Expected**: Correct compression of full book
- **Bug Coverage**: Real-world text file

#### 16. War and Peace Test
- **Test Name**: `war_and_peace`
- **Input**: `input_files/warandpeace.txt` (3,288,738 bytes)
- **Purpose**: Very large text file (3.3 MB)
- **Expected**: Correct compression of entire novel
- **Bug Coverage**: Stress test for large files, memory leak detection

## Bug Coverage Summary

### Critical Bugs Verified by Tests

1. **Bug #1: Binary File Corruption (compress.cpp:46)**
   - **Tests**: `binary_with_nulls`, `null_bytes_only`, `null_at_start`, `null_at_end`, `many_nulls`, `all_256_bytes`
   - **Fix**: Changed loop to properly handle null bytes (0x00)
   - **Impact**: Without fix, any binary file with null bytes would be corrupted

2. **Bug #3: Null Pointer Dereference in encode() (HCTree.cpp:61, 86)**
   - **Tests**: `all_256_bytes` (implicitly tests all byte values)
   - **Fix**: Added null checks before dereferencing `leaves[symbol]`
   - **Impact**: Would cause segfault when encoding symbol with zero frequency

3. **Bug #4: Child Pointer Comparison (HCTree.cpp:65, 68, 89)**
   - **Tests**: All tests (encode is called for every symbol)
   - **Fix**: Compare node pointers instead of symbols
   - **Impact**: More robust and correct pointer comparison

4. **Bug #5: Memory Leak (HCTree.cpp:10)**
   - **Tests**: Large file tests (`war_and_peace`, etc.) stress test memory
   - **Fix**: Implemented proper destructor with recursive tree deletion
   - **Impact**: Memory leak on program exit

5. **Bug #6: Decode Loop Logic (HCTree.cpp:106, 122)**
   - **Tests**: All tests (every decompression uses decode)
   - **Fix**: Changed OR to AND in while loop condition
   - **Impact**: More correct internal node detection

6. **Bug #7: Single Symbol File Edge Case**
   - **Tests**: `single_unique_symbol`, `single_byte`, `many_nulls`
   - **Fix**: Special handling when `node->p == nullptr`
   - **Impact**: Would crash on files with only one unique symbol

## Test Methodology

All tests follow the same methodology:

1. **Compress**: Input file → compressed output using `./compress`
2. **Decompress**: Compressed file → decompressed output using `./uncompress`
3. **Verify**: Compare original input with decompressed output using `diff`
4. **Result**: PASS if files are identical, FAIL otherwise

This round-trip testing ensures that:
- Compression doesn't lose data
- Decompression correctly reconstructs the original
- The Huffman encoding is lossless
- All edge cases are handled

## Exit Codes

- **0**: All tests passed
- **1**: One or more tests failed

## Test Output Directory

All test artifacts are created in `test_output/` directory:
- Compressed files: `test_output/*.huf`
- Decompressed files: `test_output/*.out`
- Test input files: `test_output/*.{txt,bin}`

This directory is automatically cleaned by `make clean` and is excluded from git via `.gitignore`.

## Future Test Improvements

Potential areas for additional testing:

1. **Fuzzing**: Random input generation to find edge cases
2. **Performance Testing**: Measure compression ratios and speeds
3. **Comparison Testing**: Compare with reference implementation
4. **Memory Testing**: Run with valgrind to detect memory issues
5. **Component Unit Tests**: Direct testing of HCTree, BitInputStream, BitOutputStream classes
