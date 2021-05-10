/**
 * Implementation of the Huffman class. Handles adding symbols
 * to an adaptive Huffman tree and retrieving Huffman codes
 * of newly added keys. The implemented Huffman tree is an FGK adaptive tree.
 * @author Patrik Nemeth (xnemet04)
 *
 * File created: 03.05.2021
 */
#include "Huffman.hpp"

#define LEFT_CHILD 1
#define RIGHT_CHILD 2
#define NOT_PARENT -1
#define NULL_PARENT 0

Huffman::Huffman()
{
    this->tree = new HuffmanNode(NYT_KEY, 0);
    this->nyt = this->tree;

    // By default no keys are in the tree, therefore no node
    // for the corresponding keys.
    for (int i = 0; i < SYMBOL_SET_SIZE; i ++) {
        this->nodes[i] = nullptr;
    }
}

Huffman::~Huffman()
{
    delete_tree(this->tree);
    this->tree = nullptr;
}

/**
 * Insert a key into the Huffman tree. If `key` already exists in the tree,
 * then only its frequency is incremented. The tree is adjusted as needed.
 * Before insertion, an Adaptive Huffman code for that `key` is generated
 * (that means NYT code + normal `key` value in binary if `key` is not in
 * the tree OR the `key`'s Huffman code if the key is in the tree) and
 * appended to the vector `bits`. Throws ERR_LARGE_KEY when a key
 * over 255 is given that is not the EOF_KEY.
 * @throws ERR_LARGE_KEY when a key over 255 is given that is not the EOF_KEY.
 * @param key the key to be inserted (values 0-255 for pixels or EOF_KEY
 * when EOF should be encoded).
 * @param bits pointer to a bool vector, to which the Adaptive Huffman
 * code for `key` will be appended.
 */
void Huffman::insert(const uint16_t key, std::vector<bool> *bits) {
    if (key > 255 && key != EOF_KEY) {
        // Only values 0-255 are valid + the EOF key.
        throw ERR_LARGE_KEY;
    }

    // Check if `key` is already in tree.
    HuffmanNode *found = find_node(key);
    get_code(found, key, bits);

    if (found != nullptr) {
        // The requested key is already in the tree.
        rebalance_tree(found);
        return;
    }

    // Pointer to NYT should never change.
    HuffmanNode *new_node = split_nyt(this->nyt, key);

    if (new_node != this->tree) {
        rebalance_tree(new_node->parent);
    }
}

/**
 * Decode encoded data represented by a bool vector "bitstream" `bits`.
 * The decoded values are appended to `data`. If an EOF code is reached,
 * then the decoder stops and does not decode the rest of the code stream
 * (all codes before EOF are decoded, but not after).
 * Exceptions ERR_NON_EMPTY_TREE or ERR_FIRST_BIT_NOT_0 may be thrown.
 * @param bits pointer to code bitstream represented by a bool vector.
 * @param data pointer to vector, to which to append decoded data.
 * @throws ERR_NON_EMPTY_TREE Thrown when a non-empty instance of the Huffman
 * tree class is attempted to be used for decoding.
 * @throws ERR_FIRST_BIT_NOT_0 If the first bit of the input bitstream `bits`
 * is not a 0.
 */
void Huffman::decode(std::vector<bool> *bits, std::vector<uint8_t> *data)
{
    // The decoder tree must be an empty tree.
    // TODO maybe write a public wrapper function for deleting a tree?
    if (this->tree->left != nullptr || this->tree->right != nullptr) {
        throw ERR_NON_EMPTY_TREE;
    }

    if ((*bits)[0] != (bool) 0) {
        throw ERR_FIRST_BIT_NOT_0;
    }

    // Start from pos = 1, because position 0 should always have
    // a "0" initial NYT code.
    size_t pos = 1; // Position in the `bits` vector.
    const size_t bits_size = bits->size();

    HuffmanNode *current;
    uint8_t pixel = 0;
    while (true) {
        current = this->tree; // Go to root.

        // Navigate to external node based on incoming code.
        while (current->left != nullptr) {//External nodes don't have children.
            // If true (1) go right, false (0) go left.
            (*bits)[pos] ?
                current = current->right : current = current->left;
            pos++;
        }

        if (current->key == NYT_KEY) {
            // NYT code received, read raw pixel value (8-bits).
            pixel = 0;
            uint8_t mask = 128;

            // If EOF, then the first bit after NYT code is set.
            // This is because after NYT 9 bit codes are sent - lower 8 for
            // pixel values and the MSB as an EOF flag.
            if ((*bits)[pos]) {
                // EOF
                return;
            }
            pos++;

            for (size_t i = 0; i < 8; i++, pos++) {
                // There is probably a better way...
                if ((*bits)[pos]) {
                    pixel += mask;
                }
                mask = mask >> 1;
            }
            data->push_back(pixel);

            // Make a new node with new pixel value as key.
            HuffmanNode *new_node = split_nyt(this->nyt, pixel);
            if (new_node != this->tree) {
                rebalance_tree(new_node->parent);
            }
        } else {
            // Valid pixel code received.
            data->push_back(current->key);
            rebalance_tree(current);
        }

        if (pos >= bits_size) {
            // All bits read, exit.
            break;
        }
    }
}

/**
 * Appends an Adaptive Huffman code of key `key` to `bits`. The `node` contains
 * the `key`, for which to calculate the Adaptive Huffman code. If `node`
 * is a nullptr, then it is assumed, that `key` is not in the Huffman tree
 * and therefore, the NYT code + the raw (non-huffman) 8-bit pixel value
 * for `key` is appended to `bits`.
 * @note NOTE: The vector<bool> may or may not be space efficient. Depends on vector
 * implementation.
 * @param node pointer to a node, which contains `key`.
 * @param key the value of the key.
 * @param bits pointer to bool vector that hold the coded bits.
 */
void Huffman::get_code(HuffmanNode *node, const uint16_t key, std::vector<bool> *bits)
{
    if (node == nullptr) {
        // First appearance of `key`

        if (this->tree->key == NYT_KEY) {
            // Only NYT is in the tree, start with a 0.
            bits->push_back(0);
        } else {
            // NYT has a path with a code - find NYT and get its code.
            code_for_node(this->nyt, bits);
        }

        // And then get the non-encoded bits for `key`.
        const uint32_t mask = 1;

        // 9 bits for an ucoded value (MSb for EOF flag
        // and the remaining 8 bits for pixel value).
        for (int i = 8; i >= 0; i--) {
            bool bit = key & (mask << i);
            bits->push_back(bit);
        }
        return;
    }

    code_for_node(node, bits);
}

/** Appends a Huffman code for `node` to vector `bits`.
 * @param node pointer to node, for which to append the Huffman code.
 * @param bits pointer to vector, to which to append the code.
 */
void Huffman::code_for_node(HuffmanNode *node, std::vector<bool> *bits)
{
    // Crawl upward from the node containing `key` and build
    // the Huffman code backwards.
    std::vector<bool> bits_reversed;
    while (node != nullptr) {
        const int8_t child_side = which_child(node->parent, node);
        switch (child_side)
        {
        case LEFT_CHILD:
            bits_reversed.push_back(0);
            break;
        case RIGHT_CHILD:
            bits_reversed.push_back(1);
            break;
        case NULL_PARENT:
            // TODO check if some behavior should be set for these...
            // But none should be required.
        case NOT_PARENT:
        default:
            break;
        }
        node = node->parent;
    }

    // Reverse to correct order and append to `bits`.
    std::reverse(bits_reversed.begin(), bits_reversed.end());
    bits->insert(bits->end(), bits_reversed.begin(), bits_reversed.end());
}

/**
 * Split the NYT leaf into a new non-leaf node and two children.
 * The left child is the NYT leaf and the right child is a new leaf
 * containing `key` as key. Returns a pointer to the newly created node.
 * @param nyt pointer to the original NYT leaf to be split.
 * @param key is the key of the new leaf.
 * @returns Pointer to the new node.
 */
HuffmanNode *Huffman::split_nyt(HuffmanNode *nyt, const uint16_t key)
{
    HuffmanNode *right = new HuffmanNode(key, 1, nyt->node_num - 1);
    HuffmanNode *new_node = new HuffmanNode(NOT_LEAF, 0, nyt->node_num, nyt->parent, nyt, right);

    const int8_t child = which_child(nyt->parent, nyt);
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

    nyt->node_num -= 2;

    // Save reference to new key node.
    this->nodes[key] = right;

    return new_node;
}

/**
 * Search the Huffman tree for the nyt node and return a reference to it.
 * If the node is not found, return nullptr.
 * @param key the key, for which its corresponding node should be returned.
 * @returns A reference to the nyt node or nullptr if not found.
 */
HuffmanNode *Huffman::find_node(const uint16_t key)
{
    return this->nodes[key];
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

/**
 * Rebalances (updates) the Huffman tree using the FGK adaptive algorithm.
 * @param current pointer to node from which to start the update.
 */
void Huffman::rebalance_tree(HuffmanNode *current)
{
    HuffmanNode *highest_number_node;
    highest_number_node = highest_number_node_in_block(current, current->freq);

    // Swap if needed.
    if (current != highest_number_node) {
        swap(current, highest_number_node);
    }

    // Increment frequency.
    current->freq += 1;

    // Update the parent if not root.
    if (this->tree != current) {
        rebalance_tree(current->parent);
    }
}

/**
 * Searches for nodes, that have frequency `freq` and adds them to
 * the vector `nodes`. Searches recursively downward from node `current`.
 * The `current` node should therefore be the root node upon first call.
 * @param current pointer to  node which is currently being checked (first call
 * to this function should have this param set as the tree root).
 * @param freq the frequency, based on which the nodes should be added
 * to `nodes`.
 * @param nodes is the vector, to which pointers to nodes with frequency `freq`
 * will be pushed.
 * @param node_num number (index) of the node, for which we are trying to find
 * a pair, with which to swap places.
 */
void Huffman::get_nodes_by_freq(
    HuffmanNode *current, const uint32_t freq,
    std::vector<HuffmanNode *> *nodes, const uint16_t node_num)
{
    // TODO this method significantly slows down the encoding/decoding
    if (current == nullptr) {
        return;
    }

    // If the node number is below the one we are attempting to find a pair for
    // in Huffman::highest_number_node_in_block(), then there is no need to
    // search further.
    if (current->node_num < node_num) {
        return;
    }

    // Suitable node, push it to vector.
    if (current->freq == freq) {
        nodes->push_back(current);
    }

    // Recurse to subtrees
    get_nodes_by_freq(current->left, freq, nodes, node_num);
    get_nodes_by_freq(current->right, freq, nodes, node_num);
}

/**
 * Return pointer to node that has the highest node number in a block. A block
 * is the set of nodes with identical frequencies. If the highest number is
 * `current`'s parent, return the second highest.
 * @param current pointer to current node, at which the tree rebalancing
 * algorithm is currently at.
 * @param block_freq all nodes with this frequency are in the same block.
 * @returns Node with highest node number within block defined by `block_freq`.
 * If the returned node would have been `current`'s parent, return second best.
 */
HuffmanNode *Huffman::highest_number_node_in_block(
    HuffmanNode *current, const uint32_t block_freq)
{
    std::vector<HuffmanNode *> nodes;
    get_nodes_by_freq(this->tree, block_freq, &nodes, current->node_num);

    std::sort(nodes.begin(), nodes.end(),
        [](const HuffmanNode * a, const HuffmanNode * b)
        {
            return a->node_num > b->node_num;
        });

    if (nodes.size() == 1) {
        // Only one in block, therefore this is the
        // highest numbered node in block.
        return nodes[0];
    }

    if (nodes[0] == current->parent) {
        // Must never return parent.
        return nodes[1];
    }

    return nodes[0];
}

/**
 * Swap nodes `a` and `b`. Their subtrees follow them.
 * @param a pointer to a node to be swapped with `b`.
 * @param b pointer to a node to be swapped with `a`.
 */
void Huffman::swap(HuffmanNode *a, HuffmanNode *b)
{
    int8_t a_child_side = which_child(a->parent, a);
    int8_t b_child_side = which_child(b->parent, b);

    switch (a_child_side)
    {
    case LEFT_CHILD:
        a->parent->left = b;
        break;
    case RIGHT_CHILD:
        a->parent->right = b;
        break;
    case NULL_PARENT:
        // node `a` is root
        // Should never happen... I think?
        std::cerr << "HUFFMAN SWAPPING 'b' WITH ROOT - UNSURE IF"
                    " CORRECTLY IMPLEMENTED.\n";
        swap_with_root(b);
        return;
    default:
        std::cerr << "a_child_side default case\n";
        throw "a_child_side default case\n";
        break;
    }

    switch (b_child_side)
    {
    case LEFT_CHILD:
        b->parent->left = a;
        break;
    case RIGHT_CHILD:
        b->parent->right = a;
        break;
    case NULL_PARENT:
        // node `b` is root
        // Same as with `a`... Should prolly never happen.
        std::cerr << "HUFFMAN SWAPPING 'a' WITH ROOT - UNSURE IF"
                    " CORRECTLY IMPLEMENTED.\n";
        swap_with_root(a);
        return;
    default:
        std::cerr << "b_child_side default case\n";
        throw "b_child_side default case\n";
        break;
    }

    HuffmanNode *foster_parent = a->parent;
    a->parent = b->parent;
    b->parent = foster_parent;

    const uint16_t dummy = a->node_num;
    a->node_num = b->node_num;
    b->node_num = dummy;
}

/**
 * Helper function to swap `node` with root (i.e. this->tree).
 * Probably incorrect implementation, but THIS SHOULD NEVER HAVE TO BE CALLED.
 */
void Huffman::swap_with_root(HuffmanNode *node)
{
    HuffmanNode *dummy, *root = this->tree;

    root->left->parent = node;
    root->right->parent = node;

    const int8_t child_side = which_child(node->parent, node);

    switch (child_side)
    {
    case LEFT_CHILD:
        node->parent->left = root;
        break;
    case RIGHT_CHILD:
        node->parent->right = root;
    default:
        std::cerr << "child_side default case in swap_with_root()\n";
        throw "child_side default case in swap_with_root()\n";
        break;
    }

    if (node->left != nullptr) node->left->parent = root;
    if (node->right != nullptr) node->right->parent = root;

    dummy = root->left;
    root->left = node->left;
    node->left = dummy;

    dummy = root->right;
    root->right = node->right;
    node->right = dummy;

    root->parent = node->parent;
    node->parent = nullptr;
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
            std::cout << "(" << it->second[i]->key << "," << it->second[i]->freq << "|" <<
                        it->second[i]->node_num << ") ";
        std::cout << '\n';
    }
}