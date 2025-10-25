#include "artigo.h"
#include "hash_table.h"
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

  // Verifica se variáveis de ambiente estão definidas
  std::string data_dir =
      std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "data/db";
  std::string hash_file = data_dir + "/dados.hash";

  std::cout << "=== FINDREC - Busca Direta no Arquivo Hash ===" << std::endl;
  std::cout << "Procurando ID: " << id << std::endl;
  std::cout << "Arquivo: " << hash_file << std::endl;

  // Estima tamanho da tabela hash (seria melhor armazenar em metadados)
  int table_size =
      100000; // Valor padrão - em implementação real seria lido de metadados

  HashTable hash_table(hash_file, table_size);

  // Mede tempo de busca
  auto start_time = std::chrono::high_resolution_clock::now();

  Artigo artigo;
  BuscaEstatisticas stats = hash_table.search(id, artigo);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  std::cout << std::endl
            << "=== RESULTADOS DA BUSCA ===" << std::endl;

  if (stats.encontrado)
  {
    std::cout << "REGISTRO ENCONTRADO:" << std::endl;
    std::cout << "-------------------" << std::endl;
    artigo.print();
  }
  else
  {
    std::cout << "REGISTRO NÃO ENCONTRADO" << std::endl;
  }

  std::cout << std::endl
            << "=== ESTATÍSTICAS DE ACESSO ===" << std::endl;
  std::cout << "Blocos lidos: " << stats.blocos_lidos << std::endl;
  std::cout << "Total de blocos no arquivo: " << stats.total_blocos
            << std::endl;
  std::cout << "Tempo de busca: " << duration.count() << " µs" << std::endl;

  if (stats.total_blocos > 0)
  {
    double percent = (double)stats.blocos_lidos / stats.total_blocos * 100.0;
    std::cout << "Porcentagem do arquivo acessada: " << std::fixed
              << std::setprecision(2) << percent << "%" << std::endl;
  }

  return stats.encontrado ? 0 : 1;
}