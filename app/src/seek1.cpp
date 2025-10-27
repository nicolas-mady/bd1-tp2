#include "b+tree.h"
#include "record.h"
#include <bits/stdc++.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <ID>" << std::endl;
    return 1;
  }

  int id;
  try {
    id = std::stoi(argv[1]);
    if (id < 1)
      throw std::exception();
  } catch (const std::exception& e) {
    std::cerr << "Erro: id inválido: " << argv[1] << std::endl;
    return 1;
  }

  std::string hash_path = "data/db/hash.bin";
  std::string idx1_path = "data/db/idx1.bin";

  std::cout << "=== seek1 " << id << " ===" << std::endl;
  std::cout << "Buscando em " << idx1_path << std::endl;

  BPlusTree<int> bptree(170);

  auto t0 = std::chrono::high_resolution_clock::now();

  if (!bptree.loadFromFile(idx1_path)) {
    std::cerr << "Erro: não foi possível carregar o índice primário" << std::endl;
    return 1;
  }

  auto result = bptree.search(id);

  auto t1 = std::chrono::high_resolution_clock::now();
  auto t = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  std::cout << " [" << t.count() << " ms] " << bptree.getLoadedNodesCount() << " blocos lidos" << std::endl;

  if (result != nullptr) {
    std::ifstream in(hash_path, std::ios::binary);

    if (!in) {
      std::cerr << "Erro: não foi possível abrir " << hash_path << std::endl;
      return 1;
    }

    long offset = (id % MAP_SIZE) * sizeof(Record) * 2;
    in.seekg(offset, std::ios::beg);

    Record rec;
    for (int tries = 0; rec.id != id && tries < 2; tries++)
      in.read(reinterpret_cast<char*>(&rec), sizeof(Record));

    in.close();

    rec.print();
    return 0;
  } else {
    std::cout << "registro não encontrado" << std::endl;
    return 1;
  }
}
