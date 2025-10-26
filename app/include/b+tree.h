#ifndef BPLUSTREE_H
#define BPLUSTREE_H

// #include "record.h"
#include <bits/stdc++.h>

int binary_search(const std::vector<std::string>& vec, const std::string& value) {
  int left = 0;
  int right = vec.size() - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (vec[mid] == value)
      return mid;
    else if (vec[mid] < value)
      left = mid + 1;
    else
      right = mid - 1;
  }
  return -1;
}

template <typename T> //, typename Compare = std::less<T>>
class BPlusTree {
private:

  struct Node {
    bool isLeaf;
    std::vector<T> keys;
    std::vector<Node*> children;
    Node* next;
  };

  Node* root;
  int m;

  /* Node* findLeaf(Node* node, const T& key) {
    if (node->isLeaf)
      return node;
    for (size_t i = 0; i < node->keys.size(); ++i)
      if (key < node->keys[i])
        return findLeaf(node->children[i], key);
    return findLeaf(node->children.back(), key);
  } */

  std::vector<Node*> findLeaf(const T& key) {
    std::vector<Node*> path;
    auto node = root;
    while (!node->isLeaf) {
      path.push_back(node);
      auto it = std::upper_bound(node->keys.begin(), node->keys.end(), key);

      if (it == node->keys.end()) {
        node = node->children.back();
      } else {
        int i = std::distance(node->keys.begin(), it);
        node = node->children[i];
      }
    }
    path.push_back(node);
    return path;
  }

  void split(Node* node, std::vector<Node*> path) {
    if (node->keys.size() <= 2 * m)
      return;

    Node* newNode = new Node();
    newNode->isLeaf = node->isLeaf;
    newNode->keys.assign(node->keys.begin() + m, node->keys.end());
    node->keys.resize(m);
    if (node->isLeaf) {
      newNode->next = node->next;
      node->next = newNode;
    } else {
      newNode->children.assign(node->children.begin() + m, node->children.end());
      node->children.resize(m);
    }

    T midKey;
    if (node->isLeaf) {
      midKey = newNode->keys[0];
    } else {
      midKey = node->keys.back();
      node->keys.pop_back();
      newNode->children.assign(node->children.begin() + m + 1, node->children.end());
      node->children.resize(m + 1);
    }
  }

public:

  BPlusTree(int m) : m(m) {
    root = new Node();
    root->isLeaf = true;
    root->next = nullptr;
  }

  void insert(const T& key) {
    auto path = findLeaf(key);
    auto leaf = path.back();
    path.pop_back();
    leaf->keys.push_back(key);
    std::sort(leaf->keys.begin(), leaf->keys.end());
    split(leaf, path);
  }

  Node* search(const T& key) const {
    auto leaf = findLeaf(key).back();
    return std::binary_search(leaf->keys.begin(), leaf->keys.end(), key) ? leaf : nullptr;
  }
};

#endif // BPLUSTREE_H
