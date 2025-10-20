
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>

using namespace std;

// === CONFIGURAÇÕES GLOBAIS DO ARQUIVO HASH ===
const int TAMANHO_BLOCO = 4096; // 4 KB (Simula o bloco de disco)
const int NUM_BUCKETS_PRIMARIOS = 10; // M = 10 buckets primários (Blocos de 0 a 9)
const char* NOME_ARQUIVO = "arquivo_hash.dat";
const int REGISTROS_POR_BLOCO = 106;

// === ESTRUTURAS DE DADOS ===

// Registro de Dados (apenas para exemplo)
struct Registro {
    int id;
    char titulo[30];
    int ano;
    char autores[50]; // Campo extra para aumentar o tamanho do registro

    // Garante que o registro tem um tamanho fixo (muito importante para I/O binário)
    Registro() : id(0), ano(0) {
        memset(titulo, 0, 30);
        memset(autores, 0, 50);
    }

    void set_data(int chave, const string& n, int i, const string& a) {
        id = chave;
        ano = i;
        strncpy(titulo, n.c_str(), 29);
        titulo[29] = '\0'; // Garantir terminação da string
        strncpy(autores, a.c_str(), 49);
        autores[49] = '\0'; // Garantir terminação da string
    }

    void display() const {
        if (id != 0) { // Considera 0 como registro vazio
            cout << "[ID: " << id << ", Nome: " << titulo << ", Ano: " << ano << ", Autores: " << autores << "]";
        } else {
            cout << "[VAZIO]";
        }
    }
};

// Bloco de Dados (O Bucket/Página)
struct Bloco {
    // Número máximo de registros que cabem em 4KB
    // static const int REGISTROS_POR_BLOCO = floor((double)TAMANHO_BLOCO / sizeof(Registro));

    Registro registros[REGISTROS_POR_BLOCO];
    long proximo_bloco; // Endereço de disco (offset) para o bloco de overflow (-1 = fim)

    Bloco() : proximo_bloco(-1) {
        // Inicializar todos os registros como vazios
        for (int i = 0; i < REGISTROS_POR_BLOCO; ++i) {
            registros[i] = Registro();
        }
    }
    
    // Calcula o número de bytes do bloco (deve ser igual a TAMANHO_BLOCO)
    long size() const {
        return sizeof(Registro) * REGISTROS_POR_BLOCO + sizeof(long); 
    }
};

// === LÓGICA DO HASHING E I/O EM ARQUIVO ===

class ArquivoHash {
private:
    fstream arquivo; // O arquivo binário no disco
    
    // Simula o gerenciador de espaço, rastreando onde o próximo bloco de overflow será escrito
    long offset_proximo_bloco; 

    // 1. Função de Hash
    int funcaoHash(int chave) {
        return chave % NUM_BUCKETS_PRIMARIOS;
    }

    // Função auxiliar para calcular o endereço de disco (offset)
    long calcularOffset(int indice_bucket) {
        // Calcula o deslocamento no arquivo (Bloco 0 começa em 0, Bloco 1 em 4096, etc.)
        return (long)indice_bucket * TAMANHO_BLOCO;
    }

    // Lê um bloco do disco no offset especificado
    bool lerBloco(long offset, Bloco& b) {
        arquivo.seekg(offset, ios::beg);
        arquivo.read(reinterpret_cast<char*>(&b), TAMANHO_BLOCO);
        return arquivo.good();
    }

    // Escreve um bloco no disco no offset especificado
    void escreverBloco(long offset, const Bloco& b) {
        arquivo.seekp(offset, ios::beg);
        arquivo.write(reinterpret_cast<const char*>(&b), TAMANHO_BLOCO);
        arquivo.flush();
    }

public:
    ArquivoHash() {
        // Abrir ou criar o arquivo binário
        arquivo.open(NOME_ARQUIVO, ios::in | ios::out | ios::binary);
        
        if (!arquivo.is_open()) {
            // Se não conseguiu abrir, tenta criar
            arquivo.open(NOME_ARQUIVO, ios::out | ios::binary);
            arquivo.close();
            arquivo.open(NOME_ARQUIVO, ios::in | ios::out | ios::binary);
            
            // Inicializar os buckets primários vazios e calcular o offset inicial
            Bloco bloco_vazio;
            for (int i = 0; i < NUM_BUCKETS_PRIMARIOS; ++i) {
                escreverBloco(calcularOffset(i), bloco_vazio);
            }
            // O primeiro bloco de overflow começará após o último bucket primário
            offset_proximo_bloco = calcularOffset(NUM_BUCKETS_PRIMARIOS);
        } else {
            // Se o arquivo já existe, calcula o offset do final para overflows
            arquivo.seekg(0, ios::end);
            offset_proximo_bloco = arquivo.tellg();
        }

        cout << "Tamanho de Registro: " << sizeof(Registro) << " bytes." << endl;
        cout << "Registros por Bloco: " << REGISTROS_POR_BLOCO << "." << endl;
        cout << "Arquivo Hash pronto. " << NUM_BUCKETS_PRIMARIOS << " buckets iniciais." << endl;
    }

    ~ArquivoHash() {
        if (arquivo.is_open()) {
            arquivo.close();
        }
    }

    // 2. Inserção de Registro (Lida com Colisões/Overflow)
    void inserir(int chave, const string& titulo, int ano, const string& autores) {
        Registro novo_registro;
        novo_registro.set_data(chave, titulo, ano, autores);

        int indice = funcaoHash(chave);
        long offset_atual = calcularOffset(indice);
        
        Bloco bloco_atual;
        long offset_anterior = -1; // Usado para atualizar o ponteiro de overflow

        // Loop para encontrar um espaço vazio (inclui blocos de overflow)
        while (offset_atual != -1) {
            lerBloco(offset_atual, bloco_atual);

            // 1. Procurar espaço vazio no bloco atual
            for (int i = 0; i < REGISTROS_POR_BLOCO; ++i) {
                if (bloco_atual.registros[i].id == 0) { // Encontrou espaço
                    bloco_atual.registros[i] = novo_registro;
                    escreverBloco(offset_atual, bloco_atual);
                    cout << "  > INSERIDO chave " << chave << " no Bucket " << indice << " (Offset: " << offset_atual << ")" << endl;
                    return;
                }
            }

            // 2. O bloco atual está cheio. Avançar para o bloco de overflow (se houver)
            offset_anterior = offset_atual;
            offset_atual = bloco_atual.proximo_bloco;
        }

        // 3. Colisão e Overflow: Todos os blocos estão cheios. Criar um novo bloco de overflow.
        Bloco novo_overflow_bloco;
        novo_overflow_bloco.registros[0] = novo_registro;
        
        // Escrever o novo bloco de overflow no final do arquivo
        long novo_offset_overflow = offset_proximo_bloco;
        escreverBloco(novo_offset_overflow, novo_overflow_bloco);
        
        // Atualizar o ponteiro de 'proximo_bloco' do último bloco encadeado
        Bloco ultimo_bloco;
        lerBloco(offset_anterior, ultimo_bloco);
        ultimo_bloco.proximo_bloco = novo_offset_overflow;
        escreverBloco(offset_anterior, ultimo_bloco);
        
        // Atualiza o rastreador de posição do próximo bloco de overflow
        offset_proximo_bloco += TAMANHO_BLOCO; 

        cout << "  > COLISÃO! Chave " << chave << " em novo bloco de OVERFLOW (Offset: " << novo_offset_overflow << ")" << endl;
    }

    // 3. Busca de Registro
    bool buscar(int chave) {
        int indice = funcaoHash(chave);
        long offset_atual = calcularOffset(indice);
        
        Bloco bloco_atual;
        int blocos_lidos = 0; // Contador de I/O

        while (offset_atual != -1) {
            lerBloco(offset_atual, bloco_atual);
            blocos_lidos++;

            for (int i = 0; i < REGISTROS_POR_BLOCO; ++i) {
                if (bloco_atual.registros[i].id == chave) {
                    cout << "ENCONTRADO: Chave " << chave << " no Bucket " << indice 
                         << ". (I/O: " << blocos_lidos << " blocos lidos)" << endl;
                    bloco_atual.registros[i].display();
                    cout << endl;
                    return true;
                }
            }
            // Mover para o próximo bloco (se o registro não foi encontrado)
            offset_atual = bloco_atual.proximo_bloco;
        }

        cout << "NÃO ENCONTRADO: Chave " << chave << ". (I/O: " << blocos_lidos << " blocos lidos)" << endl;
        return false;
    }
};

int main() {
    // 1. Início e Configuração
    // Nota: O arquivo binário é mantido entre execuções. Para resetar, exclua "arquivo_hash.dat".
    ArquivoHash db;

    // 2. Preenchendo o primeiro bucket (índice 3) para forçar colisão
    cout << "\n--- Preenchendo Bucket Primário 3 ---\n";
    // O bloco tem capacidade para 80 registros. Vamos usar 3 para simplificar o teste.
    // (Ajuste o loop se necessário, dependendo da capacidade real do Bloco).

    int MAX_REG = REGISTROS_POR_BLOCO;
    
    // Inserir registros que mapeiam para o Bucket 3 (3 % 10 = 3)
    cout << "Capacidade do Bucket 3: " << MAX_REG << " registros.\n";
    
    // Preenche o Bucket 3
    for (int i = 0; i < MAX_REG; ++i) {
        int chave = 3 + i * 10; // Ex: 3, 13, 23, 33...
        db.inserir(chave, "Reg" + to_string(chave), 20 + i, "Autor_" + to_string(i));
    }
    
    // 3. Forçar Colisão e OVERFLOW
    cout << "\n--- Forçando OVERFLOW ---\n";
    int chave_overflow = 3 + MAX_REG * 10; // Próxima chave que mapeia para 3
    db.inserir(chave_overflow, "OverflowReg", 50, "Autor_Overflow");

    // 4. Inserir em outro bucket
    db.inserir(1, "Bucket1_A", 18, "Autor_A");
    db.inserir(11, "Bucket1_B", 19, "Autor_B"); // Colisão no Bucket 1, mas ainda no mesmo bloco

    // 5. Busca
    cout << "\n--- Realizando Buscas ---\n";
    db.buscar(1); // Busca rápida: 1 bloco lido (Bucket 1)
    db.buscar(chave_overflow); // Busca lenta: 2 blocos lidos (Bucket 3 Primário + Overflow)
    db.buscar(13); // Busca rápida: 1 bloco lido (Bucket 3 Primário)
    db.buscar(93); // Não encontrado

    return 0;
}

// Nota sobre TAMANHO_BLOCO e REGISTROS_POR_BLOCO:
// Se TAMANHO_BLOCO for 4096 e sizeof(Registro) for 38, REGISTROS_POR_BLOCO = 107.
// O restante é o ponteiro 'long proximo_bloco' e o espaço não utilizado.