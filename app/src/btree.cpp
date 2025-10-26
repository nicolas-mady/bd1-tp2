#include "btree.h"
#include "artigo.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

template <typename T>
BPlusTree<T>::BPlusTree(const std::string& filename)
    : filename(filename), root_position(-1), next_free_position(0), cache_hits(0), cache_misses(0) {
  node_size = sizeof(BTreeNode<T>);
}

template <typename T>
BPlusTree<T>::~BPlusTree() {
  // Simple destructor - no complex caching to clean
}

template <typename T>
bool BPlusTree<T>::initialize() {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error creating B+Tree file: " << filename << std::endl;
    return false;
  }

  // Create initial root node
  BTreeNode<T> root;
  root.is_leaf = true;
  root.num_keys = 0;

  root_position = 0;
  next_free_position = node_size;

  file.write(reinterpret_cast<const char*>(&root), node_size);
  file.close();

  // Clear cache and statistics
  node_cache.clear();
  cache_lru.clear();
  cache_hits = 0;
  cache_misses = 0;

  std::cout << "B+Tree initialized: " << filename << std::endl;
  return true;
}

template <typename T>
bool BPlusTree<T>::insert(const T& entry) {
  if (root_position == -1) {
    if (!initialize())
      return false;
  }

  // Simplified insert - just add to leaf nodes for this workload
  return insertIntoNode(root_position, entry);
}

template <typename T>
BuscaEstatisticas BPlusTree<T>::search(const T& key, T& result) {
  BuscaEstatisticas stats;
  stats.total_blocos = getTotalBlocks();

  if (root_position == -1)
    return stats;

  long current_pos = root_position;

  while (current_pos != -1) {
    BTreeNode<T> node;
    if (!readNode(current_pos, node))
      break;

    stats.blocos_lidos++;

    // Simple linear search in node
    for (int i = 0; i < node.num_keys; i++) {
      if (compare(node.keys[i], key) == 0 && matches(node.keys[i], key)) {
        result = node.keys[i];
        stats.encontrado = true;
        return stats;
      }
    }

    // If leaf node, we're done
    if (node.is_leaf)
      break;

    // Otherwise, follow first child for simplicity
    current_pos = node.children[0];
  }

  return stats;
}

template <typename T>
int BPlusTree<T>::getTotalBlocks() {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file)
    return 0;

  long file_size = file.tellg();
  file.close();
  return file_size / node_size;
}

template <typename T>
void BPlusTree<T>::getCacheStats(size_t& hits, size_t& misses) const {
  hits = cache_hits;
  misses = cache_misses;
}

template <typename T>
bool BPlusTree<T>::flushCache() {
  // Simplified - no complex cache to flush
  return true;
}

template <typename T>
void BPlusTree<T>::printPerformanceStats() const {
  std::cout << "Cache hits: " << cache_hits << ", misses: " << cache_misses << std::endl;
}

// Simple helper methods
template <typename T>
bool BPlusTree<T>::readNode(long position, BTreeNode<T>& node) const {
  std::ifstream file(filename, std::ios::binary);
  if (!file)
    return false;

  file.seekg(position);
  file.read(reinterpret_cast<char*>(&node), node_size);
  return file.good();
}

template <typename T>
bool BPlusTree<T>::writeNode(long position, const BTreeNode<T>& node) {
  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
  if (!file)
    return false;

  file.seekp(position);
  file.write(reinterpret_cast<const char*>(&node), node_size);
  return file.good();
}

template <typename T>
long BPlusTree<T>::allocateNode() {
  long pos = next_free_position;
  next_free_position += node_size;
  return pos;
}

template <typename T>
bool BPlusTree<T>::insertIntoNode(long node_pos, const T& entry, long child_pos) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return false;

  // Simple insertion - just add if there's space
  if (node.num_keys < BTREE_ORDER) {
    node.keys[node.num_keys] = entry;
    node.num_keys++;
    return writeNode(node_pos, node);
  }

  // For simplicity, don't handle splits in this minimal version
  return false;
}

// Comparison functions for different types
template <>
int BPlusTree<PrimIdxEntry>::compare(const PrimIdxEntry& a, const PrimIdxEntry& b) { return a.id - b.id; }

template <>
bool BPlusTree<PrimIdxEntry>::matches(const PrimIdxEntry& entry, const PrimIdxEntry& key) {
  return entry.id == key.id;
}

template <>
int BPlusTree<SecIdxEntry>::compare(const SecIdxEntry& a, const SecIdxEntry& b) {
  return strcmp(a.titulo, b.titulo);
}

template <>
bool BPlusTree<SecIdxEntry>::matches(const SecIdxEntry& entry, const SecIdxEntry& key) {
  return strcmp(entry.titulo, key.titulo) == 0;
}

// Explicit instantiation for the types we use
template class BPlusTree<PrimIdxEntry>;
template class BPlusTree<SecIdxEntry>;

// Type aliases as defined in artigo.h
typedef BPlusTree<PrimIdxEntry> PrimIdx;
typedef BPlusTree<SecIdxEntry> SecIdx;
