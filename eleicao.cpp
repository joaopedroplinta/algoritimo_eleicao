#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <csignal>
#include <unistd.h>

class Processo {
public:
    int pid;
    bool ativo;

    Processo(int id) : pid(id), ativo(true) {}
};

class MulticastGroup {
public:
    void enviarMensagem(const std::string& mensagem) {
        static std::mutex printMutex;
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << mensagem << std::endl;
    }
};

class SistemaDistribuido {
private:
    std::vector<Processo> processos;
    MulticastGroup grupo;
    std::atomic<int> coordenador;
    std::mutex mtx;
    std::atomic<bool> coordenadorVivo;
    std::atomic<bool> sistemaAtivo;
    std::thread coordenadorThread; // Thread para o coordenador atual

public:
    SistemaDistribuido() : coordenador(-1), coordenadorVivo(true), sistemaAtivo(true) {
        for (int i = 0; i < 10; ++i) {
            processos.emplace_back(getpid() + i);
        }
        elegerCoordenador();
    }

    void elegerCoordenador() {
        int maiorPID = -1;
        for (auto& processo : processos) {
            if (processo.ativo && processo.pid > maiorPID) { // Considera apenas processos ativos
                maiorPID = processo.pid;
            }
        }
        coordenador = maiorPID;
        grupo.enviarMensagem("Novo coordenador eleito: " + std::to_string(coordenador));

        // Desativa o processo que foi eleito coordenador
        for (auto& processo : processos) {
            if (processo.pid == coordenador) {
                processo.ativo = false; // Marca o coordenador como inativo
                break;
            }
        }

        if (coordenadorThread.joinable()) {
            coordenadorThread.join(); // Aguarda a thread anterior do coordenador finalizar
        }

        // Inicia uma nova thread para o novo coordenador
        coordenadorVivo = true; // Marca o coordenador como vivo antes de iniciar a nova thread
        coordenadorThread = std::thread(&SistemaDistribuido::coordenadorFunc, this);
    }

    void monitorarCoordenador() {
        while (sistemaAtivo) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Intervalo para monitorar o coordenador
            if (!coordenadorVivo) { // Verifica se o coordenador está "vivo"
                desativarCoordenador(); // Marca o coordenador como inativo
                grupo.enviarMensagem("Coordenador falhou. Iniciando nova eleição em 5 segundos...");
                std::this_thread::sleep_for(std::chrono::seconds(5)); // Aguarda 5 segundos
                elegerCoordenador(); // Inicia nova eleição
            }
        }
    }

    void desativarCoordenador() {
        coordenadorVivo = false; // Marca o coordenador atual como inativo
        for (auto& processo : processos) {
            if (processo.pid == coordenador) {
                processo.ativo = false; // Marca o coordenador atual como inativo
                break;
            }
        }
        coordenador = -1; // Redefine o coordenador para forçar uma nova eleição
    }

    void coordenadorFunc() {
        while (coordenadorVivo && sistemaAtivo) {
            grupo.enviarMensagem("Sou o líder! (PID: " + std::to_string(coordenador) + ")");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    void processoFunc(int pid) {
        while (sistemaAtivo) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (pid != coordenador && processos[pid - getpid()].ativo) { // Apenas responde "Sim senhor" se não for o coordenador
                grupo.enviarMensagem("Sim senhor! (PID: " + std::to_string(pid) + ")");
            }
            checarAtivos(); // Verifica se ainda há processos ativos
        }
    }

    void checarAtivos() {
        int count = 0;
        for (const auto& processo : processos) {
            if (processo.ativo) {
                count++;
            }
        }
        if (count == 0) {
            grupo.enviarMensagem("Todos os processos foram desativados. Encerrando o sistema...");
            sistemaAtivo = false; // Finaliza o sistema
        }
    }

    void iniciar() {
        std::thread monitor(&SistemaDistribuido::monitorarCoordenador, this);
        std::vector<std::thread> threads;
        
        for (auto& processo : processos) {
            threads.emplace_back(&SistemaDistribuido::processoFunc, this, processo.pid);
        }
        
        monitor.detach();

        for (auto& t : threads) {
            t.join();
        }
        
        if (coordenadorThread.joinable()) {
            coordenadorThread.join();
        }
    }

    void simularFalhaCoordenador() {
        while (sistemaAtivo) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            coordenadorVivo = false; 
            grupo.enviarMensagem("Simulando falha do coordenador (PID: " + std::to_string(coordenador) + ")");
        }
    }

    void encerrarSistema() {
        sistemaAtivo = false;
        coordenadorVivo = false;
    }
};

int main() {
    SistemaDistribuido sistema;
    std::thread sistemaThread(&SistemaDistribuido::iniciar, &sistema);
    std::thread falhaThread(&SistemaDistribuido::simularFalhaCoordenador, &sistema);

    sistemaThread.join();
    falhaThread.join();

    sistema.encerrarSistema();

    return 0;
}
