#include "Huffman.hpp"

#define LEFT_CHILD 1
#define RIGHT_CHILD 2
#define NOT_PARENT -1
#define NULL_PARENT 0

Huffman::Huffman()
{
    this->tree = new HuffmanNode(NYT_KEY, 0);
    this->nyt = this->tree;
}

Huffman::~Huffman()
{
    delete_tree(this->tree);
    this->tree = nullptr;
}

/**
 * Insert a key into the Huffman tree. If key already exists in the tree,
 * then only its frequency is incremented. The tree is adjusted as needed.
 * @param key the key to be inserted.
 */
void Huffman::insert(uint8_t key) {
    // Check if `key` is already in tree.
    HuffmanNode *found = find_node(this->tree, key);

    if (found != nullptr) {
        // The requested key is already in the tree.
        // TODO find the node with `key` and increment frequency + check if rebalance is needed
        return;
    }

    // Pointer to NYT should never change.
    HuffmanNode *new_node = split_nyt(this->nyt, key);

    // TODO tree rebalancing?
}

/**
 * Split the NYT leaf into a new non-leaf node and two children.
 * The left child is the NYT leaf and the right child is a new leaf
 * containing `key` as key. Returns a pointer to the newly created node.
 * @param nyt pointer to the original NYT leaf to be split.
 * @param key is the key of the new leaf.
 * @returns Pointer to the new node.
 */
HuffmanNode *Huffman::split_nyt(HuffmanNode *nyt, uint8_t key)
{
    HuffmanNode *right = new HuffmanNode(key, 1);
    HuffmanNode *new_node = new HuffmanNode(NOT_LEAF, 0, nyt->parent, nyt, right);

    int8_t child = which_child(nyt->parent, nyt);
    if (child == LEFT_CHILD) {
        nyt->parent->left = new_node;
    } else if (child == RIGHT_CHILD) {
        nyt->parent->right = new_node;
    } else if (child == NOT_PARENT) {
        // TODO though probably in this case nothing should happen...
    } else {
        // NULL_PARENT
        this->tree = new_node;
    }
    nyt->parent = new_node;
    right->parent = new_node;

    new_node->freq = nyt->freq + right->freq;

    return new_node;
}

/**
 * Search the Huffman tree for the nyt node and return a reference to it.
 * If the node is not found, return nullptr.
 * @param current reference to node, from which to start the search.
 * @returns A reference to the nyt node or nullptr if not found.
 */
HuffmanNode *Huffman::find_node(HuffmanNode *current, uint16_t key)
{
    if (current == nullptr) {
        return nullptr;
    }

    if (current->key == key) {
        return current;
    }

    HuffmanNode *found = find_node(current->left, key);
    if (found != nullptr) {
        return found;
    }

    found = find_node(current->right, key);
    if (found != nullptr) {
        return found;
    }

    return nullptr;
}

/**
 * @returns LEFT_CHILD if the child is the left child, RIGHT_CHILD if it is
 * the right child, NULL_PARENT if the parent is a nullptr or NOT_PARENT if
 * the child is not this parent's.
 */
int8_t Huffman::which_child(HuffmanNode *parent, HuffmanNode *child)
{
    if (parent == nullptr) {
        return NULL_PARENT;
    }

    if (parent->left == child) {
        return LEFT_CHILD;
    } else if (parent->right == child) {
        return RIGHT_CHILD;
    }

    // The child is not theis parent's.
    return NOT_PARENT;
}

void Huffman::delete_tree(HuffmanNode *node)
{
    if (node == nullptr) {
        return;
    }

    delete_tree(node->left);
    delete_tree(node->right);

    delete node;
}

// TODO THESE ARE ONLY FOR DEBUGGING PURPOSES
void Huffman::getVerticalOrder(HuffmanNode* root, int hd, std::map<int, std::vector<HuffmanNode *>> &m)
{
    // Base case
    if (root == nullptr)
        return;

    // Store current node in map 'm'
    m[hd].push_back(root);

    // Store nodes in left subtree
    getVerticalOrder(root->left, hd-1, m);

    // Store nodes in right subtree
    getVerticalOrder(root->right, hd+1, m);
}

/**
 * Prints a binary tree as a vertical cross section per line from left to right.
 * https://www.geeksforgeeks.org/print-binary-tree-vertical-order-set-2/
 */
void Huffman::printVerticalOrder()
{
    // Create a map and store vertical order in map using
    // function getVerticalOrder()
    HuffmanNode* root = this->tree;
    std::map < int, std::vector<HuffmanNode *> > m;
    int hd = 0;
    getVerticalOrder(root, hd,m);

    // Traverse the map and print nodes at every horigontal
    // distance (hd)
    std::map< int,std::vector<HuffmanNode *> > :: iterator it;
    for (it=m.begin(); it!=m.end(); it++)
    {
        for (size_t i=0; i<it->second.size(); ++i)
            std::cout << "(" << it->second[i]->key << "," << it->second[i]->freq << ") ";
        std::cout << '\n';
    }
}