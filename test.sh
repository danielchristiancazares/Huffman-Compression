#!/bin/bash

# Comprehensive Test Suite for Huffman Compression
# Tests all edge cases and verifies bug fixes

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Create test output directory
TEST_DIR="test_output"
mkdir -p "$TEST_DIR"

# Helper function to run a test
run_test() {
    local test_name="$1"
    local input_file="$2"
    local compressed_file="$TEST_DIR/${test_name}.huf"
    local decompressed_file="$TEST_DIR/${test_name}.out"

    TESTS_RUN=$((TESTS_RUN + 1))

    echo -n "Testing: $test_name ... "

    # Compress
    if ! ./compress "$input_file" "$compressed_file" > /dev/null 2>&1; then
        echo -e "${RED}FAILED${NC} (compression failed)"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        return 1
    fi

    # Decompress
    if ! ./uncompress "$compressed_file" "$decompressed_file" > /dev/null 2>&1; then
        echo -e "${RED}FAILED${NC} (decompression failed)"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        return 1
    fi

    # Verify round-trip: original should match decompressed
    if ! diff -q "$input_file" "$decompressed_file" > /dev/null 2>&1; then
        echo -e "${RED}FAILED${NC} (output differs from input)"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        return 1
    fi

    echo -e "${GREEN}PASSED${NC}"
    TESTS_PASSED=$((TESTS_PASSED + 1))
    return 0
}

# Helper to create test file
create_test_file() {
    local filename="$1"
    shift
    printf "%b" "$@" > "$filename"
}

echo "========================================="
echo "Huffman Compression Test Suite"
echo "========================================="
echo ""

# Ensure executables exist
if [ ! -f "./compress" ] || [ ! -f "./uncompress" ]; then
    echo -e "${RED}ERROR: compress and uncompress executables not found${NC}"
    echo "Run 'make' first to build the project"
    exit 1
fi

echo "Creating additional test files..."

# Test 1: Create file with null bytes (tests Bug #1 fix)
create_test_file "$TEST_DIR/null_bytes.bin" '\x00\x00\x00\x01\x02\x00\xff\x00'

# Test 2: Create single byte file
create_test_file "$TEST_DIR/single_byte.txt" 'X'

# Test 3: Create two-symbol file
create_test_file "$TEST_DIR/two_symbols.txt" 'ABABABABAB'

# Test 4: Create file with all 256 byte values
printf "" > "$TEST_DIR/all_bytes.bin"
for i in {0..255}; do
    printf "\\x$(printf %02x $i)" >> "$TEST_DIR/all_bytes.bin"
done

# Test 5: Create file with null byte at start
create_test_file "$TEST_DIR/null_at_start.bin" '\x00\x41\x42\x43'

# Test 6: Create file with null byte at end
create_test_file "$TEST_DIR/null_at_end.bin" '\x41\x42\x43\x00'

# Test 7: Create file with multiple null bytes
create_test_file "$TEST_DIR/many_nulls.bin" '\x00\x00\x00\x00\x00'

# Test 8: Create small text file
echo "Hello, World!" > "$TEST_DIR/hello.txt"

echo "Done."
echo ""

echo "Running Edge Case Tests:"
echo "-------------------------"

# Edge case tests
run_test "empty_file" "input_files/empty"
run_test "single_unique_symbol" "input_files/justA"
run_test "single_byte" "$TEST_DIR/single_byte.txt"
run_test "two_symbols" "$TEST_DIR/two_symbols.txt"

echo ""
echo "Running Binary File Tests (Bug #1 - Null Byte Handling):"
echo "---------------------------------------------------------"

# Binary file tests (testing null byte handling - Bug #1)
run_test "binary_with_nulls" "input_files/binary.dat"
run_test "null_bytes_only" "$TEST_DIR/null_bytes.bin"
run_test "null_at_start" "$TEST_DIR/null_at_start.bin"
run_test "null_at_end" "$TEST_DIR/null_at_end.bin"
run_test "many_nulls" "$TEST_DIR/many_nulls.bin"
run_test "all_256_bytes" "$TEST_DIR/all_bytes.bin"

echo ""
echo "Running Small File Tests:"
echo "-------------------------"

# Small file tests
run_test "hello_world" "$TEST_DIR/hello.txt"
run_test "three_symbols" "input_files/justABC"

echo ""
echo "Running Large File Tests:"
echo "-------------------------"

# Large file tests
run_test "lots_of_A" "input_files/lotsofA"
run_test "dorian_gray" "input_files/pictureofdoriangray.txt"
run_test "the_prince" "input_files/theprince.txt"
run_test "war_and_peace" "input_files/warandpeace.txt"

echo ""
echo "========================================="
echo "Test Summary"
echo "========================================="
echo -e "Total tests run:    $TESTS_RUN"
echo -e "Tests passed:       ${GREEN}$TESTS_PASSED${NC}"
echo -e "Tests failed:       ${RED}$TESTS_FAILED${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
