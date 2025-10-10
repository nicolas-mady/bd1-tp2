#include "btree.h"
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <algorithm>

template <typename T>
BPlusTree<T>::BPlusTree(const std::string &filename)
    : filename(filename), root_position(-1)
{
    node_size = sizeof(BTreeNode<T>);
}

template <typename T>
BPlusTree<T>::~BPlusTree()
{
    // Destrutor
}

template <typename T>
bool BPlusTree<T>::initialize()
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Erro ao criar arquivo B+Tree: " << filename << std::endl;
        return false;
    }

    // Cria nó raiz inicial
    BTreeNode<T> root;
    root.is_leaf = true;
    root.num_keys = 0;

    root_position = 0;
    file.write(reinterpret_cast<const char *>(&root), node_size);
    file.close();

    std::cout << "B+Tree inicializada: " << filename << std::endl;
    return true;
}

template <typename T>
bool BPlusTree<T>::insert(const T &entry)
{
    if (root_position == -1)
    {
        if (!initialize())
            return false;
    }

    return insertIntoNode(root_position, entry);
}

template <typename T>
BuscaEstatisticas BPlusTree<T>::search(const T &key, T &result)
{
    BuscaEstatisticas stats;
    stats.total_blocos = getTotalBlocks();

    if (root_position == -1)
    {
        return stats;
    }

    return searchInNode(root_position, key, result, stats.blocos_lidos);
}

template <typename T>
int BPlusTree<T>::getTotalBlocks()
{
    struct stat st;
    if (stat(filename.c_str(), &st) != 0)
    {
        return 0;
    }
    return (st.st_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

template <typename T>
bool BPlusTree<T>::readNode(long position, BTreeNode<T> &node)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return false;

    file.seekg(position);
    file.read(reinterpret_cast<char *>(&node), node_size);
    file.close();

    return file.gcount() == node_size;
}

template <typename T>
bool BPlusTree<T>::writeNode(long position, const BTreeNode<T> &node)
{
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file)
        return false;

    file.seekp(position);
    file.write(reinterpret_cast<const char *>(&node), node_size);
    file.close();

    return true;
}

template <typename T>
long BPlusTree<T>::allocateNode()
{
    struct stat st;
    if (stat(filename.c_str(), &st) != 0)
    {
        return 0;
    }
    return st.st_size;
}

template <typename T>
bool BPlusTree<T>::insertIntoNode(long node_pos, const T &entry, long child_pos)
{
    BTreeNode<T> node;
    if (!readNode(node_pos, node))
    {
        return false;
    }

    // Implementação simplificada - apenas adiciona ao final se há espaço
    if (node.num_keys < BTREE_ORDER - 1)
    {
        node.keys[node.num_keys] = entry;
        if (!node.is_leaf && child_pos != -1)
        {
            node.children[node.num_keys + 1] = child_pos;
        }
        node.num_keys++;

        return writeNode(node_pos, node);
    }

    // Se não há espaço, seria necessário implementar split
    // Por simplicidade, apenas reporta erro
    std::cerr << "Nó cheio - split não implementado" << std::endl;
    return false;
}

template <typename T>
BuscaEstatisticas BPlusTree<T>::searchInNode(long node_pos, const T &key, T &result, int &blocos_lidos)
{
    BuscaEstatisticas stats;
    blocos_lidos++;

    BTreeNode<T> node;
    if (!readNode(node_pos, node))
    {
        return stats;
    }

    // Busca linear no nó (implementação simplificada)
    for (int i = 0; i < node.num_keys; i++)
    {
        if (matches(node.keys[i], key))
        {
            result = node.keys[i];
            stats.encontrado = true;
            return stats;
        }
    }

    // Se não é folha, continua busca nos filhos
    if (!node.is_leaf)
    {
        // Implementação simplificada - busca no primeiro filho
        if (node.children[0] != -1)
        {
            return searchInNode(node.children[0], key, result, blocos_lidos);
        }
    }

    return stats;
}

// Especializações das funções de comparação

template <>
int BPlusTree<IndicePrimarioEntry>::compare(const IndicePrimarioEntry &a, const IndicePrimarioEntry &b)
{
    return a.id - b.id;
}

template <>
bool BPlusTree<IndicePrimarioEntry>::matches(const IndicePrimarioEntry &entry, const IndicePrimarioEntry &key)
{
    return entry.id == key.id;
}

template <>
int BPlusTree<IndiceSecundarioEntry>::compare(const IndiceSecundarioEntry &a, const IndiceSecundarioEntry &b)
{
    return strcmp(a.titulo, b.titulo);
}

template <>
bool BPlusTree<IndiceSecundarioEntry>::matches(const IndiceSecundarioEntry &entry, const IndiceSecundarioEntry &key)
{
    return strcmp(entry.titulo, key.titulo) == 0;
}

// Instanciações explícitas
template class BPlusTree<IndicePrimarioEntry>;
template class BPlusTree<IndiceSecundarioEntry>;