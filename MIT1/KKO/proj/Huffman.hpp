#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

// TODO USED FOR DEBUGGING ONLY
#include <map>
#include <iostream>

#define NYT_KEY 300
#define EOF_KEY 256
#define NOT_LEAF 400
#define SYMBOL_SET_SIZE 258 // The maximum number of symbols the tree will
                            // hold. 256 pixel values + 1 NYT node + EOF node.


// These ERR codes are used in Huffman::decode() as exceptions.
#define ERR_NON_EMPTY_TREE 1 // Tree not empty. Use only empty tree for decode.
#define ERR_FIRST_BIT_NOT_0 2 // First bit of code bitstream is not 0.
#define ERR_LARGE_KEY 3

struct HuffmanNode {
    HuffmanNode(uint16_t k, uint32_t f, uint16_t n) :
        key(k),
        freq(f),
        node_num(n)
    {}

    /* Set key and frequency. Node number defaults to 2 * SYMBOL_SET_SIZE - 1. */
    HuffmanNode(uint16_t k, uint32_t f) :
        key(k),
        freq(f),
        node_num(2 * SYMBOL_SET_SIZE - 1)
    {}

    HuffmanNode(uint16_t k, uint32_t f, uint16_t n, HuffmanNode *p,
                HuffmanNode *l, HuffmanNode *r) :
        key(k),
        freq(f),
        node_num(n),
        parent(p),
        left(l),
        right(r)
    {}

    uint16_t key = 0; //!< The key (should be 8-bit) or NYT node (value of > 255).
    uint32_t freq = 0; //!< The frequency of the key.
    uint16_t node_num = 0; //!< The node number. Numbered bottom up, left to right.
    struct HuffmanNode *parent = nullptr;
    struct HuffmanNode *left = nullptr;
    struct HuffmanNode *right = nullptr;
};

class Huffman
{
private:
    HuffmanNode *nodes[SYMBOL_SET_SIZE];
    HuffmanNode *tree, *nyt;
    std::vector<uint8_t> keys;

    void get_code(HuffmanNode *node, const uint16_t key, std::vector<bool> *bits);
    void code_for_node(HuffmanNode *node, std::vector<bool> *bits);
    HuffmanNode *split_nyt(HuffmanNode *nyt, const uint16_t key);
    HuffmanNode *find_node(const uint16_t key);
    void get_nodes_by_freq(HuffmanNode *current, const uint32_t freq, std::vector<HuffmanNode *> *nodes, const uint16_t node_num);
    int8_t which_child(HuffmanNode *parent, HuffmanNode *child);
    void rebalance_tree(HuffmanNode *current);
    HuffmanNode *highest_number_node_in_block(HuffmanNode *current, const uint32_t block_freq);
    void swap(HuffmanNode *a, HuffmanNode *b);
    void swap_with_root(HuffmanNode *node);
    void delete_tree(HuffmanNode *node);

    // TODO DEBUGGING FUNCTIONS - DELETE
    void getVerticalOrder(HuffmanNode* root, int hd, std::map<int, std::vector<HuffmanNode *>> &m);
public:
    Huffman();
    ~Huffman();
    void insert(uint16_t key, std::vector<bool> *bits);
    void decode(std::vector<bool> *bits, std::vector<uint8_t> *data);

    // TODO DEBUGGING FUNCTIONS - DELETE
    void printVerticalOrder();
};

#endif /* HUFFMAN_HPP */