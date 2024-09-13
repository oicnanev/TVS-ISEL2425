# Class Notes

## 10SET24

### Enquadramento da disciplina no curso
![enquadramento](../img/1.jpeg)

| Semestre | Disciplina |
| -------- | -----------|
| 1º | PG, LSD |
| 2º | AC. AED |
| 3º | PSC |
| 4º | PC, LAE |
| 5º | TVS |

### Sylabus

| Tópico | Trabalho |
| ------ | -------- |
| **1. Sistemas operativos, processos, etc.** | CW1 |
| **2. Proteção de Hardware** | CW2 |
| Feedback #1 | 28..31 out |
| **Teste parcial 1** | 5nov ou 12nov |
| **3. Serviços, Hipervisores, Emuladores** | CW3 |
| **4. Contentores** | CW4 |
| Feedback #2 | 17..20 dez |
| **Teste parcial/ EN** | 6..18 jan |


## 11SET24

#### Sumário
- Taxonomia da Virtualização
- Processos dos Sistemas Operativos como Ambientes Virtuais de Execução
- Arquitetura Geral de um Sistema Operativo

### Virtualização
- Emulação, extensão ou modificação do comportamento de um objecto computacional (físico) através de *software*
- Virtualização do ambiente de execução, como exemplo a JVM

![virtualization](../img/virtualization.jpeg)

### Processo
- Representa um programa que está a correr, espaço em memória... Programa em funcionamento.
- Com o surgimento do UNIX em 1969/70 foi criado o conceito de acesso à memória por segmentação para virtualizar a memória. O espaço de memória de cada processo tem uma **base** corespondente na memória física. O **espaço de endereçamento virtual** é único por processo.
- Cada processo tem um CPU virtual na forma de uma **thread**. Cada processo pode ter uma ou mais threads.
- Cada processo tem uma **tabela de sessão** sobre *files*. Em UNIX, tudo o que seja passível de operações Input/Output é designado por *file*; em Windows, estas files são chamadas de objectos do sistema. *File* pode ser um ficheiro texto, uma directoria ou uma ligação à internet.

![process memory](../img/memory.jpeg)

### Arquitectura de um Sistema Operativo

![so arch](../img/so_arch.jpeg)

#### Sistema Operativo
- Kernel do SO
- Ambiente de Operação
    * Operation Environment
    * Programas necessários para o funcionamento do S.O., ex: dir/ls

#### Níveis de execução CPU x86
| # | Privilégio |
| -- | ---------- |
| 0 | Maior privilégio |
| 1 |   |
| 2 |   |
| 3 | Menor privilégio - algumas instruções não são aceitáveis


