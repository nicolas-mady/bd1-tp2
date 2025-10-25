#ifndef BTREE_H
#define BTREE_H

#include "artigo.h"
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>

// Simplified B+Tree order for this specific workload
#define BTREE_ORDER 16

// Simplified cache size
#define NODE_CACHE_SIZE 256

// Nó da B+Tree
template <typename T>
struct BTreeNode
{
  bool is_leaf;
  int num_keys;
  T keys[BTREE_ORDER];
  long children[BTREE_ORDER + 1]; // Posições dos filhos no arquivo
  long next_leaf;                 // Para folhas: ponteiro para próxima folha

  BTreeNode() : is_leaf(true), num_keys(0), next_leaf(-1)
  {
    for (int i = 0; i <= BTREE_ORDER; i++)
    {
      children[i] = -1;
    }
  }
};

// Classe B+Tree genérica
template <typename T>
class BPlusTree
{
private:
  std::string filename;
  long root_position;
  int node_size;
  long next_free_position; // Para controle de alocação

  // Sistema de cache LRU para nós
  struct CacheNode
  {
    BTreeNode<T> node;
    bool dirty; // Indica se o nó foi modificado

    CacheNode() : dirty(false) {}
  };

  mutable std::unordered_map<long, CacheNode> node_cache;
  mutable std::list<long> cache_lru; // Lista para controle LRU
  mutable size_t cache_hits;
  mutable size_t cache_misses;

public:
  BPlusTree(const std::string &filename);
  ~BPlusTree();

  // Inicializa a árvore
  bool initialize();

  // Insere uma entrada
  bool insert(const T &entry);

  // Busca uma entrada
  BuscaEstatisticas search(const T &key, T &result);

  // Obtém total de blocos
  int getTotalBlocks();

  // Obtém estatísticas do cache
  void getCacheStats(size_t &hits, size_t &misses) const;

  // Força escrita de todos os nós sujos do cache
  bool flushCache();

  // Imprime estatísticas de performance
  void printPerformanceStats() const;

private:
  // Métodos auxiliares para manipulação de nós
  bool readNode(long position, BTreeNode<T> &node) const;
  bool writeNode(long position, const BTreeNode<T> &node);
  long allocateNode();

  // Métodos de cache
  bool getNodeFromCache(long position, BTreeNode<T> &node) const;
  void putNodeInCache(long position, const BTreeNode<T> &node, bool dirty = false) const;
  void evictOldestCacheEntry() const;
  bool writeNodeToFile(long position, const BTreeNode<T> &node);

  // Métodos para inserção
  bool insertIntoNode(long node_pos, const T &entry, long child_pos = -1);
  long splitNode(long node_pos, T &promoted_key);
  bool insertIntoLeaf(long node_pos, const T &entry);
  bool insertIntoInternal(long node_pos, const T &entry, long child_pos);
  int findChildIndex(const BTreeNode<T> &node, const T &key);
  void insertIntoSortedArray(T keys[], int &num_keys, const T &entry, int max_size);
  bool insertWithSplit(long node_pos, const T &entry, T &promoted_key, long &new_node_pos);

  // Métodos para busca
  BuscaEstatisticas searchInNode(long node_pos, const T &key, T &result,
                                 int &blocos_lidos);

  // Funções de comparação específicas para cada tipo
  int compare(const T &a, const T &b);
  bool matches(const T &entry, const T &key);
};

// Especializações das B+Trees
using PrimIdx = BPlusTree<PrimIdxEntry>;
using SecIdx = BPlusTree<SecIdxEntry>;

#endif // BTREE_H