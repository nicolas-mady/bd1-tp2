#ifndef SIMPLE_BTREE_H
#define SIMPLE_BTREE_H

#include "artigo.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

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

// Implementação simplificada de B+Tree usando std::vector para facilitar
// Em uma implementação real seria completamente em disco

template <typename T> class SimpleBTree {
private:
  std::string filename;
  std::vector<T> entries;
  bool loaded;

public:
  SimpleBTree(const std::string &filename)
      : filename(filename), loaded(false) {}

  bool initialize() {
    // Cria arquivo vazio se não existir
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    file.close();
    return loadFromDisk();
  }

  bool insert(const T &entry) {
    if (!loaded && !loadFromDisk()) {
      return false;
    }

    // Inserção ordenada simples
    auto pos = std::lower_bound(
        entries.begin(), entries.end(), entry,
        [this](const T &a, const T &b) { return compare(a, b) < 0; });
    entries.insert(pos, entry);

    return saveToDisk();
  }

  BuscaEstatisticas search(const T &key, T &result) {
    BuscaEstatisticas stats;
    stats.total_blocos = getTotalBlocks();
    stats.blocos_lidos = 1; // Simplificado - assume 1 bloco lido

    if (!loaded && !loadFromDisk()) {
      return stats;
    }

    // Busca binária
    auto it = std::lower_bound(
        entries.begin(), entries.end(), key,
        [this](const T &a, const T &b) { return compare(a, b) < 0; });

    if (it != entries.end() && matches(*it, key)) {
      result = *it;
      stats.encontrado = true;
    }

    return stats;
  }

  int getTotalBlocks() {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) {
      return 0;
    }
    return std::max(1, (int)((st.st_size + BLOCK_SIZE - 1) / BLOCK_SIZE));
  }

private:
  bool loadFromDisk() {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
      return false;

    entries.clear();

    // Lê número de entradas
    size_t count;
    file.read(reinterpret_cast<char *>(&count), sizeof(count));

    if (file.gcount() != sizeof(count)) {
      loaded = true;
      return true; // Arquivo vazio
    }

    // Lê entradas
    entries.resize(count);
    for (size_t i = 0; i < count; i++) {
      file.read(reinterpret_cast<char *>(&entries[i]), sizeof(T));
    }

    file.close();
    loaded = true;
    return true;
  }

  bool saveToDisk() {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
      return false;

    // Escreve número de entradas
    size_t count = entries.size();
    file.write(reinterpret_cast<const char *>(&count), sizeof(count));

    // Escreve entradas
    for (const auto &entry : entries) {
      file.write(reinterpret_cast<const char *>(&entry), sizeof(T));
    }

    file.close();
    return true;
  }

  // Funções de comparação específicas para cada tipo (implementadas nas
  // especializações)
  int compare(const T &a, const T &b);
  bool matches(const T &entry, const T &key);
};

// Especializações das funções de comparação para IndicePrimarioEntry
template <>
inline int
SimpleBTree<IndicePrimarioEntry>::compare(const IndicePrimarioEntry &a,
                                          const IndicePrimarioEntry &b) {
  return a.id - b.id;
}

template <>
inline bool
SimpleBTree<IndicePrimarioEntry>::matches(const IndicePrimarioEntry &entry,
                                          const IndicePrimarioEntry &key) {
  return entry.id == key.id;
}

// Especializações das funções de comparação para IndiceSecundarioEntry
template <>
inline int
SimpleBTree<IndiceSecundarioEntry>::compare(const IndiceSecundarioEntry &a,
                                            const IndiceSecundarioEntry &b) {
  return strcmp(a.titulo, b.titulo);
}

template <>
inline bool
SimpleBTree<IndiceSecundarioEntry>::matches(const IndiceSecundarioEntry &entry,
                                            const IndiceSecundarioEntry &key) {
  return strcmp(entry.titulo, key.titulo) == 0;
}

// Aliases para usar no lugar das antigas definições
using SimpleIndicePrimario = SimpleBTree<IndicePrimarioEntry>;
using SimpleIndiceSecundario = SimpleBTree<IndiceSecundarioEntry>;

#endif // SIMPLE_BTREE_H