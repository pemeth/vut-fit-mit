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
 * Split the NYT leaf into a new non-leaf node and two children.
 * The left child is the NYT leaf and the right child is a new leaf
 * containing `key` as key. Returns a pointer to the newly created node.
 * @param nyt pointer to the original NYT leaf to be split.
 * @param key is the key of the new leaf.
 * @returns Pointer to the new node.
 */
HuffmanNode *Huffman::split_nyt(HuffmanNode *nyt, uint8_t key)
{
    HuffmanNode *right = new HuffmanNode(key, 1, nyt->node_num - 1);
    HuffmanNode *new_node = new HuffmanNode(NOT_LEAF, 0, nyt->node_num, nyt->parent, nyt, right);

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

    nyt->node_num -= 2;
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
 * @param current pointer to  node which is currently being checked (first call
 * to this function should have this param set as the tree root).
 * @param freq the frequency, based on which the nodes should be added
 * to `nodes`.
 * @param nodes is the vector, to which pointers to nodes with frequency `freq`
 * will be pushed.
 */
void Huffman::get_nodes_by_freq(
    HuffmanNode *current, uint32_t freq, std::vector<HuffmanNode *> *nodes)
{
    if (current == nullptr) {
        return;
    }

    // Suitable node, push it to vector.
    if (current->freq == freq) {
        nodes->push_back(current);
    }

    // Recurse to subtrees
    get_nodes_by_freq(current->left, freq, nodes);
    get_nodes_by_freq(current->right, freq, nodes);
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
    HuffmanNode *current, uint32_t block_freq)
{
    std::vector<HuffmanNode *> nodes;
    get_nodes_by_freq(this->tree, block_freq, &nodes);

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

    uint16_t dummy = a->node_num;
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

    int8_t child_side = which_child(node->parent, node);

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