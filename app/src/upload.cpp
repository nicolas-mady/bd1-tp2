#include "artigo.h"
#include "hash_table.h"
#include "simple_btree.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
// #include <lazycsv>

// Função para fazer parse de uma linha CSV
std::vector<std::string> parseCSVLine(const std::string &line) {
  std::vector<std::string> fields;
  std::string field;
  bool inQuotes = false;

  for (size_t i = 0; i < line.length(); i++) {
    char c = line[i];

    if (c == '"') {
      inQuotes = !inQuotes;
    } else if (c == ';' && !inQuotes) {
      fields.push_back(trim(field));
      field.clear();
    } else {
      field += c;
    }
  }

  // Adiciona último campo
  fields.push_back(trim(field));

  return fields;
}

// Remove aspas duplas do início e fim de uma string
std::string removeQuotes(const std::string &str) {
  if (str.length() >= 2 && str[0] == '"' && str[str.length() - 1] == '"') {
    return str.substr(1, str.length() - 2);
  }
  return str;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <arquivo_csv>" << std::endl;
    return 1;
  }

  std::string csv_filename = argv[1];

  // Verifica se variáveis de ambiente estão definidas
  std::string data_dir =
      std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "data/db";

  std::cout << "=== UPLOAD - Carregamento de Dados ===" << std::endl;
  std::cout << "Arquivo CSV: " << csv_filename << std::endl;
  std::cout << "Diretório de dados: " << data_dir << std::endl;

  // Cria diretório se não existir
  system(("mkdir -p " + data_dir).c_str());

  // Nomes dos arquivos de dados e índices
  std::string hash_file = data_dir + "/hash.dat";
  std::string primary_index_file = data_dir + "/prim_idx.dat";
  std::string secondary_index_file = data_dir + "/sec_idx.dat";

  // Abre arquivo CSV
  std::ifstream csv_file(csv_filename);
  if (!csv_file) {
    std::cerr << "Erro ao abrir arquivo CSV: " << csv_filename << std::endl;
    return 1;
  }

  // Conta número de linhas para dimensionar tabela hash
  std::string line;
  int total_records = 0;
  while (std::getline(csv_file, line)) {
    if (!line.empty())
      total_records++;
  }
  csv_file.clear();
  csv_file.seekg(0);

  std::cout << "Total de registros estimado: " << total_records << std::endl;

  // Dimensiona tabela hash (fator de carga ~0.7)
  int table_size = (total_records * 10) / 7;

  // Inicializa estruturas
  HashTable hash_table(hash_file, table_size);
  SimplePrimIdx indice_primario(primary_index_file);
  SimpleIndiceSecundario indice_secundario(secondary_index_file);

  if (!hash_table.initialize()) {
    std::cerr << "Erro ao inicializar tabela hash" << std::endl;
    return 1;
  }

  if (!indice_primario.initialize()) {
    std::cerr << "Erro ao inicializar índice primário" << std::endl;
    return 1;
  }

  if (!indice_secundario.initialize()) {
    std::cerr << "Erro ao inicializar índice secundário" << std::endl;
    return 1;
  }

  // Processa arquivo CSV
  int records_processed = 0;
  int records_inserted = 0;
  auto start_time = std::chrono::high_resolution_clock::now();

  while (std::getline(csv_file, line)) {
    if (line.empty())
      continue;

    records_processed++;

    try {
      std::vector<std::string> fields = parseCSVLine(line);

      if (fields.size() < 7) {
        std::cerr << "Linha inválida (poucos campos): " << records_processed
                  << std::endl;
        continue;
      }

      // Extrai campos do CSV
      int id = std::stoi(removeQuotes(fields[0]));
      std::string titulo = removeQuotes(fields[1]);
      int ano = std::stoi(removeQuotes(fields[2]));
      std::string autores = removeQuotes(fields[3]);
      int citacoes = std::stoi(removeQuotes(fields[4]));
      std::string data_atualizacao = removeQuotes(fields[5]);
      std::string snippet = removeQuotes(fields[6]);

      // Cria registro
      Artigo artigo(id, titulo, ano, autores, citacoes, data_atualizacao,
                    snippet);

      // Insere na tabela hash
      if (hash_table.insert(artigo)) {
        // Calcula posição hash para o índice primário
        long hash_position = Artigo::hash_id(id, table_size);

        // Cria entradas para os índices
        PrimIdxEntry primary_entry(id, hash_position);
        SecIdxEntry secondary_entry(titulo, id);

        // Insere nos índices
        indice_primario.insert(primary_entry);
        indice_secundario.insert(secondary_entry);

        records_inserted++;

        if (records_inserted % 1000 == 0) {
          std::cout << "Inseridos: " << records_inserted << " registros..."
                    << std::endl;
        }
      } else {
        std::cerr << "Erro ao inserir registro ID: " << id << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Erro ao processar linha " << records_processed << ": "
                << e.what() << std::endl;
    }
  }

  csv_file.close();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - start_time);

  std::cout << std::endl << "=== ESTATÍSTICAS DE CARREGAMENTO ===" << std::endl;
  std::cout << "Registros processados: " << records_processed << std::endl;
  std::cout << "Registros inseridos: " << records_inserted << std::endl;
  std::cout << "Tempo de execução: " << duration.count() << " ms" << std::endl;
  std::cout << "Taxa de inserção: "
            << (records_inserted * 1000.0 / duration.count()) << " registros/s"
            << std::endl;

  std::cout << std::endl << "=== ARQUIVOS CRIADOS ===" << std::endl;
  std::cout << "Arquivo de dados (hash): " << hash_file << std::endl;
  std::cout << "Índice primário (B+Tree): " << primary_index_file << std::endl;
  std::cout << "Índice secundário (B+Tree): " << secondary_index_file
            << std::endl;

  return 0;
}
