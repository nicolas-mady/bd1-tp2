#include "b+tree.h"
#include "record.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
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

  if (!bptree.loadFromFile(secondary_index_file))
  {
    std::cerr << "Erro: não foi possível carregar o índice secundário" << std::endl;
    return 1;
  }

  auto load_end = std::chrono::high_resolution_clock::now();
  auto load_duration = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start);

  std::cout << "Índice carregado (lazy loading) em " << load_duration.count() << " µs" << std::endl;
  std::cout << "Nós carregados inicialmente: " << bptree.getLoadedNodesCount()
            << " de " << bptree.getTotalNodesCount() << std::endl;

  // Buscar títulos que contêm o termo fornecido
  auto search_start = std::chrono::high_resolution_clock::now();

  // Usar busca por prefixo (mais eficiente)
  auto results = bptree.searchByPrefix(titulo);

  // Se não encontrar com prefixo, tentar substring limitada
  if (results.empty())
  {
    results = bptree.searchBySubstring(titulo);
  }

  auto search_end = std::chrono::high_resolution_clock::now();
  auto search_duration = std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start);

  std::cout << std::endl
            << "=== SEARCH RESULTS ===" << std::endl;

  bool found = !results.empty();

  if (found)
  {
    std::cout << "RECORDS FOUND IN SECONDARY INDEX:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Found " << results.size() << " matching records:" << std::endl;

    // Mostrar até 5 resultados
    for (size_t i = 0; i < std::min(results.size(), size_t(5)); i++)
    {
      std::cout << "[" << (i + 1) << "] Title: \"" << results[i].first << "\"" << std::endl;
      std::cout << "    Article ID: " << results[i].second << std::endl;
    }

    if (results.size() > 5)
    {
      std::cout << "... and " << (results.size() - 5) << " more results" << std::endl;
    }
  }
  else
  {
    std::cout << "RECORD NOT FOUND IN SECONDARY INDEX" << std::endl;
    std::cout << "No titles containing \"" << titulo << "\" were found" << std::endl;
  }

  std::cout << std::endl
            << "=== SEARCH STATISTICS ===" << std::endl;
  std::cout << "Total nodes in tree: " << bptree.getTotalNodesCount() << std::endl;
  std::cout << "Nodes loaded during search: " << bptree.getLoadedNodesCount() << std::endl;
  std::cout << "Index load time: " << load_duration.count() << " µs" << std::endl;
  std::cout << "Search time: " << search_duration.count() << " µs" << std::endl;

  if (bptree.getTotalNodesCount() > 0)
  {
    double access_percent = (double)bptree.getLoadedNodesCount() / bptree.getTotalNodesCount() * 100.0;
    std::cout << "Nodes accessed: " << std::fixed << std::setprecision(2) << access_percent << "%" << std::endl;
  }

  std::cout << std::endl
            << "=== TITLE SEARCH ANALYSIS ===" << std::endl;
  std::cout << "Search strategy: Lazy loading B+Tree with substring matching" << std::endl;
  std::cout << "Nodes loaded: " << bptree.getLoadedNodesCount() << std::endl;
  std::cout << "Search type: Title substring -> ID mapping" << std::endl;

  return found ? 0 : 1;
}
