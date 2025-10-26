#include <iostream>
#include <fstream>
#include <cstring> // Para std::memset

// Define o struct (deve ser o mesmo usado para escrever o arquivo!)
struct Aluno {
    int id;
    char nome[20];
    float nota;
};

void ler_registro_por_indice(const char* nome_arquivo, int indice_registro) {
    // Abrir no modo de leitura (in) e BINÁRIO (binary)
    std::ifstream arquivo(nome_arquivo, std::ios::in | std::ios::binary);

    if (!arquivo.is_open()) {
        std::cerr << "ERRO: Não foi possível abrir o arquivo '" << nome_arquivo << "'" << std::endl;
        return;
    }

    Aluno aluno;
    
    // 1. Calcular o OFFSET (posição) do registro
    // Offset = Índice * Tamanho do Struct
    long offset = indice_registro * sizeof(Aluno);

    // 2. Mover o ponteiro de leitura para o offset calculado
    // ios::beg indica que o offset é calculado a partir do INÍCIO do arquivo.
    arquivo.seekg(offset, std::ios::beg);

    // *Importante*: Limpe o struct antes de ler para evitar lixo, 
    // especialmente se você estiver lendo membro por membro.
    // std::memset(&aluno, 0, sizeof(Aluno));

    // 3. Ler o bloco de bytes do registro inteiro
    // (char*): Converte o endereço da struct para um ponteiro de byte.
    // sizeof(Aluno): Especifica quantos bytes ler.
    arquivo.read(reinterpret_cast<char*>(&aluno), sizeof(Aluno));

    // 4. Verificar o status da leitura
    if (arquivo.fail()) {
        std::cerr << "ERRO: Falha ao ler o registro (índice fora do limite ou erro de arquivo)." << std::endl;
    } else {
        std::cout << "\n--- Registro lido (Índice: " << indice_registro << ") ---" << std::endl;
        std::cout << "ID: " << aluno.id << std::endl;
        std::cout << "Nome: " << aluno.nome << std::endl;
        std::cout << "Nota: " << aluno.nota << std::endl;
    }

    arquivo.close();
}

// ** FUNÇÃO DE ESCRITA (Para criar o arquivo de exemplo) **
void escrever_arquivo_binario(const char* nome_arquivo) {
    std::ofstream arquivo(nome_arquivo, std::ios::out | std::ios::binary | std::ios::trunc);

    Aluno a1 = {101, "Alice", 9.5f};
    Aluno a2 = {102, "Bob", 7.8f};
    Aluno a3 = {103, "Charlie", 6.2f};

    arquivo.write(reinterpret_cast<char*>(&a1), sizeof(Aluno));
    arquivo.write(reinterpret_cast<char*>(&a2), sizeof(Aluno));
    arquivo.write(reinterpret_cast<char*>(&a3), sizeof(Aluno));

    arquivo.close();
    std::cout << "Arquivo binário criado com 3 registros de Aluno." << std::endl;
}
// ** FIM DA FUNÇÃO DE ESCRITA **

int main() {
    const char* NOME_ARQUIVO = "alunos.dat";
    
    // 1. Cria o arquivo para ter dados para ler
    escrever_arquivo_binario(NOME_ARQUIVO); 
    
    // 2. Lê o registro no índice 1 (o segundo registro: Bob)
    ler_registro_por_indice(NOME_ARQUIVO, 1); 
    
    // 3. Lê o registro no índice 0 (o primeiro registro: Alice)
    ler_registro_por_indice(NOME_ARQUIVO, 0); 

    return 0;
}