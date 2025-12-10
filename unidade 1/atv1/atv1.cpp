/*
Organização dos Processos:
    - O processo P1 cria dois processos filhos (F1 e F2).
    - Cada filho cria dois processos netos:
         F1 → N1("ls")   e N2("pwd")
         F2 → N3("date") e N4("whoami")
    - Os filhos aguardam o término de seus respectivos netos.
    - O processo P1 aguarda o término de ambos os filhos.

Marcus Vinicius da Silva Araujo, 20240008468
Emyle dos Santos Lucena, 20240007907
*/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

// Função auxiliar para executar comando no processo neto
void executarComando(const char* comando, const char* nomeNeto) {
    cout << "[" << nomeNeto << "] Executando comando: " << comando << endl;
    execl("/bin/sh", "sh", "-c", comando, (char*)nullptr);
    
    // Se chegou aqui, execl falhou
    cerr << "[" << nomeNeto << "] Falha ao executar comando!" << endl;
    exit(1);
}

int main() {
    pid_t pidF1, pidF2;     // PIDs dos processos filhos
    pid_t pidN1, pidN2;     // PIDs dos netos de F1
    pid_t pidN3, pidN4;     // PIDs dos netos de F2

    cout << "=== Iniciando programa ===" << endl;
    cout << "[P1] Processo pai iniciado. PID: " << getpid() << endl;
    cout << endl;

    // ==================== CRIAÇÃO DO FILHO F1 ====================
    pidF1 = fork();

    if (pidF1 < 0) {
        cerr << "Erro ao criar processo F1!" << endl;
        exit(1);
    }

    if (pidF1 == 0) {
        // === Processo F1 ===
        
        // Criar neto N1 (executa "ls")
        pidN1 = fork();
        if (pidN1 < 0) {
            cerr << "Erro ao criar processo N1!" << endl;
            exit(1);
        }
        if (pidN1 == 0) {
            // === Processo N1 ===
            executarComando("ls", "N1");
        }

        // Criar neto N2 (executa "pwd")
        pidN2 = fork();
        if (pidN2 < 0) {
            cerr << "Erro ao criar processo N2!" << endl;
            exit(1);
        }
        if (pidN2 == 0) {
            // === Processo N2 ===
            executarComando("pwd", "N2");
        }

        // F1 espera N1 e N2 terminarem
        wait(nullptr);
        wait(nullptr);

        // Mensagem do processo filho F1
        cout << endl;
        cout << "[F1] processo F1." << endl;
        cout << "[F1] PID: " << getpid() << endl;
        cout << "[F1] PID pai (P1): " << getppid() << endl;
        
        exit(0);
    }

    // ==================== CRIAÇÃO DO FILHO F2 ====================
    pidF2 = fork();

    if (pidF2 < 0) {
        cerr << "Erro ao criar processo F2!" << endl;
        exit(1);
    }

    if (pidF2 == 0) {
        // === Processo F2 ===
        
        // Criar neto N3 (executa "date")
        pidN3 = fork();
        if (pidN3 < 0) {
            cerr << "Erro ao criar processo N3!" << endl;
            exit(1);
        }
        if (pidN3 == 0) {
            // === Processo N3 ===
            executarComando("date", "N3");
        }

        // Criar neto N4 (executa "whoami")
        pidN4 = fork();
        if (pidN4 < 0) {
            cerr << "Erro ao criar processo N4!" << endl;
            exit(1);
        }
        if (pidN4 == 0) {
            // === Processo N4 ===
            executarComando("whoami", "N4");
        }

        // F2 espera N3 e N4 terminarem
        wait(nullptr);
        wait(nullptr);

        // Mensagem do processo filho F2
        cout << endl;
        cout << "[F2] processo F2." << endl;
        cout << "[F2] PID: " << getpid() << endl;
        cout << "[F2] PID pai (P1): " << getppid() << endl;
        
        exit(0);
    }

    // ==================== PROCESSO PAI P1 ====================
    // P1 espera F1 e F2 terminarem
    wait(nullptr);
    wait(nullptr);

    // Mensagem final do processo pai
    cout << endl;
    cout << "========================================" << endl;
    cout << "[P1] processo P1." << endl;
    cout << "[P1] PID: " << getpid() << endl;
    cout << "========================================" << endl;
    cout << "[P1] Processos filhos e netos terminados" << endl;

    return 0;
}

