# TVS, Inv 2022-2023 - Teste parcial - Exemplo 1

## Para implementar um serviço para operar como daemon pondera-se receber os pedidos através de um named pipe (ou fifo, com tipo p no sistema de ficheiros) ou através de um socket de domínio Unix (tipo s no sistema de ficheiros) de tipo stream. Qual seria a principal desvantagem para o named pipe?

A principal desvantagem de utilizar um named pipe (FIFO) para implementar um serviço daemon, em comparação com um socket de domínio Unix de tipo stream, é a limitação na comunicação bidirecional e na flexibilidade de conexão. Aqui estão algumas desvantagens específicas:

### Limitações do Named Pipe (FIFO)

1. **Comunicação Unidirecional**:
   - Named pipes são normalmente usados para comunicação unidirecional. Embora seja possível configurar comunicação bidirecional, isso não é tão intuitivo quanto com sockets de domínio Unix.

2. **Gestão de Conexões**:
   - Named pipes são menos flexíveis na gestão de múltiplas conexões simultâneas. Eles funcionam bem para comunicação simples entre dois processos, mas não são ideais para cenários em que múltiplos clientes precisam se conectar ao daemon ao mesmo tempo.

3. **Performance e Overhead**:
   - Named pipes podem ter mais overhead e menor desempenho para comunicações complexas ou de alto volume, em comparação com sockets de domínio Unix.

### Vantagens dos Sockets de Domínio Unix (Stream)

1. **Comunicação Bidirecional e Flexível**:
   - Sockets de domínio Unix permitem comunicação bidirecional por natureza, facilitando a troca de dados em ambos os sentidos entre cliente e servidor.

2. **Gestão de Múltiplas Conexões**:
   - Sockets de domínio Unix são mais adequados para serviços que precisam lidar com múltiplas conexões simultâneas, com a capacidade de aceitar e gerenciar vários clientes de forma eficiente.

3. **Desempenho Superior**:
   - Sockets de domínio Unix frequentemente oferecem melhor desempenho e menor latência em comparação com named pipes, especialmente em aplicações que requerem comunicação intensiva.

### Consideração Final

De modo geral, para um serviço daemon que deve lidar com múltiplas conexões simultâneas e requer comunicação bidirecional eficiente, os sockets de domínio Unix de tipo stream são normalmente a escolha preferida devido à sua flexibilidade e desempenho superiores.

## 2. Um dos mecanismos previstos para comunicação entre processos (IPC) em sistemas da família Unix é a memória partilhada. No entanto, a utilização de memória partilhada geralmente implica a utilização de outros mecanismos de comunicação entre processos. Porquê?

A utilização de memória partilhada (shared memory) em sistemas Unix para IPC é eficaz porque permite que múltiplos processos acessem e modifiquem a mesma região de memória, o que pode ser muito eficiente em termos de desempenho. No entanto, o uso de memória partilhada geralmente implica a utilização de outros mecanismos de comunicação entre processos, principalmente por causa das seguintes razões:

### Sincronização

1. **Controle de Acesso**:
   - Sem um mecanismo de sincronização, múltiplos processos podem tentar acessar e modificar a memória partilhada simultaneamente, resultando em condições de corrida (race conditions) e dados corrompidos. Para evitar isso, são utilizados mecanismos de sincronização como semáforos, mutexes ou locks para coordenar o acesso à memória partilhada.

### Notificação

2. **Coordenação de Eventos**:
   - Quando um processo escreve dados na memória partilhada, os outros processos precisam ser notificados dessa alteração. Sinais (signals), pipes, ou sockets são frequentemente usados para enviar notificações entre processos sobre a disponibilidade de novos dados ou mudanças na memória partilhada.

### Inicialização e Limpeza

3. **Configuração Inicial**:
   - Antes que a memória partilhada possa ser utilizada, os processos precisam coordenar a criação e a inicialização dessa memória. Isso muitas vezes requer a troca de mensagens para assegurar que todos os processos envolvidos estejam prontos e configurados corretamente.

4. **Finalização e Limpeza**:
   - Da mesma forma, quando a memória partilhada não é mais necessária, os processos precisam coordenar a liberação dos recursos. Isso pode ser feito utilizando sinais ou mensagens para garantir que todos os processos liberem a memória de forma ordenada.

### Exemplo Prático

Suponha que dois processos, A e B, precisem compartilhar dados. Eles podem usar um semáforo para sincronizar o acesso à memória partilhada, garantindo que apenas um processo acesse a memória de cada vez. Além disso, quando o processo A escreve dados na memória, ele pode enviar um sinal ou usar um socket para notificar o processo B de que os dados estão prontos para serem lidos.

### Conclusão

Embora a memória partilhada seja um mecanismo poderoso e eficiente para IPC, ela geralmente precisa ser combinada com outros mecanismos para garantir a sincronização adequada, coordenação de eventos e gestão de recursos, tornando a comunicação entre processos segura e eficaz.

## 3. Um servicço systemd pode estar: enabled, disabled, active, inactive.

## a. o que indica cada um destes quatro estados?

No `systemd`, os estados de um serviço indicam o seu status de inicialização e execução. Aqui está o que cada um desses quatro estados significa:

### 1. **Enabled (Habilitado)**:
   - **Indicação**: O serviço está configurado para iniciar automaticamente durante a inicialização do sistema.
   - **Configuração**: Os links simbólicos para o serviço estão presentes nos diretórios de `wants` ou `requires` de um ou mais alvos (`targets`), como `multi-user.target.wants/`.
   - **Exemplo de Comando**: Você pode habilitar um serviço com `sudo systemctl enable nome_do_serviço`.

### 2. **Disabled (Desabilitado)**:
   - **Indicação**: O serviço não está configurado para iniciar automaticamente durante a inicialização do sistema.
   - **Configuração**: Não há links simbólicos para o serviço nos diretórios de `wants` ou `requires` dos alvos (`targets`).
   - **Exemplo de Comando**: Você pode desabilitar um serviço com `sudo systemctl disable nome_do_serviço`.

### 3. **Active (Ativo)**:
   - **Indicação**: O serviço está atualmente em execução.
   - **Status de Execução**: O processo do serviço foi iniciado e está funcionando como esperado.
   - **Exemplo de Comando**: Você pode verificar se um serviço está ativo com `systemctl is-active nome_do_serviço`.

### 4. **Inactive (Inativo)**:
   - **Indicação**: O serviço não está em execução no momento.
   - **Status de Execução**: O processo do serviço foi parado ou nunca foi iniciado desde a inicialização do sistema.
   - **Exemplo de Comando**: Você pode verificar se um serviço está inativo com `systemctl is-active nome_do_serviço`.

### Exemplo de Uso

Aqui está um exemplo prático de como você pode usar os comandos `systemctl` para gerenciar um serviço chamado `example.service`:

- **Habilitar o serviço**:

```bash
sudo systemctl enable example.service
```

- **Desabilitar o serviço**:

```bash
sudo systemctl disable example.service
```

- **Iniciar o serviço (torná-lo ativo)**:

```bash
sudo systemctl start example.service
```

- **Parar o serviço (torná-lo inativo)**:

```bash
sudo systemctl stop example.service
```

Esses estados ajudam a gerenciar quando e como os serviços são executados no sistema, proporcionando controle e flexibilidade na administração de serviços.

## b. Indique os pares destes quatro estados que são legítimos de ocorrer (compatíveis entre si).

Os estados de um serviço `systemd` podem ocorrer em combinações legítimas, representando a configuração de inicialização e o status de execução do serviço. Aqui estão os pares de estados que são compatíveis entre si:

### Pares Legítimos

1. **Enabled e Active**:
   - O serviço está configurado para iniciar automaticamente na inicialização e está atualmente em execução.
   - **Exemplo**: Um serviço de rede que é ativado no boot e está atualmente ativo e em operação.

2. **Enabled e Inactive**:
   - O serviço está configurado para iniciar automaticamente na inicialização, mas no momento está parado ou ainda não foi iniciado.
   - **Exemplo**: Um serviço que foi configurado para iniciar no boot, mas foi manualmente parado pelo administrador.

3. **Disabled e Active**:
   - O serviço não está configurado para iniciar automaticamente na inicialização, mas foi iniciado manualmente e está atualmente em execução.
   - **Exemplo**: Um serviço que é iniciado manualmente para manutenção ou testes e, portanto, não é ativado automaticamente no boot.

4. **Disabled e Inactive**:
   - O serviço não está configurado para iniciar automaticamente na inicialização e também não está em execução no momento.
   - **Exemplo**: Um serviço que não é necessário no sistema e está totalmente desativado, sem estar em execução.

### Resumo dos Pares Compatíveis

- **Enabled e Active**
- **Enabled e Inactive**
- **Disabled e Active**
- **Disabled e Inactive**

Esses pares são legítimos e representam os diferentes estados de configuração e execução de serviços gerenciados pelo `systemd` no sistema Linux.

## 4. No âmbito do systemd, explique sucintamente o que é um ficheiro de unidade do tipo .socket e o que se entende por socket activation.

### Ficheiro de Unidade do Tipo .socket

Um ficheiro de unidade do tipo `.socket` no `systemd` é usado para definir sockets que são gerenciados pelo `systemd` e podem ser utilizados para a comunicação entre processos (IPC). Ele especifica as propriedades e comportamentos dos sockets, como os tipos de sockets (TCP, UDP, Unix domain sockets), endereços, portas, permissões, entre outras configurações.

Exemplo de um ficheiro `.socket`:

```ini
[Unit]
Description=Socket para o meu serviço

[Socket]
ListenStream=/run/meu_servico.sock

[Install]
WantedBy=sockets.target
```

### Socket Activation

Socket activation é uma técnica usada pelo `systemd` para iniciar serviços sob demanda, quando uma conexão é recebida por um socket configurado. Em vez de iniciar o serviço durante a inicialização do sistema (boot), o `systemd` escuta no socket especificado e inicia o serviço apenas quando necessário, economizando recursos e melhorando a eficiência.

Como funciona:

1. **Configuração**: Um ficheiro de unidade `.socket` define o socket que o `systemd` deve monitorar.
2. **Monitoramento**: O `systemd` escuta no socket configurado e aguarda conexões.
3. **Ativação**: Quando uma conexão é recebida, o `systemd` ativa o serviço associado ao socket.
4. **Execução**: O serviço processa a conexão e responde ao cliente.

Exemplo de ficheiro de unidade de serviço associado a um socket:

```ini
[Unit]
Description=Meu Serviço

[Service]
ExecStart=/usr/bin/meu_servico

[Install]
WantedBy=multi-user.target
```

Com a configuração correta, o ficheiro `.socket` e o ficheiro de serviço trabalham juntos para permitir que o `systemd` inicie serviços apenas quando necessário, economizando recursos do sistema e melhorando a responsividade.

## 5. Explique sucintamente o que se entende por paravirtualização e qual a sua principal desvantagem.

### Paravirtualização

**Paravirtualização** é uma técnica de virtualização em que o sistema operacional convidado (guest OS) é modificado para ser ciente do hipervisor e para se comunicar diretamente com ele utilizando uma API especial conhecida como "hypercalls". Isso permite um desempenho mais eficiente, pois elimina a necessidade de emular completamente o hardware subjacente.

### Principal Desvantagem

A principal desvantagem da paravirtualização é a **necessidade de modificar o sistema operacional convidado**. Isso implica que nem todos os sistemas operacionais podem ser utilizados como convidados sem adaptações, o que limita a flexibilidade e a compatibilidade. Sistemas operacionais que não foram modificados para suportar paravirtualização não podem tirar proveito dessa técnica, tornando-a menos versátil em comparação com a virtualização completa (full virtualization), que pode executar sistemas operacionais não modificados.

## 6. Explique sucintamente o que se entende por máquinas virtuais de processo. Dê exemplos práticos da utilização deste tipo de virtualização no cenário particular do set de instruções da arquitetura do host ser distinto do existente no ambiente virtualizado. 

### Máquinas Virtuais de Processo

Máquinas virtuais de processo são um tipo de virtualização que permite a execução de um único processo ou aplicação em um ambiente virtual, proporcionando uma abstração completa da plataforma de hardware e do sistema operacional subjacente. Elas permitem que aplicações sejam executadas de maneira isolada e independente do sistema operacional ou da arquitetura de hardware do host.

### Exemplos Práticos

1. **Java Virtual Machine (JVM)**:
   - A JVM é um exemplo clássico de uma máquina virtual de processo. Ela permite que aplicações Java sejam executadas em qualquer sistema que tenha uma JVM instalada, independentemente da arquitetura de hardware. O código Java é compilado em bytecode, que a JVM interpreta e executa no ambiente virtualizado.

2. **.NET Common Language Runtime (CLR)**:
   - Similar à JVM, o CLR da Microsoft permite a execução de aplicações desenvolvidas em várias linguagens de programação (como C# e VB.NET) em qualquer sistema que suporte a CLR. O código é compilado em um bytecode intermediário (Microsoft Intermediate Language, MSIL), que a CLR interpreta e executa.

3. **Emuladores de Arquitetura Diferente**:
   - Em cenários onde a arquitetura do host é diferente do ambiente virtualizado, emuladores como QEMU são usados para criar um ambiente de execução para processadores distintos. Por exemplo, QEMU pode emular uma arquitetura ARM em um sistema x86, permitindo que aplicações desenvolvidas para ARM sejam executadas em um host x86.

### Vantagens

- **Portabilidade**: Aplicações podem ser executadas em qualquer plataforma que suporte a máquina virtual, independentemente da arquitetura de hardware.
- **Isolamento**: Cada aplicação ou processo é executado em um ambiente isolado, proporcionando maior segurança e estabilidade.
- **Flexibilidade**: Desenvolvedores podem escrever código uma vez e executá-lo em diversas plataformas sem modificações adicionais.

Esses exemplos ilustram como máquinas virtuais de processo permitem a execução eficiente e isolada de aplicações, independentemente das diferenças nas arquiteturas de hardware entre o host e o ambiente virtualizado.

## 8. Comente a seguinte afirmação:
> «Um dos custos incontornáveis do sistema de contentores Docker é o de manter múltiplas versões do kernel Linux para que, por exemplo, as imagens baseadas em Ubuntu 22.04 usem o kernel 5.15 enquanto as imagens baseadas em Red Hat Enterprise Linux 8 precisam do kernel 4.18.»

Essa afirmação não está correta. No sistema de contentores Docker, todos os contentores compartilham o mesmo kernel do host. Isso significa que as imagens baseadas em diferentes distribuições, como Ubuntu 22.04 e Red Hat Enterprise Linux 8, usam o mesmo kernel do sistema operacional host onde o Docker está sendo executado.

### Explicação Detalhada

1. **Kernel Compartilhado**:
   - Docker opera através de namespaces e cgroups para fornecer isolamento entre os contêineres. No entanto, todos os contêineres compartilham o mesmo kernel do host. Isso significa que apenas uma versão do kernel é necessária no host, independentemente das versões das imagens baseadas em diferentes distribuições Linux que estão sendo executadas.

2. **Compatibilidade das Imagens**:
   - Embora as imagens Docker possam ser baseadas em diferentes distribuições Linux, elas são projetadas para serem compatíveis com o kernel do host. As bibliotecas e dependências necessárias para as aplicações dentro dos contêineres estão incluídas na própria imagem.

3. **Manutenção Simplificada**:
   - A manutenção de múltiplas versões do kernel não é necessária, já que o Docker usa o kernel do sistema host. Isso simplifica a gestão e manutenção dos sistemas que executam contêineres Docker.

### Conclusão

Portanto, ao usar Docker, não há um "custo incontornável" de manter múltiplas versões do kernel Linux. A infraestrutura de contêineres é projetada para usar um único kernel compartilhado pelo host, permitindo que diferentes imagens de contêiner coexistam de maneira eficiente e simplificada.

## 9. Um ficheiro docker-compose.yml, para especificação de uma solução composta com o nome tp2, contém três serviços: svca, svcb e svcc, todos colocados na mesma rede, svcnet, de tipo bridge. Os serviços svca e svcc têm apenas uma instância cada um, mas o serviço svcb foi lançado com scale=4. Executando um shell (/bin/sh) no contentor do serviço svca, qual é a diferença observável entre executar ping svcb ou ping tp2-svcb-1 ?

### Diferença Observável entre `ping svcb` e `ping tp2-svcb-1`

Quando você executa um shell (`/bin/sh`) no contêiner do serviço `svca` e utiliza os comandos `ping svcb` e `ping tp2-svcb-1`, a diferença observável está relacionada ao comportamento de resolução de nomes de host e à forma como o Docker Compose gerencia múltiplas instâncias de um serviço escalado.

#### 1. `ping svcb`

- **Resolução de Nome de Serviço**:
  - Quando você executa `ping svcb`, o Docker Compose utiliza o DNS interno para resolver o nome do serviço `svcb`. Como o serviço `svcb` foi escalado com `scale=4`, isso significa que há quatro instâncias do serviço `svcb` em execução, nomeadas como `tp2-svcb-1`, `tp2-svcb-2`, `tp2-svcb-3` e `tp2-svcb-4`.
  - O DNS interno do Docker realiza um balanceamento de carga round-robin entre essas instâncias. Isso significa que cada solicitação para `svcb` será resolvida para um dos quatro contêineres de forma cíclica. Assim, um comando `ping svcb` pode ser resolvido para qualquer um dos contêineres `tp2-svcb-1` a `tp2-svcb-4`, e cada execução do comando pode alcançar uma instância diferente.

#### 2. `ping tp2-svcb-1`

- **Resolução de Nome Específico da Instância**:
  - Quando você executa `ping tp2-svcb-1`, você está especificando diretamente uma instância particular do serviço `svcb`. Neste caso, a solicitação será resolvida especificamente para o contêiner `tp2-svcb-1`, ignorando o balanceamento de carga round-robin.
  - Esse comando sempre irá direcionar o ping para a mesma instância específica, `tp2-svcb-1`.

### Resumo

- **`ping svcb`**: Utiliza o DNS interno do Docker para resolver o nome do serviço `svcb` e faz o balanceamento de carga entre todas as instâncias do serviço (`tp2-svcb-1`, `tp2-svcb-2`, `tp2-svcb-3`, e `tp2-svcb-4`).
- **`ping tp2-svcb-1`**: Resolve diretamente para a instância específica `tp2-svcb-1` do serviço `svcb`.

Isso permite que você teste tanto o comportamento de balanceamento de carga entre várias instâncias quanto o acesso direto a uma instância específica dentro da mesma rede do Docker Compose.
