#include "b+tree.h"
#include "record.h"
#include <iostream>

int main()
{
    std::cout << "=== Teste de carregamento das árvores B+ ===" << std::endl;

    // Testar carregamento da árvore de inteiros
    BPlusTree<int> bptIdx1(170);
    std::cout << "Carregando índice primário..." << std::endl;
    if (bptIdx1.loadFromFile("data/db/idx1.bin"))
    {
        std::cout << "Índice primário carregado com sucesso!" << std::endl;

        // Testar algumas buscas
        std::cout << "Testando busca por 100000000..." << std::endl;
        if (bptIdx1.search(100000000) != nullptr)
        {
            std::cout << "Encontrado!" << std::endl;
        }
        else
        {
            std::cout << "Não encontrado!" << std::endl;
        }
    }
    else
    {
        std::cerr << "Erro ao carregar índice primário!" << std::endl;
    }

    // Testar carregamento da árvore de pares
    BPlusTree<std::pair<std::string, int>> bptIdx2(6);
    std::cout << "\nCarregando índice secundário..." << std::endl;
    if (bptIdx2.loadFromFile("data/db/idx2.bin"))
    {
        std::cout << "Índice secundário carregado com sucesso!" << std::endl;

        // Mostrar algumas entradas
        std::cout << "Primeiras entradas do índice secundário:" << std::endl;
        // bptIdx2.traverse(); // Descomente para ver todas as entradas
    }
    else
    {
        std::cerr << "Erro ao carregar índice secundário!" << std::endl;
    }

    return 0;
}