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

  Node* root;
  int m;

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

  void insertInternal(Node* parent, Node* child, const T& key) {
    auto it = std::upper_bound(parent->keys.begin(), parent->keys.end(), key);
    int i = std::distance(parent->keys.begin(), it);
    parent->keys.insert(it, key);
    parent->children.insert(parent->children.begin() + i + 1, child);

    if (parent->keys.size() > 2 * m) {
      Node* newParent = new Node();
      newParent->isLeaf = false;

      T midKey = parent->keys[m];
      parent->keys.erase(parent->keys.begin() + m);

      newParent->keys.assign(parent->keys.begin() + m, parent->keys.end());
      parent->keys.erase(parent->keys.begin() + m, parent->keys.end());

      newParent->children.assign(parent->children.begin() + m + 1, parent->children.end());
      parent->children.erase(parent->children.begin() + m + 1, parent->children.end());

      Node* grandParent = findParent(root, parent);
      if (grandParent == nullptr) {
        Node* newRoot = new Node();
        newRoot->isLeaf = false;
        newRoot->keys.push_back(midKey);
        newRoot->children.push_back(parent);
        newRoot->children.push_back(newParent);
        root = newRoot;
      } else {
        insertInternal(grandParent, newParent, midKey);
      }
    }
  }

  Node* findParent(Node* node, Node* child) {
    if (node == nullptr || node->isLeaf)
      return nullptr;

    for (auto c : node->children)
      if (c == child)
        return node;

    for (auto c : node->children) {
      Node* parent = findParent(c, child);
      if (parent != nullptr)
        return parent;
    }
    return nullptr;
  }

  // Generic separator function (for int, etc.)
  template <typename U = T>
  typename std::enable_if<!std::is_same<U, std::string>::value, T>::type
  get_separator(const T& key, const T& last_key_in_prev_node) {
    return key;
  }

  // Specialized separator function for std::string (prefix optimization)
  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::string>::value, std::string>::type
  get_separator(const std::string& key, const std::string& last_key_in_prev_node) {
    size_t len = 0;
    while (len < key.length() && len < last_key_in_prev_node.length() && key[len] == last_key_in_prev_node[len])
      len++;
    if (len < key.length())
      return key.substr(0, len + 1);
    return key;
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

    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    leaf->keys.insert(it, key);

    if (leaf->keys.size() > 2 * m) {
      Node* newLeaf = new Node();
      newLeaf->isLeaf = true;

      T last_key_in_old_leaf = leaf->keys.back();

      newLeaf->keys.assign(leaf->keys.begin() + m, leaf->keys.end());
      leaf->keys.erase(leaf->keys.begin() + m, leaf->keys.end());

      newLeaf->next = leaf->next;
      leaf->next = newLeaf;

      T midKey = get_separator(newLeaf->keys[0], last_key_in_old_leaf);
      Node* parent = findParent(root, leaf);

      if (parent == nullptr) {
        Node* newRoot = new Node();
        newRoot->isLeaf = false;
        newRoot->keys.push_back(midKey);
        newRoot->children.push_back(leaf);
        newRoot->children.push_back(newLeaf);
        root = newRoot;
      } else {
        insertInternal(parent, newLeaf, midKey);
      }
    }
  }

  Node* search(const T& key) const {
    auto leaf = findLeaf(key).back();
    return std::binary_search(leaf->keys.begin(), leaf->keys.end(), key) ? leaf : nullptr;
  }

  void traverse() const {
    auto node = root;
    while (!node->isLeaf)
      node = node->children[0];

    while (node != nullptr) {
      for (const auto& key : node->keys)
        std::cout << key << std::endl;
      node = node->next;
    }
    std::cout << std::endl;
  }
};

#endif // BPLUSTREE_H
