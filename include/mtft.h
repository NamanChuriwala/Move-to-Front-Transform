/*
 * DO NOT MODIFY THE CONTENTS OF THIS FILE.
 * IT WILL BE REPLACED DURING GRADING
 */
#ifndef MTFT_H
#define MTFT_H

#define USAGE(program_name, retcode) do { \
fprintf(stderr, "USAGE: %s %s\n", program_name, \
"[-h] [-e|-d] [-b BYTES]\n" \
"   -h       Help: displays this help menu.\n" \
"   -e       Read uncompressed data from standard input, write compressed data\n" \
"            to standard output\n" \
"   -d       Read compressed data from standard input, write uncompressed data\n" \
"            to standard output\n" \
"   -b       Interpret BYTES (1 or 2) bytes of uncompressed data as a single\n" \
"            input 'symbol' (default is 1).\n" \
); \
exit(retcode); \
} while(0)

/*
 * Options info, set by validargs.
 *   If -h is specified, then the HELP_OPTION bit is set.
 *   If -e is specified, then the ENCODE_OPTION bit is set.
 *   If -d is specified, then the DECODE_OPTION bit is set.
 *   At most one of ENCODE_OPTION or DECODE_OPTION can be set.
 *   The least-significant byte contains the number of bytes per symbol.
 */
int global_options;

#define HELP_OPTION (0x80000000)
#define ENCODE_OPTION (0x40000000)
#define DECODE_OPTION (0x20000000)

/*
 * Type used to represent symbols in an unencoded data stream.
 * A symbol is an integer with a fixed maximum number of bits.
 * The number of bits is not a compiled-in constant, but rather
 * is given as a parameter to the program.  This program supports
 * 8-bit-wide symbols (i.e. bytes) and 16-bit-wide symbols.
 */
typedef int SYMBOL;

/* Value of type SYMBOL that does not represent a valid symbol. */
#define NO_SYMBOL ((SYMBOL)(-1))

/*
 * Type used to represent codes in an encoded data stream.
 * Symbols are mapped to codes dynamically; i.e. the particular code
 * assigned to each symbol can change during execution.
 * Since the mapping from symbols to codes has to be invertible to
 * permit decoding, there have to be at least as many code values
 * as there are symbol values.  This program will use code values
 * that are up to twice as large as the maximum symbol value.
 */
typedef int CODE;

/*
 * Type used to represent an offset (numbered 0, 1, 2, ...) in a stream of symbols.
 */
typedef long OFFSET;

/* Value of type OFFSET that does not represent a valid offset. */
#define NO_OFFSET ((OFFSET)(-1))

/*
 * Maximum size of the "alphabet" of symbols.
 * Valid symbol values are in the range [0, SYMBOL_MAX-1].
 */
#define SYMBOL_MAX (1 << 16)

/*
 * Array that you can use in a preliminary implementation (see the assignment
 * handout) to map each possible rank to the symbol currently having that rank
 * (or to NO_SYMBOL).
 *
 * You *must* use this array to store this mapping, because you are not
 * permitted to declare your own arrays.
 */
SYMBOL symbol_rank[SYMBOL_MAX];

/*
 * Definition of a node in the "move-to-front" (MTF) map.
 * The overall structure of the map is a binary tree of nodes, with symbols
 * stored at the leaves and all leaves at the same depth (i.e. distance
 * from the root).
 *
 * At any given time, the depth of a leaf node in the map is given by the
 * minimum number of bits required to express the current offset.
 * This bound is increased by one each time the current offset reaches the
 * next power of two.  See the assignment handout for further explanation.
 */
typedef struct mtf_node {
    struct mtf_node *left_child;   // Pointer to left child node
    struct mtf_node *right_child;  // Pointer to right child node
    struct mtf_node *parent;       // Pointer to parent node
    int left_count;                // Number of symbols stored below left child
    int right_count;               // Number of symbols stored below right child
    SYMBOL symbol;                 // Symbol stored at this node (only at a leaf)
} MTF_NODE;

/*
 * Root node of the "MTF map", which is used to map between codes and symbols.
 *
 * You *must* make sure that this variable points to your map,
 * because some of our tests will check its contents for correctness.
 */
MTF_NODE *mtf_map;

/*
 * The current offset in the sequence of symbols being encoded or decoded.
 * This variable starts from zero and is incremented each time a symbol is
 * read (during encoding) or each time a symbol is written (during decoding).
 *
 * You *must* make sure that this variable is correctly updated, because
 * some of our tests will look at it.
 */
OFFSET current_offset;

/*
 * Array used to map each symbol to the last offset at which it occurred
 * in the unencoded data stream, or to NO_OFFSET if it has not yet occurred.
 *
 * You *must* use this array to store this mapping, because you are not
 * permitted to declare your own arrays.
 */
OFFSET last_offset[SYMBOL_MAX];

/*
 * Pool of nodes to use in constructing the MTF map.
 * At any given time, the map will have at most SYMBOL_MAX leaf nodes, at which
 * symbols are stored.  The path from the root to a leaf for a particular symbol
 * is given by the sequence of bits in the binary representation of the last offset
 * at which that symbol occurred in the stream of symbols being encoded or decoded.
 * If it currently takes B bits to represent an offset, then the total number of
 * nodes in the map can be no more than B * SYMBOL_MAX + 1, because there is just
 * one root node and each path to a leaf has SYMBOL_MAX nodes.  However, this
 * upper bound will only be reached for rather pathological data streams.
 * Usually the paths to the leaves will overlap quite substantially, so that the
 * total number of nodes required will be only a few times SYMBOL_MAX.
 * We have chosen to provide a node pool of size 3 * SYMBOL_MAX, which should
 * be more than sufficient for the data streams that we will want to process.
 *
 * You *must* use the entries of this array for the nodes in your map, because
 * you are not permitted to use malloc() or to declare any arrays yourself.
 */
#define MAX_NODES (3 * SYMBOL_MAX)
MTF_NODE node_pool[MAX_NODES];

/*
 * Index of the first unused node in the node pool.
 * To allocate a node from the pool, obtain a pointer to the node at this index
 * and then increment the index by one.
 */
int first_unused_node_index;

/*
 * Head of a list of "recycled" nodes that are currently available for use.
 * This variable is either NULL, which represents an empty list, or it points to
 * the first of a sequence of MTF_NODE structures arranged as a singly linked list
 * using the left_child field as the "next" pointer.
 *
 * Whenever a node is removed from your MTF map, it should be linked at the head
 * of this list for possible later re-use.
 *
 * Whenever a node is needed for the tree, if there are any nodes in this list,
 * then the first one should be removed and used.  Otherwise, if this list is empty,
 * then the first unused node in the node pool should be used, and the
 * first_unused_node_index variable incremented.
 */
MTF_NODE *recycled_node_list;  // NULL if the list is empty.

/*
 * Below this point are prototypes for functions that MUST occur in your program.
 * Non-functioning stubs for all these functions have been provided in the various source
 * files, where detailed specifications for the required behavior of these functions have
 * been given.
 *
 * Your implementations of these functions MUST have exactly the specified interfaces and
 * behave exactly according to the specifications, because we may choose to test any or all
 * of them independently of your main program.
 */

int validargs(int argc, char **argv);
int mtf_encode(void);
int mtf_decode(void);
CODE mtf_map_encode(SYMBOL s);
SYMBOL mtf_map_decode(CODE c);

#endif
