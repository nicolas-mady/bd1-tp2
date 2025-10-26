#include "artigo.h"
#include "btree.h"
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
}

template <typename T>
bool BPlusTree<T>::initialize() {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error creating B+Tree file: " << filename << std::endl;
    return false;
  }

  BTreeNode<T> root;
  root.is_leaf = true;
  root.num_keys = 0;

  root_position = 0;
  next_free_position = node_size;

  file.write(reinterpret_cast<const char*>(&root), node_size);
  file.close();

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

    for (int i = 0; i < node.num_keys; i++) {
      if (node.keys[i] == key) {
        result = node.keys[i];
        stats.encontrado = true;
        return stats;
      }
    }

    if (node.is_leaf)
      break;

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
  return true;
}

template <typename T>
void BPlusTree<T>::printPerformanceStats() const {
  std::cout << "Cache hits: " << cache_hits << ", misses: " << cache_misses << std::endl;
}

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

  if (node.num_keys < BTREE_ORDER) {
    node.keys[node.num_keys] = entry;
    node.num_keys++;
    return writeNode(node_pos, node);
  }

  return false;
}

template class BPlusTree<PrimIdxEntry>;
template class BPlusTree<SecIdxEntry>;

typedef BPlusTree<PrimIdxEntry> PrimIdx;
typedef BPlusTree<SecIdxEntry> SecIdx;

template <typename T>
BuscaEstatisticas BPlusTree<T>::search(const T& key, T& result) {
  BuscaEstatisticas stats;
  stats.total_blocos = getTotalBlocks();

  if (root_position == -1)
    return stats;

  return searchInNode(root_position, key, result, stats.blocos_lidos);
}

template <typename T>
int BPlusTree<T>::getTotalBlocks() {
  struct stat st;
  if (stat(filename.c_str(), &st) != 0)
    return 0;
  return (st.st_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

template <typename T>
bool BPlusTree<T>::readNode(long position, BTreeNode<T>& node) const {
  if (getNodeFromCache(position, node)) {
    cache_hits++;
    return true;
  }

  cache_misses++;
  std::ifstream file(filename, std::ios::binary);
  if (!file)
    return false;

  file.seekg(position);
  file.read(reinterpret_cast<char*>(&node), node_size);
  file.close();

  if (file.gcount() == node_size) {
    putNodeInCache(position, node, false);
    return true;
  }

  return false;
}

template <typename T>
bool BPlusTree<T>::writeNode(long position, const BTreeNode<T>& node) {
  putNodeInCache(position, node, true);

  if (node_cache.size() < NODE_CACHE_SIZE * 0.95)
    return true;

  return writeNodeToFile(position, node);
}

template <typename T>
long BPlusTree<T>::allocateNode() {
  long position = next_free_position;
  next_free_position += node_size;
  return position;
}

template <typename T>
bool BPlusTree<T>::insertIntoNode(long node_pos, const T& entry, long child_pos) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return false;

  if (node.is_leaf)
    return insertIntoLeaf(node_pos, entry);
  else
    return insertIntoInternal(node_pos, entry, child_pos);
}

template <typename T>
bool BPlusTree<T>::insertIntoLeaf(long node_pos, const T& entry) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return false;

  if (node.num_keys < BTREE_ORDER - 1) {
    insertIntoSortedArray(node.keys, node.num_keys, entry, BTREE_ORDER);
    return writeNode(node_pos, node);
  }

  T promoted_key;
  long new_node_pos = splitNode(node_pos, promoted_key);

  if (new_node_pos == -1)
    return false;

  BTreeNode<T> left_node, right_node;
  readNode(node_pos, left_node);
  readNode(new_node_pos, right_node);

  if (compare(entry, promoted_key) < 0) {
    insertIntoSortedArray(left_node.keys, left_node.num_keys, entry, BTREE_ORDER);
    writeNode(node_pos, left_node);
  } else {
    insertIntoSortedArray(right_node.keys, right_node.num_keys, entry, BTREE_ORDER);
    writeNode(new_node_pos, right_node);
  }

  if (node_pos == root_position) {
    BTreeNode<T> new_root;
    new_root.is_leaf = false;
    new_root.num_keys = 1;
    new_root.keys[0] = promoted_key;
    new_root.children[0] = node_pos;
    new_root.children[1] = new_node_pos;

    long new_root_pos = allocateNode();
    writeNode(new_root_pos, new_root);
    root_position = new_root_pos;
  } else {
    std::cerr << "Split de nó não-raiz não implementado completamente" << std::endl;
  }

  return true;
}

template <typename T>
bool BPlusTree<T>::insertIntoInternal(long node_pos, const T& entry, long child_pos) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return false;

  int child_index = findChildIndex(node, entry);

  if (!insertIntoNode(node.children[child_index], entry, child_pos))
    return false;

  return true;
}

template <typename T>
long BPlusTree<T>::splitNode(long node_pos, T& promoted_key) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return -1;

  BTreeNode<T> new_node;
  new_node.is_leaf = node.is_leaf;

  int mid = BTREE_ORDER / 2;

  if (node.is_leaf) {
    for (int i = mid; i < node.num_keys; i++)
      new_node.keys[i - mid] = node.keys[i];
    new_node.num_keys = node.num_keys - mid;
    node.num_keys = mid;

    new_node.next_leaf = node.next_leaf;

    promoted_key = new_node.keys[0];
  } else {
    promoted_key = node.keys[mid];

    for (int i = mid + 1; i < node.num_keys; i++)
      new_node.keys[i - mid - 1] = node.keys[i];

    for (int i = mid + 1; i <= node.num_keys; i++)
      new_node.children[i - mid - 1] = node.children[i];

    new_node.num_keys = node.num_keys - mid - 1;
    node.num_keys = mid;
  }

  long new_node_pos = allocateNode();
  writeNode(new_node_pos, new_node);

  if (node.is_leaf)
    node.next_leaf = new_node_pos;
  writeNode(node_pos, node);

  return new_node_pos;
}

template <typename T>
int BPlusTree<T>::findChildIndex(const BTreeNode<T>& node, const T& key) {
  int left = 0, right = node.num_keys;
  while (left < right) {
    int mid = left + (right - left) / 2;
    if (compare(key, node.keys[mid]) >= 0)
      left = mid + 1;
    else
      right = mid;
  }
  return left;
}

template <typename T>
void BPlusTree<T>::insertIntoSortedArray(T keys[], int& num_keys, const T& entry, int max_size) {
  if (num_keys >= max_size - 1)
    return;

  int left = 0, right = num_keys;
  while (left < right) {
    int mid = left + (right - left) / 2;
    if (compare(keys[mid], entry) < 0)
      left = mid + 1;
    else
      right = mid;
  }
  int pos = left;

  if (pos < num_keys)
    std::memmove(&keys[pos + 1], &keys[pos], (num_keys - pos) * sizeof(T));

  keys[pos] = entry;
  num_keys++;
}

template <typename T>
bool BPlusTree<T>::insertWithSplit(long node_pos, const T& entry, T& promoted_key, long& new_node_pos) {
  new_node_pos = -1;

  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return false;

  if (node.is_leaf) {
    if (node.num_keys < BTREE_ORDER - 1) {
      insertIntoSortedArray(node.keys, node.num_keys, entry, BTREE_ORDER);
      return writeNode(node_pos, node);
    } else {
      T temp_keys[BTREE_ORDER + 1];
      int temp_num_keys = 0;

      for (int i = 0; i < node.num_keys; i++)
        temp_keys[temp_num_keys++] = node.keys[i];

      int pos = 0;
      while (pos < temp_num_keys && compare(temp_keys[pos], entry) < 0)
        pos++;

      for (int i = temp_num_keys; i > pos; i--)
        temp_keys[i] = temp_keys[i - 1];
      temp_keys[pos] = entry;
      temp_num_keys++;

      int mid = temp_num_keys / 2;

      node.num_keys = mid;
      for (int i = 0; i < mid; i++)
        node.keys[i] = temp_keys[i];

      BTreeNode<T> new_node;
      new_node.is_leaf = true;
      new_node.num_keys = temp_num_keys - mid;
      for (int i = mid; i < temp_num_keys; i++)
        new_node.keys[i - mid] = temp_keys[i];

      new_node.next_leaf = node.next_leaf;
      new_node_pos = allocateNode();
      node.next_leaf = new_node_pos;

      promoted_key = new_node.keys[0];

      writeNode(node_pos, node);
      writeNode(new_node_pos, new_node);

      return true;
    }
  } else {
    int child_index = findChildIndex(node, entry);

    T child_promoted_key;
    long child_new_node_pos;

    if (!insertWithSplit(node.children[child_index], entry, child_promoted_key, child_new_node_pos))
      return false;

    if (child_new_node_pos == -1)
      return true;

    if (node.num_keys < BTREE_ORDER - 1) {
      insertIntoSortedArray(node.keys, node.num_keys, child_promoted_key, BTREE_ORDER);

      int ptr_pos = 0;
      while (ptr_pos < node.num_keys - 1 && compare(node.keys[ptr_pos], child_promoted_key) < 0)
        ptr_pos++;
      ptr_pos++;

      for (int i = node.num_keys; i > ptr_pos; i--)
        node.children[i] = node.children[i - 1];
      node.children[ptr_pos] = child_new_node_pos;

      return writeNode(node_pos, node);
    } else {
      T temp_keys[BTREE_ORDER + 1];
      long temp_children[BTREE_ORDER + 2];

      int j = 0;
      bool inserted = false;
      for (int i = 0; i < node.num_keys; i++) {
        if (!inserted && compare(child_promoted_key, node.keys[i]) < 0) {
          temp_keys[j++] = child_promoted_key;
          inserted = true;
        }
        temp_keys[j++] = node.keys[i];
      }
      if (!inserted)
        temp_keys[j] = child_promoted_key;
      int temp_num_keys = node.num_keys + 1;

      for (int i = 0; i <= child_index; i++)
        temp_children[i] = node.children[i];
      temp_children[child_index + 1] = child_new_node_pos;
      for (int i = child_index + 1; i <= node.num_keys; i++)
        temp_children[i + 1] = node.children[i];

      int mid = temp_num_keys / 2;
      promoted_key = temp_keys[mid];

      node.num_keys = mid;
      for (int i = 0; i < mid; i++) {
        node.keys[i] = temp_keys[i];
        node.children[i] = temp_children[i];
      }
      node.children[mid] = temp_children[mid];

      BTreeNode<T> new_internal_node;
      new_internal_node.is_leaf = false;
      new_internal_node.num_keys = temp_num_keys - mid - 1;

      for (int i = 0; i < new_internal_node.num_keys; i++) {
        new_internal_node.keys[i] = temp_keys[mid + 1 + i];
        new_internal_node.children[i] = temp_children[mid + 1 + i];
      }
      new_internal_node.children[new_internal_node.num_keys] = temp_children[temp_num_keys];

      new_node_pos = allocateNode();

      writeNode(node_pos, node);
      writeNode(new_node_pos, new_internal_node);

      return true;
    }
  }
}

template <typename T>
BuscaEstatisticas BPlusTree<T>::searchInNode(long node_pos, const T& key, T& result, int& blocos_lidos) {
  BuscaEstatisticas stats;
  blocos_lidos++;

  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return stats;

  if (node.is_leaf) {
    int left = 0, right = node.num_keys - 1;
    while (left <= right) {
      int mid = (left + right) / 2;
      int cmp = compare(node.keys[mid], key);

      if (cmp == 0 && matches(node.keys[mid], key)) {
        result = node.keys[mid];
        stats.encontrado = true;
        return stats;
      } else if (cmp < 0) {
        left = mid + 1;
      } else {
        right = mid - 1;
      }
    }
  } else {
    int child_index = findChildIndex(node, key);
    if (child_index <= node.num_keys && node.children[child_index] != -1)
      return searchInNode(node.children[child_index], key, result, blocos_lidos);
  }

  return stats;
}

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

template <typename T>
bool BPlusTree<T>::getNodeFromCache(long position, BTreeNode<T>& node) const {
  auto it = node_cache.find(position);
  if (it != node_cache.end()) {
    node = it->second.node;

    auto lru_it = std::find(cache_lru.begin(), cache_lru.end(), position);
    if (lru_it != cache_lru.end())
      cache_lru.erase(lru_it);
    cache_lru.push_back(position);

    return true;
  }
  return false;
}

template <typename T>
void BPlusTree<T>::putNodeInCache(long position, const BTreeNode<T>& node, bool dirty) const {
  if (node_cache.size() >= NODE_CACHE_SIZE)
    evictOldestCacheEntry();

  CacheNode& cache_node = node_cache[position];
  cache_node.node = node;
  cache_node.dirty = dirty;

  auto lru_it = std::find(cache_lru.begin(), cache_lru.end(), position);
  if (lru_it != cache_lru.end())
    cache_lru.erase(lru_it);
  cache_lru.push_back(position);
}

template <typename T>
void BPlusTree<T>::evictOldestCacheEntry() const {
  if (cache_lru.empty())
    return;

  long oldest_pos = cache_lru.front();
  cache_lru.pop_front();

  auto it = node_cache.find(oldest_pos);
  if (it != node_cache.end()) {
    if (it->second.dirty)
      const_cast<BPlusTree<T>*>(this)->writeNodeToFile(oldest_pos, it->second.node);
    node_cache.erase(it);
  }
}

template <typename T>
bool BPlusTree<T>::writeNodeToFile(long position, const BTreeNode<T>& node) {
  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
  if (!file) {
    file.open(filename, std::ios::binary | std::ios::out);
    if (!file)
      return false;
    file.close();
    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file)
      return false;
  }

  file.seekp(position);
  file.write(reinterpret_cast<const char*>(&node), node_size);
  file.close();

  return true;
}

template <typename T>
bool BPlusTree<T>::flushCache() {
  for (auto& entry : node_cache) {
    if (entry.second.dirty) {
      if (!writeNodeToFile(entry.first, entry.second.node))
        return false;
      entry.second.dirty = false;
    }
  }
  return true;
}

template <typename T>
void BPlusTree<T>::getCacheStats(size_t& hits, size_t& misses) const {
  hits = cache_hits;
  misses = cache_misses;
}

template <typename T>
void BPlusTree<T>::printPerformanceStats() const {
  size_t total = cache_hits + cache_misses;
  double hit_rate = total > 0 ? (double) cache_hits / total * 100.0 : 0.0;

  std::cout << "\n=== ESTATÍSTICAS DE PERFORMANCE B+Tree ===" << std::endl;
  std::cout << "Cache hits: " << cache_hits << std::endl;
  std::cout << "Cache misses: " << cache_misses << std::endl;
  std::cout << "Hit rate: " << std::fixed << std::setprecision(2) << hit_rate << "%" << std::endl;
  std::cout << "Nós em cache: " << node_cache.size() << "/" << NODE_CACHE_SIZE << std::endl;
  std::cout << "Tamanho do nó: " << node_size << " bytes" << std::endl;
  std::cout << "Ordem da árvore: " << BTREE_ORDER << std::endl;
  std::cout << "==========================================" << std::endl;
}

template class BPlusTree<PrimIdxEntry>;
template class BPlusTree<SecIdxEntry>;
