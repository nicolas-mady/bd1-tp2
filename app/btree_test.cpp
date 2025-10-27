#include "b+tree.h"
#include "record.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Uso: " << argv[0] << " <comando>" << std::endl;
        std::cout << "Comandos disponíveis:" << std::endl;
        std::cout << "  load_test - Testa carregamento das árvores" << std::endl;
        std::cout << "  search_id <id> - Busca por ID" << std::endl;
        std::cout << "  search_title <titulo> - Busca por título" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "load_test")
    {
        std::cout << "=== Teste de carregamento das árvores B+ ===" << std::endl;

        // Carregar árvore de IDs
        BPlusTree<int> bptIdx1(170);
        std::cout << "Carregando índice primário (IDs)..." << std::endl;
        if (bptIdx1.loadFromFile("data/db/idx1.bin"))
        {
            std::cout << "✓ Índice primário carregado com sucesso!" << std::endl;
        }
        else
        {
            std::cerr << "✗ Erro ao carregar índice primário!" << std::endl;
            return 1;
        }

        // Carregar árvore de títulos
        BPlusTree<std::pair<std::string, int>> bptIdx2(6);
        std::cout << "Carregando índice secundário (títulos)..." << std::endl;
        if (bptIdx2.loadFromFile("data/db/idx2.bin"))
        {
            std::cout << "✓ Índice secundário carregado com sucesso!" << std::endl;
        }
        else
        {
            std::cerr << "✗ Erro ao carregar índice secundário!" << std::endl;
            return 1;
        }

        std::cout << "\nTodos os índices carregados com sucesso!" << std::endl;
        std::cout << "As árvores B+ estão prontas para uso." << std::endl;
    }
    else if (command == "show_trees")
    {
        // Mostrar conteúdo das árvores (cuidado com arquivos grandes!)
        std::cout << "=== Mostrando conteúdo das árvores ===" << std::endl;

        BPlusTree<std::pair<std::string, int>> bptIdx2(6);
        if (bptIdx2.loadFromFile("data/db/idx2.bin"))
        {
            std::cout << "Primeiras entradas do índice secundário:" << std::endl;
            bptIdx2.traverse(); // Cuidado: pode ser muito grande!
        }
    }
    else
    {
        std::cout << "Comando desconhecido: " << command << std::endl;
        return 1;
    }

    return 0;
}