#include "b+tree.h"
#include "record.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " \"<Título>\"" << std::endl;
    std::cerr << "Exemplo: " << argv[0] << " \"Design and implementation\"" << std::endl;
    return 1;
  }

  std::string titulo = argv[1];
  std::string data_dir = std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "data/db";
  std::string secondary_index_file = data_dir + "/idx2.bin";

  std::cout << "=== SEEK2 - Search via Secondary Index (B+Tree Lazy Loading) ===" << std::endl;
  std::cout << "Looking for title: \"" << titulo << "\"" << std::endl;
  std::cout << "Secondary index: " << secondary_index_file << std::endl;

  // Carregar árvore B+ com lazy loading
  BPlusTree<std::pair<std::string, int>> bptree(6);

  auto load_start = std::chrono::high_resolution_clock::now();

  if (!bptree.loadFromFile(secondary_index_file)) {
    std::cerr << "Erro: não foi possível carregar o índice secundário" << std::endl;
    return 1;
  }

  auto load_end = std::chrono::high_resolution_clock::now();
  auto load_duration = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start);

  std::cout << "Índice carregado (lazy loading) em " << load_duration.count() << " µs" << std::endl;
  std::cout << "Nós carregados inicialmente: " << bptree.getLoadedNodesCount()
            << " de " << bptree.getTotalNodesCount() << std::endl;

  // Buscar títulos que começam com o termo fornecido
  auto search_start = std::chrono::high_resolution_clock::now();

  // Buscar uma correspondência exata primeiro
  std::pair<std::string, int> searchKey = {titulo, 0};
  auto result = bptree.search(searchKey);

  auto search_end = std::chrono::high_resolution_clock::now();
  auto search_duration = std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start);

  std::cout << std::endl
            << "=== SEARCH RESULTS ===" << std::endl;

  bool found = false;
  int foundId = -1;
  std::string foundTitle;

  if (result != nullptr) {
    // Procurar na folha por títulos que contêm o termo
    for (const auto& pair : result->keys) {
      if (pair.first.find(titulo) != std::string::npos) {
        std::cout << "RECORD FOUND IN SECONDARY INDEX:" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << "Title: \"" << pair.first << "\"" << std::endl;
        std::cout << "Article ID: " << pair.second << std::endl;
        found = true;
        foundId = pair.second;
        foundTitle = pair.first;
        break;
      }
    }
  }

  if (!found) {
    std::cout << "RECORD NOT FOUND IN SECONDARY INDEX" << std::endl;
    std::cout << "Títulos começando com \"" << titulo.substr(0, 3) << "\" não encontrados" << std::endl;
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

  std::cout << std::endl
            << "=== TITLE SEARCH ANALYSIS ===" << std::endl;
  std::cout << "Search strategy: Lazy loading B+Tree with substring matching" << std::endl;
  std::cout << "Nodes loaded: " << bptree.getLoadedNodesCount() << std::endl;
  std::cout << "Search type: Title substring -> ID mapping" << std::endl;

  return found ? 0 : 1;
}
