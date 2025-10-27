#include "b+tree.h"
#include <iostream>

int main()
{
    std::cout << "=== Teste básico das árvores B+ ===" << std::endl;

    // Testar árvore de inteiros
    BPlusTree<int> btree1(3);
    btree1.insert(10);
    btree1.insert(20);
    btree1.insert(5);
    btree1.insert(15);

    std::cout << "Árvore de inteiros criada e populada." << std::endl;
    std::cout << "Salvando em test_int.bin..." << std::endl;

    if (btree1.saveToFile("test_int.bin"))
    {
        std::cout << "Salvo com sucesso!" << std::endl;

        // Testar carregamento
        BPlusTree<int> btree1_loaded(3);
        if (btree1_loaded.loadFromFile("test_int.bin"))
        {
            std::cout << "Carregado com sucesso!" << std::endl;
            std::cout << "Conteúdo carregado:" << std::endl;
            btree1_loaded.traverse();
        }
    }

    // Testar árvore de pares
    BPlusTree<std::pair<std::string, int>> btree2(2);
    btree2.insert({"apple", 1});
    btree2.insert({"banana", 2});
    btree2.insert({"cherry", 3});

    std::cout << "\nÁrvore de pares criada e populada." << std::endl;
    std::cout << "Salvando em test_pair.bin..." << std::endl;

    if (btree2.saveToFile("test_pair.bin"))
    {
        std::cout << "Salvo com sucesso!" << std::endl;

        // Testar carregamento
        BPlusTree<std::pair<std::string, int>> btree2_loaded(2);
        if (btree2_loaded.loadFromFile("test_pair.bin"))
        {
            std::cout << "Carregado com sucesso!" << std::endl;
            std::cout << "Conteúdo carregado:" << std::endl;
            btree2_loaded.traverse();
        }
    }

    return 0;
}