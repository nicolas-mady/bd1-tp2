#include "b+tree.h"
#include "record.h"
#include <bits/stdc++.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " \"<Título>\"" << std::endl;
    return 1;
  }

  std::string titulo = argv[1];
  std::string hash_path = "data/db/hash.bin";
  std::string idx2_path = "data/db/idx2.bin";

  std::cout << "=== seek2 " << titulo << " ===" << std::endl;
  std::cout << "Buscando em " << idx2_path << std::endl;

  BPlusTree<std::pair<std::string, int>> bptree(6);

  auto t0 = std::chrono::high_resolution_clock::now();

  if (!bptree.loadFromFile(idx2_path)) {
    std::cerr << "Erro: não foi possível carregar o índice secundário" << std::endl;
    return 1;
  }

  auto results = bptree.searchByPrefix(titulo);

  if (results.empty())
    results = bptree.searchBySubstring(titulo);

  auto t1 = std::chrono::high_resolution_clock::now();
  auto t = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

  std::cout << " [" << t.count() << " ms] " << bptree.getLoadedNodesCount() << " blocos lidos" << std::endl;

  bool found = !results.empty();

  if (found) {
    std::ifstream in(hash_path, std::ios::binary);

    if (!in) {
      std::cerr << "Erro: não foi possível abrir " << hash_path << std::endl;
      return 1;
    }

    for (size_t i = 0; i < results.size(); i++) {
      int id = results[i].second;
      long offset = (id % MAP_SIZE) * sizeof(Record) * 2;
      in.seekg(offset, std::ios::beg);

      Record rec;
      for (int tries = 0; rec.id != id && tries < 2; tries++)
        in.read(reinterpret_cast<char*>(&rec), sizeof(Record));

      rec.print();
    }
    in.close();
    return 0;
  } else {
    std::cout << "nenhum registro encontrado" << std::endl;
    return 1;
  }
}
