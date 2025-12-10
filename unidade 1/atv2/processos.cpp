/*
Este programa cria um vetor com 10.000 números aleatórios e realiza
o cálculo de três estatísticas básicas: média aritmética, mediana e 
desvio padrão. A implementação utiliza três processos filhos criados
com fork(), onde cada processo é responsável por uma das operações 
estatísticas. A comunicação entre processos é feita através de pipes,
permitindo que os resultados sejam enviados ao processo pai. O propósito 
é avaliar o desempenho comparativo com as versões single-thread e com 
múltiplas threads. Além disso, são medidos tanto o tempo de criação dos 
processos quanto o tempo total de processamento.

Marcus Vinicius da Silva Araujo, 20240008468
Emyle dos Santos Lucena, 20240007907
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <unistd.h>
#include <sys/wait.h>

using std::cout;
using std::endl;
using std::vector;

// Quantidade fixa de entradas
const int nEntradas = 10000;
vector<int> valores(nEntradas);

// Identificadores dos tipos de resultados
enum ResultType { MEDIA, MEDIANA, DESVIO_PADRAO };

// Função que calcula a média aritmética
void calc_media(int write_fd) {
    long long soma = 0;

    // Soma todos os valores do vetor
    for (int i = 0; i < nEntradas; i++)
        soma += valores[i];

    // Média aritmética simples
    double media = (double) soma / nEntradas;

    // Escreve o tipo de resultado e o valor no pipe
    int type = MEDIA;
    write(write_fd, &type, sizeof(int));
    write(write_fd, &media, sizeof(double));

    _exit(0);
}

// Função que calcula a mediana
void calc_mediana(int write_fd) {
    // Cria cópia local para realizar ordenação
    vector<int> v = valores;

    std::sort(v.begin(), v.end());

    // Cálculo da mediana depende da paridade do tamanho
    double mediana;
    if (nEntradas % 2 == 0)
        mediana = (v[nEntradas/2] + v[nEntradas/2 - 1]) / 2.0;
    else
        mediana = v[nEntradas/2];

    // Escreve o tipo de resultado e o valor no pipe
    int type = MEDIANA;
    write(write_fd, &type, sizeof(int));
    write(write_fd, &mediana, sizeof(double));

    _exit(0);
}

// Função que calcula o desvio padrão populacional
void calc_desvio(int write_fd) {
    // Calcula a média localmente
    long long soma = 0;
    for (int i = 0; i < nEntradas; i++)
        soma += valores[i];
    double media_local = (double) soma / nEntradas;

    // Calcula o desvio padrão usando a média local
    double acumulado = 0;
    for (int i = 0; i < nEntradas; i++)
        acumulado += (valores[i] - media_local) * (valores[i] - media_local);

    double desvioPadrao = sqrt(acumulado / nEntradas);

    // Escreve o tipo de resultado e o valor no pipe
    int type = DESVIO_PADRAO;
    write(write_fd, &type, sizeof(int));
    write(write_fd, &desvioPadrao, sizeof(double));

    _exit(0);
}

// Função main
int main() {

    // Gerador de números aleatórios entre 0 e 100
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 100);

    // Preenche o vetor com valores aleatórios
    for (int i = 0; i < nEntradas; i++)
        valores[i] = dist(gen);

    cout << "Exemplo de execucao com 3 processos (fork/pipe)" << endl;

    // Criação do pipe (pipe_fds[0] = leitura, pipe_fds[1] = escrita)
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        perror("Erro ao criar pipe");
        return 1;
    }

    // Declaração dos PIDs dos processos filhos
    pid_t pids[3];
    int num_processos = 3;

    // Marca início do tempo de execução geral
    auto inicio = std::chrono::high_resolution_clock::now();

    // Criação dos três processos usando fork
    auto t_inicio = std::chrono::high_resolution_clock::now();

    // Processo 1: Média
    pids[0] = fork();
    if (pids[0] == 0) {
        close(pipe_fds[0]);
        calc_media(pipe_fds[1]);
    } else if (pids[0] < 0) {
        perror("Erro ao criar processo para media");
        return 1;
    }

    // Processo 2: Mediana
    pids[1] = fork();
    if (pids[1] == 0) {
        close(pipe_fds[0]);
        calc_mediana(pipe_fds[1]);
    } else if (pids[1] < 0) {
        perror("Erro ao criar processo para mediana");
        return 1;
    }

    // Processo 3: Desvio Padrão
    pids[2] = fork();
    if (pids[2] == 0) {
        close(pipe_fds[0]);
        calc_desvio(pipe_fds[1]);
    } else if (pids[2] < 0) {
        perror("Erro ao criar processo para desvio padrao");
        return 1;
    }

    auto t_fim = std::chrono::high_resolution_clock::now();

    // Pai fecha lado de escrita do pipe
    close(pipe_fds[1]);

    // Variáveis para armazenar os resultados
    double media = 0;
    double mediana = 0;
    double desvioPadrao = 0;

    // Lê do pipe todos os resultados enviados pelos filhos
    int valoresRecebidos = 0;
    while (valoresRecebidos < num_processos) {
        int type;
        double value;

        if (read(pipe_fds[0], &type, sizeof(int)) > 0) {
            if (read(pipe_fds[0], &value, sizeof(double)) > 0) {
                switch (type) {
                    case MEDIA:
                        media = value;
                        break;
                    case MEDIANA:
                        mediana = value;
                        break;
                    case DESVIO_PADRAO:
                        desvioPadrao = value;
                        break;
                }
                valoresRecebidos++;
            }
        }
    }

    // Espera todos os processos filhos terminarem
    for (int i = 0; i < num_processos; i++)
        waitpid(pids[i], nullptr, 0);

    // Marca fim da execução
    auto fim = std::chrono::high_resolution_clock::now();

    // Cálculo dos tempos
    double tempo_exec = std::chrono::duration<double, std::milli>(fim - inicio).count();
    double tempo_criacao = std::chrono::duration<double, std::milli>(t_fim - t_inicio).count();

    // Exibe resultados
    cout << "Media: " << media << endl;
    cout << "Mediana: " << mediana << endl;
    cout << "Desvio padrao: " << desvioPadrao << endl;

    cout << "--- Metricas de Tempo ---" << endl;
    cout << "Tempo total: " << tempo_exec << " ms" << endl;
    cout << "Tempo de criacao dos processos: " << tempo_criacao << " ms" << endl;

    return 0;
}
