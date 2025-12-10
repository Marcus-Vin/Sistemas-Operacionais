/*
Este programa cria um vetor com 10.000 números aleatórios e realiza
o cálculo de três estatísticas básicas: média aritmética, mediana e 
desvio padrão. A implementação utiliza um único processo, executando
todos os cálculos de forma sequencial. O propósito é servir como 
baseline de comparação com as versões que utilizam múltiplos processos
(fork) ou múltiplas threads. O programa mede o tempo total de 
processamento para análise de desempenho.

Marcus Vinicius da Silva Araujo, 20240008468
Emyle dos Santos Lucena, 20240007907
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

using std::cout;
using std::endl;
using std::vector;

// Quantidade fixa de entradas
const int nEntradas = 10000;
vector<int> valores(nEntradas);

// Função que calcula a média aritmética
double calc_media() {
    long long soma = 0;

    // Soma todos os valores do vetor
    for (int i = 0; i < nEntradas; i++)
        soma += valores[i];

    // Média aritmética simples
    return (double) soma / nEntradas;
}

// Função que calcula a mediana
double calc_mediana() {
    // Cria cópia local para realizar ordenação
    vector<int> v = valores;

    std::sort(v.begin(), v.end());

    // Cálculo da mediana depende da paridade do tamanho
    if (nEntradas % 2 == 0)
        return (v[nEntradas/2] + v[nEntradas/2 - 1]) / 2.0;
    else
        return v[nEntradas/2];
}

// Função que calcula o desvio padrão populacional
double calc_desvio(double media) {
    // Calcula o desvio padrão usando a média recebida
    double acumulado = 0;
    for (int i = 0; i < nEntradas; i++)
        acumulado += (valores[i] - media) * (valores[i] - media);

    return sqrt(acumulado / nEntradas);
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

    cout << "Exemplo de execucao com 1 processo (single-process)" << endl;

    // Marca início do tempo de execução geral
    auto inicio = std::chrono::high_resolution_clock::now();

    // Executa os cálculos sequencialmente
    double media = calc_media();
    double mediana = calc_mediana();
    double desvioPadrao = calc_desvio(media);

    // Marca fim da execução
    auto fim = std::chrono::high_resolution_clock::now();

    // Cálculo do tempo
    double tempo_exec = std::chrono::duration<double, std::milli>(fim - inicio).count();

    // Exibe resultados
    cout << "Media: " << media << endl;
    cout << "Mediana: " << mediana << endl;
    cout << "Desvio padrao: " << desvioPadrao << endl;

    cout << "--- Metricas de Tempo ---" << endl;
    cout << "Tempo total: " << tempo_exec << " ms" << endl;

    return 0;
}
