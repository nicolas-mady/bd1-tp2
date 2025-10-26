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
      if (node.keys[i] == key) {
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

// Explicit instantiation for the types we use
template class BPlusTree<PrimIdxEntry>;
template class BPlusTree<SecIdxEntry>;

// Type aliases as defined in artigo.h
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
  // Tenta primeiro do cache
  if (getNodeFromCache(position, node)) {
    cache_hits++;
    return true;
  }

  // Se não está no cache, lê do arquivo
  cache_misses++;
  std::ifstream file(filename, std::ios::binary);
  if (!file)
    return false;

  file.seekg(position);
  file.read(reinterpret_cast<char*>(&node), node_size);
  file.close();

  if (file.gcount() == node_size) {
    // Adiciona ao cache após leitura bem-sucedida
    putNodeInCache(position, node, false);
    return true;
  }

  return false;
}

template <typename T>
bool BPlusTree<T>::writeNode(long position, const BTreeNode<T>& node) {
  // Atualiza no cache como dirty (será escrito depois)
  putNodeInCache(position, node, true);

  // Estratégia lazy mais agressiva: só escreve quando necessário
  if (node_cache.size() < NODE_CACHE_SIZE * 0.95)
    return true; // Write-behind: escrita será feita pelo flush ou eviction

  // Se cache quase cheio, força escrita
  return writeNodeToFile(position, node);
}

template <typename T>
long BPlusTree<T>::allocateNode() {
  // Usa controle de posição ao invés de calcular sempre
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

  // Se há espaço no nó folha
  if (node.num_keys < BTREE_ORDER - 1) {
    insertIntoSortedArray(node.keys, node.num_keys, entry, BTREE_ORDER);
    return writeNode(node_pos, node);
  }

  // Nó está cheio, precisa fazer split
  T promoted_key;
  long new_node_pos = splitNode(node_pos, promoted_key);

  if (new_node_pos == -1)
    return false;

  // Decide em qual nó inserir a nova entrada
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

  // Se este é o nó raiz, cria uma nova raiz
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
    // Propaga a chave promovida para o pai
    // Implementação simplificada - precisaria encontrar o pai
    std::cerr << "Split de nó não-raiz não implementado completamente" << std::endl;
  }

  return true;
}

template <typename T>
bool BPlusTree<T>::insertIntoInternal(long node_pos, const T& entry, long child_pos) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return false;

  // Encontra o filho apropriado para inserir
  int child_index = findChildIndex(node, entry);

  // Recursivamente insere no filho
  if (!insertIntoNode(node.children[child_index], entry, child_pos))
    return false;

  return true;
}

template <typename T>
long BPlusTree<T>::splitNode(long node_pos, T& promoted_key) {
  BTreeNode<T> node;
  if (!readNode(node_pos, node))
    return -1;

  // Cria novo nó
  BTreeNode<T> new_node;
  new_node.is_leaf = node.is_leaf;

  // Ponto médio para o split
  int mid = BTREE_ORDER / 2;

  if (node.is_leaf) {
    // Para nós folha, copia metade das chaves para o novo nó
    for (int i = mid; i < node.num_keys; i++)
      new_node.keys[i - mid] = node.keys[i];
    new_node.num_keys = node.num_keys - mid;
    node.num_keys = mid;

    // Liga os nós folha
    new_node.next_leaf = node.next_leaf;

    // A chave promovida é a primeira chave do novo nó
    promoted_key = new_node.keys[0];
  } else {
    // Para nós internos, promove a chave do meio
    promoted_key = node.keys[mid];

    // Move chaves para o novo nó
    for (int i = mid + 1; i < node.num_keys; i++)
      new_node.keys[i - mid - 1] = node.keys[i];

    // Move ponteiros para filhos
    for (int i = mid + 1; i <= node.num_keys; i++)
      new_node.children[i - mid - 1] = node.children[i];

    new_node.num_keys = node.num_keys - mid - 1;
    node.num_keys = mid;
  }

  // Escreve o novo nó no arquivo
  long new_node_pos = allocateNode();
  writeNode(new_node_pos, new_node);

  // Atualiza o nó original
  if (node.is_leaf)
    node.next_leaf = new_node_pos;
  writeNode(node_pos, node);

  return new_node_pos;
}

template <typename T>
int BPlusTree<T>::findChildIndex(const BTreeNode<T>& node, const T& key) {
  // Busca binária para melhor performance
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
    return; // Array cheio

  // Busca binária para encontrar posição (mais eficiente que busca linear)
  int left = 0, right = num_keys;
  while (left < right) {
    int mid = left + (right - left) / 2;
    if (compare(keys[mid], entry) < 0)
      left = mid + 1;
    else
      right = mid;
  }
  int pos = left;

  // Usa memmove para deslocamento mais eficiente
  if (pos < num_keys)
    std::memmove(&keys[pos + 1], &keys[pos], (num_keys - pos) * sizeof(T));

  // Insere novo elemento
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
    // Nó folha
    if (node.num_keys < BTREE_ORDER - 1) {
      // Há espaço, apenas insere
      insertIntoSortedArray(node.keys, node.num_keys, entry, BTREE_ORDER);
      return writeNode(node_pos, node);
    } else {
      // Nó cheio, precisa fazer split
      // Primeiro adiciona a chave temporariamente
      T temp_keys[BTREE_ORDER + 1];
      int temp_num_keys = 0;

      // Copia todas as chaves existentes
      for (int i = 0; i < node.num_keys; i++)
        temp_keys[temp_num_keys++] = node.keys[i];

      // Insere a nova chave na posição correta
      int pos = 0;
      while (pos < temp_num_keys && compare(temp_keys[pos], entry) < 0)
        pos++;

      for (int i = temp_num_keys; i > pos; i--)
        temp_keys[i] = temp_keys[i - 1];
      temp_keys[pos] = entry;
      temp_num_keys++;

      // Faz o split
      int mid = temp_num_keys / 2;

      // Atualiza nó original (parte esquerda)
      node.num_keys = mid;
      for (int i = 0; i < mid; i++)
        node.keys[i] = temp_keys[i];

      // Cria novo nó (parte direita)
      BTreeNode<T> new_node;
      new_node.is_leaf = true;
      new_node.num_keys = temp_num_keys - mid;
      for (int i = mid; i < temp_num_keys; i++)
        new_node.keys[i - mid] = temp_keys[i];

      // Liga os nós folha
      new_node.next_leaf = node.next_leaf;
      new_node_pos = allocateNode();
      node.next_leaf = new_node_pos;

      // A chave promovida é a primeira chave do novo nó
      promoted_key = new_node.keys[0];

      // Escreve os nós
      writeNode(node_pos, node);
      writeNode(new_node_pos, new_node);

      return true;
    }
  } else {
    // Nó interno
    int child_index = findChildIndex(node, entry);

    T child_promoted_key;
    long child_new_node_pos;

    // Insere recursivamente no filho
    if (!insertWithSplit(node.children[child_index], entry, child_promoted_key, child_new_node_pos))
      return false;

    // Se não houve split no filho, não há mais nada a fazer
    if (child_new_node_pos == -1)
      return true;

    // Houve split no filho, precisa inserir a chave promovida neste nó
    if (node.num_keys < BTREE_ORDER - 1) {
      // Há espaço para a chave promovida
      insertIntoSortedArray(node.keys, node.num_keys, child_promoted_key, BTREE_ORDER);

      // Precisa também inserir o ponteiro para o novo nó filho
      // Encontra onde inserir o ponteiro
      int ptr_pos = 0;
      while (ptr_pos < node.num_keys - 1 && compare(node.keys[ptr_pos], child_promoted_key) < 0)
        ptr_pos++;
      ptr_pos++; // O ponteiro vai depois da chave

      // Desloca ponteiros para a direita
      for (int i = node.num_keys; i > ptr_pos; i--)
        node.children[i] = node.children[i - 1];
      node.children[ptr_pos] = child_new_node_pos;

      return writeNode(node_pos, node);
    } else {
      // Nó interno também está cheio, precisa fazer split
      // Primeiro insere a chave promovida temporariamente
      T temp_keys[BTREE_ORDER + 1];
      long temp_children[BTREE_ORDER + 2];

      // Copia todas as chaves existentes e insere a nova na posição correta
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

      // Copia ponteiros e insere o novo ponteiro
      for (int i = 0; i <= child_index; i++)
        temp_children[i] = node.children[i];
      temp_children[child_index + 1] = child_new_node_pos;
      for (int i = child_index + 1; i <= node.num_keys; i++)
        temp_children[i + 1] = node.children[i];

      // Faz o split
      int mid = temp_num_keys / 2;
      promoted_key = temp_keys[mid];

      // Atualiza nó original (lado esquerdo)
      node.num_keys = mid;
      for (int i = 0; i < mid; i++) {
        node.keys[i] = temp_keys[i];
        node.children[i] = temp_children[i];
      }
      node.children[mid] = temp_children[mid];

      // Cria novo nó interno (lado direito)
      BTreeNode<T> new_internal_node;
      new_internal_node.is_leaf = false;
      new_internal_node.num_keys = temp_num_keys - mid - 1;

      for (int i = 0; i < new_internal_node.num_keys; i++) {
        new_internal_node.keys[i] = temp_keys[mid + 1 + i];
        new_internal_node.children[i] = temp_children[mid + 1 + i];
      }
      new_internal_node.children[new_internal_node.num_keys] = temp_children[temp_num_keys];

      // Aloca posição para o novo nó
      new_node_pos = allocateNode();

      // Escreve os nós
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
    // Busca binária nas folhas
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
    // Nó interno - encontra filho apropriado
    int child_index = findChildIndex(node, key);
    if (child_index <= node.num_keys && node.children[child_index] != -1)
      return searchInNode(node.children[child_index], key, result, blocos_lidos);
  }

  return stats;
}

// Especializações das funções de comparação

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

// ============= IMPLEMENTAÇÃO DO SISTEMA DE CACHE =============

template <typename T>
bool BPlusTree<T>::getNodeFromCache(long position, BTreeNode<T>& node) const {
  auto it = node_cache.find(position);
  if (it != node_cache.end()) {
    node = it->second.node;

    // Move para o fim da lista LRU (mais recentemente usado)
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
  // Se cache está cheio, remove o mais antigo
  if (node_cache.size() >= NODE_CACHE_SIZE)
    evictOldestCacheEntry();

  // Adiciona/atualiza nó no cache
  CacheNode& cache_node = node_cache[position];
  cache_node.node = node;
  cache_node.dirty = dirty;

  // Atualiza LRU
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
    // Se o nó está dirty, escreve antes de remover
    if (it->second.dirty)
      const_cast<BPlusTree<T>*>(this)->writeNodeToFile(oldest_pos, it->second.node);
    node_cache.erase(it);
  }
}

template <typename T>
bool BPlusTree<T>::writeNodeToFile(long position, const BTreeNode<T>& node) {
  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
  if (!file) {
    // Se arquivo não existe, cria
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

// ============= FIM DO SISTEMA DE CACHE =============

// Instanciações explícitas
template class BPlusTree<PrimIdxEntry>;
template class BPlusTree<SecIdxEntry>;
