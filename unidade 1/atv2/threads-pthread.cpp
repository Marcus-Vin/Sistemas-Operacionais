/*
Este programa cria um vetor com 10.000 números aleatórios e realiza
o cálculo de três estatísticas básicas: média aritmética, mediana e 
desvio padrão. A implementação utiliza três threads POSIX (pthread) 
executando de forma concorrente, onde cada thread é responsável por 
uma das operações estatísticas. O propósito é avaliar o desempenho 
comparativo com as versões single-thread e com múltiplos processos 
via fork(). Além disso, são medidos tanto o tempo de criação das 
threads quanto o tempo total de processamento.

Marcus Vinicius da Silva Araujo, 20240008468
Emyle dos Santos Lucena, 20240007907
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <pthread.h>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

// Variáveis globais para armazenar resultados
double media = 0;
double mediana = 0;
double desvioPadrao = 0;

// Quantidade fixa de entradas
const int nEntradas = 10000;
vector<int> valores(nEntradas);

// Função que calcula a média aritmética
// Assinatura padrão de pthreads: void* func(void* arg)
void* calc_media(void* arg) {
    long long soma = 0;

    // Soma todos os valores do vetor
    for (int i = 0; i < nEntradas; i++)
        soma += valores[i];

    // Média aritmética simples
    media = (double) soma / valores.size();

    return NULL;
}

// Função que calcula a mediana
void* calc_mediana(void* arg) {
    // Cria cópia local para realizar ordenação
    vector<int> v = valores;

    std::sort(v.begin(), v.end());

    // Cálculo da mediana depende da paridade do tamanho
    if (nEntradas % 2 == 0)
        mediana = (v[nEntradas/2] + v[nEntradas/2 - 1]) / 2.0;
    else
        mediana = v[nEntradas/2];

    return NULL;
}

// Função que calcula o desvio padrão populacional
void* calc_desvio(void* arg) {
    // Calcula a média localmente para evitar condição de corrida
    long long soma = 0;
    for (int i = 0; i < nEntradas; i++)
        soma += valores[i];
    double media_local = (double) soma / nEntradas;

    // Calcula o desvio padrão usando a média local
    double acumulado = 0;
    for (int i = 0; i < nEntradas; i++)
        acumulado += (valores[i] - media_local) * (valores[i] - media_local);

    desvioPadrao = sqrt(acumulado / nEntradas);

    return NULL;
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

    cout << "Exemplo de execucao com 3 threads (POSIX pthreads)" << endl;

    // Declaração dos identificadores das threads
    pthread_t t1, t2, t3;

    // Marca início do tempo de execução geral
    auto inicio = std::chrono::high_resolution_clock::now();

    // Criação das três threads usando pthread_create
    // Assinatura: pthread_create(pthread_t*, attr*, função*, arg*)
    auto t_inicio = std::chrono::high_resolution_clock::now();
    
    int rc1 = pthread_create(&t1, NULL, calc_media, NULL);
    int rc2 = pthread_create(&t2, NULL, calc_mediana, NULL);
    int rc3 = pthread_create(&t3, NULL, calc_desvio, NULL);
    
    auto t_fim = std::chrono::high_resolution_clock::now();

    // Verifica se as threads foram criadas com sucesso
    if (rc1 != 0 || rc2 != 0 || rc3 != 0) {
        cout << "Erro ao criar threads!" << endl;
        return 1;
    }

    // Espera todas as threads terminarem usando pthread_join
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

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
    cout << "Tempo de criacao das threads: " << tempo_criacao << " ms" << endl;

    return 0;
}

