#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <bits/stdc++.h>

template <typename T>
class BPlusTree {
private:

  struct Node {
    bool isLeaf;
    std::vector<T> keys;
    std::vector<Node*> children;
    Node* next;
  };
  Compare cmp;
  Node* root;

  // Node* findLeaf(Node* node, const T& key) {
  //   if (node->isLeaf)
  //     return node;
  //   for (size_t i = 0; i < node->keys.size(); ++i)
  //     if (key < node->keys[i])
  //       return findLeaf(node->children[i], key);
  //   return findLeaf(node->children.back(), key);
  // }
  Node* findLeaf(const T& key) {
    auto node = root;
    while (!node->isLeaf) {
      // int i = std::lower_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
      int i = std::upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
      node = node->children[i];
    }
    return node;
  }


public:

  BPlusTree() {
    root = new Node();
    root->isLeaf = true;
  }

  void insert(const T& key) {
  }

  bool search(const T& key) const {
  }

  void print() const {
  }
};

#endif
