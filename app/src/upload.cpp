#include <bits/stdc++.h>
#include <record.hpp>

std::string trim(std::string& field) {
  std::string trimmed = field;
  trimmed.erase(trimmed.begin(), find_if(trimmed.begin(), trimmed.end(), [](char c) { return !isspace(c); }));
  trimmed.erase(find_if(trimmed.rbegin(), trimmed.rend(), [](char c) { return !isspace(c); }).base(), trimmed.end());
  return trimmed;
}

std::vector<std::string> parse(std::string& line) {
  std::vector<std::string> fields;
  std::string field;
  bool inQuotes = false;

  for (char c : line) {
    if (c == '"') {
      inQuotes = !inQuotes;
    } else if (c == ';' && !inQuotes) {
      fields.push_back(trim(field));
      field.clear();
    } else {
      field += c;
    }
  }

  fields.push_back(trim(field));
  return fields;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "uso: " << argv[0] << " <arquivo_csv>" << std::endl;
    return EXIT_FAILURE;
  }

  system("rm -rf data/db");
  system("mkdir data/db");

  std::string csv_file_name = argv[1];
  std::string hash_file_name = "data/db/hash.dat";
  // std::string primary_index_file = "data/db/idx1.dat";
  // std::string secondary_index_file = "data/db/idx2.dat";
  // PrimIdx indice_primario(primary_index_file);
  // SecIdx indice_secundario(secondary_index_file);

  std::vector<std::vector<Record>> hashmap(MAP_SIZE);
  int processed = 0;
  std::ifstream csv_file(csv_file_name);
  
  if (!csv_file) {
    std::cerr << "erro: não foi possível abrir " << csv_file_name << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "=== upload " << csv_file_name << " ===" << std::endl;

  auto t0 = std::chrono::high_resolution_clock::now();

  for (std::string line; getline(csv_file, line); processed++) {
    std::vector<std::string> fields = parse(line);
    Record artigo(fields);
    hashmap[artigo.id % MAP_SIZE].push_back(artigo);
    // long hash_position = Artigo::hash_id(id, MAP_SIZE);
    // PrimIdxEntry prim_entry(id, hash_position);
    // SecIdxEntry sec_entry(titulo, id);
    // indice_primario.insert(prim_entry);
    // indice_secundario.insert(sec_entry);
    if (processed % 100000 == 0 && processed > 0) {
      auto tx = std::chrono::high_resolution_clock::now();
      auto t = std::chrono::duration_cast<std::chrono::seconds>(tx - t0);
      std::cout << " [" << t.count() << "s] " << processed << " registros processados" << std::endl;
    }
  }

  csv_file.close();

  std::cout << std::endl;
  std::cout << "escrevendo " << hash_file_name << "..." << std::endl;

  std::ofstream out(hash_file_name, std::ios::binary);

  for (auto& bucket : hashmap) {
    int count = 0;

    for (const Record& record : bucket) {
      out.write(reinterpret_cast<const char*>(&record), sizeof(Record));
      count++;
    }

    for (; count < 2; count++)
      out.write(std::string(sizeof(Record), '\0').c_str(), sizeof(Record));
  }

  long file_size = out.tellp();
  out.close();

  auto tx = std::chrono::high_resolution_clock::now();
  auto t = std::chrono::duration_cast<std::chrono::seconds>(tx - t0);

  int num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

  std::cout << " [" << t.count() << "s] " << num_blocks << " blocos escritos" << std::endl;

  return EXIT_SUCCESS;
}
