#include "b+tree.h"
#include "record.h"
#include <bits/stdc++.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <ID>" << std::endl;
    return 1;
  }

  int id;
  try {
    id = std::stoi(argv[1]);
  } catch (const std::exception& e) {
    std::cerr << "ID inválido: " << argv[1] << std::endl;
    return 1;
  }

  std::string data_dir = std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "data/db";
  std::string primary_index_file = data_dir + "/idx1.bin";

  std::cout << "=== SEEK1 - Search via Primary Index (B+Tree Lazy Loading) ===" << std::endl;
  std::cout << "Looking for ID: " << id << std::endl;
  std::cout << "Primary index: " << primary_index_file << std::endl;

  // Carregar árvore B+ com lazy loading
  BPlusTree<int> bptree(170);

  auto load_start = std::chrono::high_resolution_clock::now();

  if (!bptree.loadFromFile(primary_index_file)) {
    std::cerr << "Erro: não foi possível carregar o índice primário" << std::endl;
    return 1;
  }

  auto load_end = std::chrono::high_resolution_clock::now();
  auto load_duration = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start);

  std::cout << "Índice carregado (lazy loading) em " << load_duration.count() << " µs" << std::endl;
  std::cout << "Nós carregados inicialmente: " << bptree.getLoadedNodesCount()
            << " de " << bptree.getTotalNodesCount() << std::endl;

  // Buscar o ID
  auto search_start = std::chrono::high_resolution_clock::now();

  auto result = bptree.search(id);

  auto search_end = std::chrono::high_resolution_clock::now();
  auto search_duration = std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start);

  std::cout << std::endl
            << "=== SEARCH RESULTS ===" << std::endl;

  if (result != nullptr) {
    std::cout << "RECORD FOUND IN PRIMARY INDEX:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Node contains " << result->keys.size() << " keys" << std::endl;
  } else {
    std::cout << "RECORD NOT FOUND IN PRIMARY INDEX" << std::endl;
  }

  std::cout << std::endl
            << "=== SEARCH STATISTICS ===" << std::endl;
  std::cout << "Total nodes in tree: " << bptree.getTotalNodesCount() << std::endl;
  std::cout << "Nodes loaded during search: " << bptree.getLoadedNodesCount() << std::endl;
  std::cout << "Index load time: " << load_duration.count() << " µs" << std::endl;
  std::cout << "Search time: " << search_duration.count() << " µs" << std::endl;

  if (bptree.getTotalNodesCount() > 0) {
    double access_percent = (double) bptree.getLoadedNodesCount() / bptree.getTotalNodesCount() * 100.0;
    std::cout << "Nodes accessed: " << std::fixed << std::setprecision(2) << access_percent << "%" << std::endl;
  }

  return result != nullptr ? 0 : 1;
}
