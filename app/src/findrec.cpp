#include <bits/stdc++.h>
#include <record.h>

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

  std::string hash_file_name = "data/db/hash.dat";

  std::cout << "=== FINDREC ===" << std::endl;
  std::cout << "Arquivo: " << hash_file_name << std::endl;
  std::cout << "Procurando ID: " << id << std::endl;
  std::cout << std::endl;

  // Estima tamanho da tabela hash (seria melhor armazenar em metadados)
  // int table_size = 100000; // Valor padrão - em implementação real seria lido de metadados

  // HashTable hash_table(hash_file, table_size);

  // Mede tempo de busca
  auto start_time = std::chrono::high_resolution_clock::now();

  // char line_check[sizeof(Record)];
  int count = 0;
  std::ifstream in(hash_file_name, std::ios::binary);
  // in.seekg(0, std::ios::end);
  // std::cout << std::endl << "Tamanho do arquivo hash: " << in.tellg() << std::endl;
  long offset = (id % MAP_SIZE) * sizeof(Record) * 2;
  Record rec;
  in.seekg(offset, std::ios::beg);
  in.read(reinterpret_cast<char*>(&rec), sizeof(Record));
  in.close();
  rec.print();

  // // in.getline(&line_check[0], sizeof(Record));
  // in.read(line_check, sizeof(Record));
  // std::cout << line_check << std::endl;
  // // while (line_check.find_first_not_of('\0') != std::string::npos) {
  //   count++;
  // // in.getline(&line_check[0], sizeof(Record) + 1);
  // }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  // if (stats.encontrado)
  // {
  //   std::cout << "REGISTRO ENCONTRADO:" << std::endl;
  //   std::cout << "-------------------" << std::endl;
  //   artigo.print();
  // }
  // else
  // {
  //   std::cout << "REGISTRO NÃO ENCONTRADO" << std::endl;
  // }

  // std::cout << std::endl << "=== ESTATÍSTICAS DE ACESSO ===" << std::endl;
  // std::cout << "Blocos lidos: " << stats.blocos_lidos << std::endl;
  // std::cout << "Total de blocos no arquivo: " << stats.total_blocos << std::endl;
  std::cout << std::endl;
  std::cout << "Tempo de busca: " << duration.count() << " µs" << std::endl;

  // if (stats.total_blocos > 0)
  // {
  //   double percent = (double)stats.blocos_lidos / stats.total_blocos * 100.0;
  //   std::cout << "Porcentagem do arquivo acessada: " << std::fixed
  //             << std::setprecision(2) << percent << "%" << std::endl;
  // }

  // return stats.encontrado ? 0 : 1;
}
