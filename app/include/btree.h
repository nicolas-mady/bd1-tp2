#ifndef BTREE_H
#define BTREE_H

#include "artigo.h"
#include <cstring>
#include <string>
#include <vector>

// Ordem da B+Tree (número máximo de chaves por nó)
#define BTREE_ORDER 100

// Estrutura para entrada do índice primário (ID -> posição no arquivo hash)
struct IndicePrimarioEntry {
  int id;
  long posicao_hash;

  IndicePrimarioEntry() : id(0), posicao_hash(0) {}
  IndicePrimarioEntry(int id, long pos) : id(id), posicao_hash(pos) {}
};

// Estrutura para entrada do índice secundário (Título -> ID)
struct IndiceSecundarioEntry {
  char titulo[MAX_TITULO + 1];
  int id;

  IndiceSecundarioEntry() : id(0) { memset(titulo, 0, sizeof(titulo)); }

  IndiceSecundarioEntry(const std::string &t, int i) : id(i) {
    strncpy(titulo, t.c_str(), MAX_TITULO);
    titulo[MAX_TITULO] = '\0';
  }
};

// Nó da B+Tree
template <typename T> struct BTreeNode {
  bool is_leaf;
  int num_keys;
  T keys[BTREE_ORDER];
  long children[BTREE_ORDER + 1]; // Posições dos filhos no arquivo
  long next_leaf;                 // Para folhas: ponteiro para próxima folha

  BTreeNode() : is_leaf(true), num_keys(0), next_leaf(-1) {
    for (int i = 0; i <= BTREE_ORDER; i++) {
      children[i] = -1;
    }
  }
};

// Classe B+Tree genérica
template <typename T> class BPlusTree {
private:
  std::string filename;
  long root_position;
  int node_size;

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

private:
  // Métodos auxiliares para manipulação de nós
  bool readNode(long position, BTreeNode<T> &node);
  bool writeNode(long position, const BTreeNode<T> &node);
  long allocateNode();

  // Métodos para inserção
  bool insertIntoNode(long node_pos, const T &entry, long child_pos = -1);
  long splitNode(long node_pos, T &promoted_key);

  // Métodos para busca
  BuscaEstatisticas searchInNode(long node_pos, const T &key, T &result,
                                 int &blocos_lidos);

  // Funções de comparação específicas para cada tipo
  int compare(const T &a, const T &b);
  bool matches(const T &entry, const T &key);
};

// Especializações das B+Trees
using IndicePrimario = BPlusTree<IndicePrimarioEntry>;
using IndiceSecundario = BPlusTree<IndiceSecundarioEntry>;

#endif // BTREE_H