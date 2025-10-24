#include "artigo.h"
#include "hash_table.h"
#include "simple_btree.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <ID>" << std::endl;
    return 1;
  }

  int id;
  try {
    id = std::stoi(argv[1]);
  } catch (const std::exception &e) {
    std::cerr << "ID inválido: " << argv[1] << std::endl;
    return 1;
  }

  // Verifica se variáveis de ambiente estão definidas
  std::string data_dir =
      std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "/data/db";
  std::string hash_file = data_dir + "/dados.hash";
  std::string primary_index_file = data_dir + "/indice_primario.btree";

  std::cout << "=== SEEK1 - Busca via Índice Primário ===" << std::endl;
  std::cout << "Procurando ID: " << id << std::endl;
  std::cout << "Índice primário: " << primary_index_file << std::endl;
  std::cout << "Arquivo de dados: " << hash_file << std::endl;

  // Inicializa índice primário
  SimplePrimIdx indice_primario(primary_index_file);

  // Mede tempo de busca no índice
  auto start_time = std::chrono::high_resolution_clock::now();

  // Busca no índice primário
  PrimIdxEntry key_entry(id, 0);
  PrimIdxEntry result_entry;
  SearchStats index_stats = indice_primario.search(key_entry, result_entry);

  std::cout << std::endl << "=== BUSCA NO ÍNDICE PRIMÁRIO ===" << std::endl;
  std::cout << "Blocos lidos no índice: " << index_stats.blocos_lidos
            << std::endl;
  std::cout << "Total de blocos do índice: " << index_stats.total_blocos
            << std::endl;

  if (!index_stats.encontrado) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time);

    std::cout << "ID não encontrado no índice primário" << std::endl;
    std::cout << "Tempo total de busca: " << duration.count() << " µs"
              << std::endl;
    return 1;
  }

  std::cout << "ID encontrado no índice - posição hash: "
            << result_entry.posicao_hash << std::endl;

  // Agora busca o registro no arquivo de dados usando a posição do índice
  // Para simplificar, vamos usar a busca hash normal
  int table_size = 100000; // Valor padrão
  HashTable hash_table(hash_file, table_size);

  Artigo artigo;
  SearchStats data_stats = hash_table.search(id, artigo);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  std::cout << std::endl << "=== BUSCA NO ARQUIVO DE DADOS ===" << std::endl;
  std::cout << "Blocos lidos no arquivo de dados: " << data_stats.blocos_lidos
            << std::endl;

  std::cout << std::endl << "=== RESULTADOS DA BUSCA ===" << std::endl;

  if (data_stats.encontrado) {
    std::cout << "REGISTRO ENCONTRADO:" << std::endl;
    std::cout << "-------------------" << std::endl;
    artigo.print();
  } else {
    std::cout << "REGISTRO NÃO ENCONTRADO NO ARQUIVO DE DADOS" << std::endl;
  }

  std::cout << std::endl << "=== ESTATÍSTICAS TOTAIS ===" << std::endl;
  std::cout << "Total de blocos lidos (índice + dados): "
            << (index_stats.blocos_lidos + data_stats.blocos_lidos)
            << std::endl;
  std::cout << "Blocos do índice primário: " << index_stats.total_blocos
            << std::endl;
  std::cout << "Tempo total de busca: " << duration.count() << " µs"
            << std::endl;

  // Comparação de eficiência
  if (index_stats.total_blocos > 0 && data_stats.total_blocos > 0) {
    double index_percent =
        (double)index_stats.blocos_lidos / index_stats.total_blocos * 100.0;
    double total_blocks = index_stats.total_blocos + data_stats.total_blocos;
    double total_read = index_stats.blocos_lidos + data_stats.blocos_lidos;
    double overall_percent = total_read / total_blocks * 100.0;

    std::cout << "Porcentagem do índice acessada: " << std::fixed
              << std::setprecision(2) << index_percent << "%" << std::endl;
    std::cout << "Porcentagem total dos arquivos acessada: " << std::fixed
              << std::setprecision(2) << overall_percent << "%" << std::endl;
  }

  return data_stats.encontrado ? 0 : 1;
}