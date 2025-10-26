#include "artigo.h"
#include "btree.h"
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
  std::string secondary_index_file = data_dir + "/sec_idx.dat";

  std::cout << "=== SEEK2 - Search via Secondary Index (B+Tree Only) ===" << std::endl;
  std::cout << "Looking for title: \"" << titulo << "\"" << std::endl;
  std::cout << "Secondary index: " << secondary_index_file << std::endl;

  SecIdx indice_secundario(secondary_index_file);

  auto start_time = std::chrono::high_resolution_clock::now();

  SecIdxEntry key_entry(titulo, 0);
  SecIdxEntry result_entry;
  BuscaEstatisticas index_stats = indice_secundario.search(key_entry, result_entry);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  std::cout << std::endl
            << "=== SEARCH RESULTS ===" << std::endl;

  if (index_stats.encontrado) {
    std::cout << "RECORD FOUND IN SECONDARY INDEX:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Title: \"" << result_entry.titulo << "\"" << std::endl;
    std::cout << "Article ID: " << result_entry.id << std::endl;
  } else {
    std::cout << "RECORD NOT FOUND IN SECONDARY INDEX" << std::endl;
  }

  std::cout << std::endl
            << "=== SEARCH STATISTICS ===" << std::endl;
  std::cout << "Index blocks read: " << index_stats.blocos_lidos << std::endl;
  std::cout << "Total index blocks: " << index_stats.total_blocos << std::endl;
  std::cout << "Search time: " << duration.count() << " µs" << std::endl;

  if (index_stats.total_blocos > 0) {
    double access_percent = (double) index_stats.blocos_lidos / index_stats.total_blocos * 100.0;
    std::cout << "Index accessed: " << std::fixed << std::setprecision(2) << access_percent << "%" << std::endl;
  }

  std::cout << std::endl
            << "=== TITLE SEARCH ANALYSIS ===" << std::endl;
  std::cout << "Simplified search: Direct lookup in secondary B+Tree index" << std::endl;
  std::cout << "Total blocks accessed: " << index_stats.blocos_lidos << std::endl;
  std::cout << "Search type: Title -> ID mapping only" << std::endl;

  return index_stats.encontrado ? 0 : 1;
}
