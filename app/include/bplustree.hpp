#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include <bits/stdc++.h>

template <typename T, size_t ORDER = 4>
class BPlusTree {
    static_assert(ORDER >= 3, "B+ Tree order must be at least 3");
private:
    struct Node {
        bool isLeaf;
        std::vector<T> keys;
        std::vector<Node*> children;
        Node* next; // Used for leaf nodes
    };

    Node* root;

public:
    BPlusTree() {
        root = new Node();
        root->isLeaf = true;
    }

    // Insert a key into the B+ Tree
    void insert(const T& key) {
        // Implementation of insertion logic
    }

    // Search for a key in the B+ Tree
    bool search(const T& key) const {
        // Implementation of search logic
    }

    // Print the B+ Tree
    void print() const {
        // Implementation of print logic
    }
};

#endif // BPLUSTREE_HPP