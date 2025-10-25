#include "artigo.h"
#include "btree.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Uso: " << argv[0] << " <ID>" << std::endl;
    return 1;
  }

  int id;
  try
  {
    id = std::stoi(argv[1]);
  }
  catch (const std::exception &e)
  {
    std::cerr << "ID inválido: " << argv[1] << std::endl;
    return 1;
  }

  std::string data_dir = std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "data/db";
  std::string primary_index_file = data_dir + "/prim_idx.dat";

  std::cout << "=== SEEK1 - Search via Primary Index (B+Tree Only) ===" << std::endl;
  std::cout << "Looking for ID: " << id << std::endl;
  std::cout << "Primary index: " << primary_index_file << std::endl;

  // Initialize primary index
  PrimIdx indice_primario(primary_index_file);

  auto start_time = std::chrono::high_resolution_clock::now();

  // Search in primary index only
  PrimIdxEntry key_entry(id, 0);
  PrimIdxEntry result_entry;
  BuscaEstatisticas index_stats = indice_primario.search(key_entry, result_entry);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  std::cout << std::endl
            << "=== SEARCH RESULTS ===" << std::endl;

  if (index_stats.encontrado)
  {
    std::cout << "RECORD FOUND IN PRIMARY INDEX:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "ID: " << result_entry.id << std::endl;
    std::cout << "Hash Position: " << result_entry.posicao_hash << std::endl;
  }
  else
  {
    std::cout << "RECORD NOT FOUND IN PRIMARY INDEX" << std::endl;
  }

  std::cout << std::endl
            << "=== SEARCH STATISTICS ===" << std::endl;
  std::cout << "Index blocks read: " << index_stats.blocos_lidos << std::endl;
  std::cout << "Total index blocks: " << index_stats.total_blocos << std::endl;
  std::cout << "Search time: " << duration.count() << " µs" << std::endl;

  if (index_stats.total_blocos > 0)
  {
    double access_percent = (double)index_stats.blocos_lidos / index_stats.total_blocos * 100.0;
    std::cout << "Index accessed: " << std::fixed << std::setprecision(2)
              << access_percent << "%" << std::endl;
  }

  return index_stats.encontrado ? 0 : 1;
}