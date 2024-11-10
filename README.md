# Algoritmo de Eleição Distribuída

Este projeto implementa um algoritmo de eleição distribuída em C++. O objetivo é simular um sistema distribuído onde processos elegem um coordenador e monitoram sua atividade.

## Estrutura do Código

### Classes

- **Processo**: Representa um processo no sistema distribuído.
- **MulticastGroup**: Simula o envio de mensagens multicast.
- **SistemaDistribuido**: Gerencia os processos, a eleição do coordenador e a monitorização do sistema.

### Métodos Principais

- **elegerCoordenador()**: Realiza a eleição do coordenador.
- **monitorarCoordenador()**: Monitora a atividade do coordenador.
- **desativarCoordenador()**: Marca o coordenador atual como inativo.
- **coordenadorFunc()**: Função executada pelo coordenador.
- **processoFunc(int pid)**: Função executada pelos processos.
- **checarAtivos()**: Verifica se ainda há processos ativos.
- **iniciar()**: Inicia o sistema distribuído.
- **simularFalhaCoordenador()**: Simula a falha do coordenador.
- **encerrarSistema()**: Encerra o sistema.

## Como Executar

1. Compile o código:
    ```sh
    g++ eleicao.cpp -o eleicao -pthread
    ```

2. Execute o programa:
    ```sh
    ./eleicao
    ```

## Funcionamento

- O sistema inicia com 10 processos.
- Um coordenador é eleito com base no maior PID.
- O coordenador envia mensagens periódicas indicando que está ativo.
- Os processos respondem ao coordenador.
- Se o coordenador falhar, uma nova eleição é iniciada após 5 segundos.
- O sistema simula falhas do coordenador a cada 10 segundos.

## Dependências

- C++
- Biblioteca de threads (`<thread>`)
- Biblioteca de sincronização (`<mutex>`, `<atomic>`)
- Biblioteca de tempo (`<chrono>`)
- Biblioteca de sinal (`<csignal>`)
- Biblioteca de entrada/saída (`<iostream>`)

## Autor

- Desenvolvido por João Pedro dos Santos Henrique Plinta

## Licença

Este projeto está licenciado sob a [MIT License](LICENSE).
