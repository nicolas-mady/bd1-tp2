#include <bits/stdc++.h>
#include <record.hpp>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "uso: " << argv[0] << " <id>" << std::endl;
    return 1;
  }

  int id;
  try {
    id = std::stoi(argv[1]);
    if (id < 1)
      throw std::exception();
  } catch (const std::exception& e) {
    std::cerr << "id inválido: " << argv[1] << std::endl;
    return 1;
  }

  std::string hash_file_name = "data/db/hash.dat";

  std::cout << "=== findrec " << id << " ===" << std::endl;
  std::cout << "buscando em " << hash_file_name << std::endl;

  auto t0 = std::chrono::high_resolution_clock::now();

  std::ifstream in(hash_file_name, std::ios::binary);

  long offset = (id % MAP_SIZE) * sizeof(Record) * 2;
  in.seekg(offset, std::ios::beg);

  Record rec;
  int tries = 0;

  do {
    in.read(reinterpret_cast<char*>(&rec), sizeof(Record));
    tries++;
  } while (rec.id != id && tries < 2);

  in.close();
  
  auto t1 = std::chrono::high_resolution_clock::now();
  auto t = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  std::cout << " [" << t.count() << "ms]" << " 1 bloco lido" << std::endl;

  if (rec.id == id)
    rec.print();
  else
    std::cout << "registro não encontrado" << std::endl;

  return rec.id != id;
}
