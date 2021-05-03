#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

// TODO USED FOR DEBUGGING ONLY
#include <map>
#include <iostream>

#define NYT_KEY 300
#define NOT_LEAF 400

struct HuffmanNode {
    HuffmanNode(uint16_t k, uint32_t f) :
        key(k),
        freq(f),
        parent(nullptr),
        left(nullptr),
        right(nullptr)
    {}

    HuffmanNode(uint16_t k, uint32_t f, HuffmanNode *p,
                HuffmanNode *l, HuffmanNode *r) :
        key(k),
        freq(f),
        parent(p),
        left(l),
        right(r)
    {}

    uint16_t key; //!< The key (should be 8-bit) or NYT node (value of > 255).
    uint32_t freq; //!< The frequency of the key.
    struct HuffmanNode *parent;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
};

class Huffman
{
private:
    HuffmanNode *tree;
    std::vector<uint8_t> keys;

    void split_nyt(HuffmanNode *nyt, uint8_t key);
    HuffmanNode *find_nyt(HuffmanNode *root);
    int8_t which_child(HuffmanNode *parent, HuffmanNode *child);
    void delete_tree(HuffmanNode *node);

    // TODO DEBUGGING FUNCTIONS - DELETE
    void getVerticalOrder(HuffmanNode* root, int hd, std::map<int, std::vector<int>> &m);
public:
    Huffman();
    ~Huffman();
    void insert(uint8_t key);

    // TODO DEBUGGING FUNCTIONS - DELETE
    void printVerticalOrder();
};

#endif /* HUFFMAN_HPP */