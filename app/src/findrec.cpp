#include <bits/stdc++.h>
#include <record.h>

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
    std::cerr << "erro: id inválido: " << argv[1] << std::endl;
    return 1;
  }

  std::string hash_path = "data/db/hash.bin";

  std::cout << "=== findrec " << id << " ===" << std::endl;
  std::cout << "buscando em " << hash_path << std::endl;

  auto t0 = std::chrono::high_resolution_clock::now();

  std::ifstream in(hash_path, std::ios::binary);

  if (!in) {
    std::cerr << "erro: não foi possível abrir " << hash_path << std::endl;
    return 1;
  }

  long offset = (id % MAP_SIZE) * sizeof(Record) * 2;
  in.seekg(offset, std::ios::beg);

  Record rec;
  for (int tries = 0; rec.id != id && tries < 2; tries++)
    in.read(reinterpret_cast<char*>(&rec), sizeof(Record));

  in.close();

  auto t1 = std::chrono::high_resolution_clock::now();
  auto t = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

  std::cout << " [" << t.count() << " µs]" << " 1 bloco lido" << std::endl;

  if (rec.id == id)
    rec.print();
  else
    std::cout << "registro não encontrado" << std::endl;

  return rec.id != id;
}
