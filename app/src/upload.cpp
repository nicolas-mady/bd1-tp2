#include "artigo.h"
#include "hash_table.h"
#include "btree.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Fixed constants for this specific dataset
#define EXPECTED_RECORDS 1021439
#define FIXED_TABLE_SIZE 1500000

// Simple CSV parser for semicolon-separated values
std::vector<std::string> parseCSVLine(const std::string &line)
{
  std::vector<std::string> fields;
  std::string field;
  bool inQuotes = false;

  for (char c : line)
  {
    if (c == '"')
    {
      inQuotes = !inQuotes;
    }
    else if (c == ';' && !inQuotes)
    {
      fields.push_back(trim(field));
      field.clear();
    }
    else
    {
      field += c;
    }
  }
  fields.push_back(trim(field));
  return fields;
}

std::string removeQuotes(const std::string &str)
{
  if (str.length() >= 2 && str[0] == '"' && str[str.length() - 1] == '"')
  {
    return str.substr(1, str.length() - 2);
  }
  return str;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Uso: " << argv[0] << " <arquivo_csv>" << std::endl;
    return 1;
  }

  std::string csv_filename = argv[1];
  std::string data_dir = std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "data/db";

  std::cout << "=== UPLOAD - Simplified for Fixed Dataset ===" << std::endl;
  std::cout << "Expected records: " << EXPECTED_RECORDS << std::endl;
  std::cout << "Table size: " << FIXED_TABLE_SIZE << std::endl;

  // Create data directory
  system(("mkdir -p " + data_dir).c_str());

  // File names
  std::string hash_file = data_dir + "/hash.dat";
  std::string primary_index_file = data_dir + "/prim_idx.dat";
  std::string secondary_index_file = data_dir + "/sec_idx.dat";

  // Initialize structures with fixed sizes
  HashTable hash_table(hash_file, FIXED_TABLE_SIZE);
  PrimIdx indice_primario(primary_index_file);
  SecIdx indice_secundario(secondary_index_file);

  if (!hash_table.initialize() || !indice_primario.initialize() || !indice_secundario.initialize())
  {
    std::cerr << "Error initializing data structures" << std::endl;
    return 1;
  }

  std::cout << "Structures initialized successfully!" << std::endl;

  // Process CSV file
  std::ifstream csv_file(csv_filename);
  if (!csv_file)
  {
    std::cerr << "Error opening CSV file: " << csv_filename << std::endl;
    return 1;
  }

  int records_processed = 0;
  int records_inserted = 0;
  auto start_time = std::chrono::high_resolution_clock::now();

  std::string line;
  while (std::getline(csv_file, line) && records_processed < EXPECTED_RECORDS)
  {
    if (line.empty())
      continue;

    records_processed++;

    try
    {
      std::vector<std::string> fields = parseCSVLine(line);
      if (fields.size() < 7)
        continue;

      int id = std::stoi(removeQuotes(fields[0]));
      std::string titulo = removeQuotes(fields[1]);
      int ano = std::stoi(removeQuotes(fields[2]));
      std::string autores = removeQuotes(fields[3]);
      int citacoes = std::stoi(removeQuotes(fields[4]));
      std::string data_atualizacao = removeQuotes(fields[5]);
      std::string snippet = removeQuotes(fields[6]);

      Artigo artigo(id, titulo, ano, autores, citacoes, data_atualizacao, snippet);

      // Insert into hash table
      if (hash_table.insert(artigo))
      {
        records_inserted++;

        // Insert into indices
        long hash_position = Artigo::hash_id(id, FIXED_TABLE_SIZE);
        PrimIdxEntry prim_entry(id, hash_position);
        SecIdxEntry sec_entry(titulo, id);

        indice_primario.insert(prim_entry);
        indice_secundario.insert(sec_entry);
      }

      // Progress tracking every 10k records
      if (records_processed % 10000 == 0)
      {
        std::cout << "Processed: " << records_processed << "/" << EXPECTED_RECORDS
                  << " (" << (100.0 * records_processed / EXPECTED_RECORDS) << "%)" << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error processing line " << records_processed << ": " << e.what() << std::endl;
    }
  }

  csv_file.close();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  std::cout << std::endl
            << "=== LOADING STATISTICS ===" << std::endl;
  std::cout << "Records processed: " << records_processed << std::endl;
  std::cout << "Records inserted: " << records_inserted << std::endl;
  std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
  std::cout << "Insertion rate: " << (records_inserted * 1000.0 / duration.count()) << " records/s" << std::endl;

  std::cout << std::endl
            << "=== FILES CREATED ===" << std::endl;
  std::cout << "Hash file: " << hash_file << std::endl;
  std::cout << "Primary index: " << primary_index_file << std::endl;
  std::cout << "Secondary index: " << secondary_index_file << std::endl;

  return 0;
}
