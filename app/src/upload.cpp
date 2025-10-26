#include <bits/stdc++.h>
#include <record.h>

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

std::string removeQuotes(std::string& field) {
  if (field.length() >= 2 && field[0] == '"' && field[field.length() - 1] == '"')
    return field.substr(1, field.length() - 2);
  return field;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <csv_file>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string csv_file_name = argv[1];
  system("rm -rf data/db");
  system("mkdir data/db");
  std::string hash_file_name = "data/db/hash.dat";
  // std::string primary_index_file = "data/db/idx1.dat";
  // std::string secondary_index_file = "data/db/idx2.dat";
  // HashTable hash_table(hash_os, MAP_SIZE);
  // PrimIdx indice_primario(primary_index_file);
  // SecIdx indice_secundario(secondary_index_file);
  std::cout << "=== FINDREC ===" << std::endl;
  std::cout << "Arquivo: " << csv_file_name << std::endl;
  std::vector<std::vector<Record>> hashmap(MAP_SIZE);
  int processed = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  std::ifstream csv_file(csv_file_name);

  if (!csv_file) {
    std::cerr << "Erro ao abrir arquivo csv: " << csv_file_name << std::endl;
    return EXIT_FAILURE;
  }

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
      auto cur_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(cur_time - start_time);
      std::cout << "[" << duration.count() << "s] " << processed << " registros processados" << std::endl;
    }
  }
  csv_file.close();
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
  out.close();
  return EXIT_SUCCESS;
}
