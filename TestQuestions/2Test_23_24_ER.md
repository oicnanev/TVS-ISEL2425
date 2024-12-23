# TVS, Inv 2023/2024 - Época de Recurso - Teste parcial 2

## 6. Num sistema Linux estão disponíveis múltiplos mecanismos de comunicação entre processos, incluindo named pipes (fifos) e sockets de domínio Unix (Unix Domain Sockets).

## a. Indique duas características semelhantes nestes dois mecanismos de comunicação.

Tanto os Named Pipes (FIFOs) quanto os Unix Domain Sockets são mecanismos de comunicação entre processos (IPC) no sistema Linux. Duas características semelhantes entre esses dois mecanismos são:

1. **Comunicação entre processos (IPC)**:
   - Ambos permitem a comunicação entre diferentes processos no mesmo sistema. Eles fornecem um canal para que os processos possam enviar e receber dados uns dos outros.

2. **Orientação a fluxo de dados**:
   - Tanto os Named Pipes quanto os Unix Domain Sockets são orientados a fluxo de dados, o que significa que eles transmitem os dados em uma sequência contínua de bytes. Isso é útil para comunicação de dados em tempo real, onde a ordem dos bytes é importante.

Essas características tornam ambos os mecanismos úteis para troca de informações entre processos de forma eficiente e organizada em sistemas Linux.

## b. Explique a diferença fundamental entre os dois mecanismos e qual tem mais vantagens.

### Diferença Fundamental

1. **Named Pipes (FIFOs)**:
   - **Definição**: Named Pipes, ou FIFOs (First In, First Out), são uma forma de comunicação unidirecional entre processos. Eles são criados como arquivos especiais no sistema de arquivos e podem ser acessados através de um nome definido.
   - **Uso**: Um processo grava dados no Named Pipe e outro processo lê esses dados. É importante que a leitura e escrita sejam realizadas de forma síncrona, já que um Named Pipe atua como um buffer.

2. **Unix Domain Sockets**:
   - **Definição**: Unix Domain Sockets são uma interface de comunicação entre processos que permite a troca de dados bidirecionalmente. Eles são semelhantes aos sockets de rede, mas são usados para comunicação local dentro do mesmo sistema operacional.
   - **Uso**: Permitem que processos se comuniquem de maneira mais flexível e eficiente. Eles suportam tanto comunicação orientada a fluxo (como Named Pipes) quanto comunicação orientada a mensagens.

### Vantagens

1. **Named Pipes (FIFOs)**:
   - **Simplicidade**: Named Pipes são simples de usar e configurar, especialmente para tarefas de comunicação linear e unidirecional entre processos.
   - **Compatibilidade**: São compatíveis com uma ampla gama de programas e scripts que exigem uma abordagem FIFO para comunicação.

2. **Unix Domain Sockets**:
   - **Desempenho**: Geralmente oferecem melhor desempenho devido à comunicação direta entre processos, evitando a sobrecarga de comunicação em rede.
   - **Flexibilidade**: Suportam tanto comunicação orientada a fluxo quanto a mensagens, permitindo mais opções de implementação.
   - **Segurança**: Oferecem maior controle sobre os aspectos de segurança, como permissões de acesso e controle de conexões.

### Qual tem mais vantagens?

A escolha entre Named Pipes e Unix Domain Sockets depende do caso de uso específico:

- **Para Tarefas Simples e Unidirecionais**: Named Pipes são mais simples de implementar e podem ser suficientes para muitas tarefas de comunicação unidirecional entre processos.

- **Para Tarefas Complexas e Bidirecionais**: Unix Domain Sockets são mais poderosos e flexíveis, oferecendo melhor desempenho e mais opções de comunicação, tornando-os uma escolha melhor para tarefas que exigem comunicação bidirecional, baixo atraso, e melhor controle de segurança.

## 7. Alguns processos num sistema Linux correspondem a serviços a operar como daemons que, em geral, operam continuamente, parametrizados por ficheiros de configuração. Explique que mecanismo é normalmente usado para lhes indicar que houve alteração das configurações ou que devem terminar?

Em sistemas Linux, os daemons (serviços que operam continuamente em segundo plano) geralmente respondem a sinais (signals) para controlar suas operações, como a alteração de configurações ou a finalização do processo. Os sinais são um mecanismo de comunicação eficiente entre o sistema operacional e os processos. Aqui estão os sinais mais comuns usados para essas finalidades:

### Alteração das Configurações

1. **SIGHUP (Signal Hang Up)**:
   - **Uso**: O sinal `SIGHUP` é frequentemente utilizado para indicar a um daemon que ele deve recarregar sua configuração. Isso permite que o daemon aplique novas configurações sem a necessidade de ser completamente reiniciado.
   - **Como Usar**: Para enviar um `SIGHUP` a um processo, você pode usar o comando `kill` seguido do ID do processo (PID). Por exemplo, se o PID do daemon for `1234`:

```bash
kill -HUP 1234
```

### Finalização do Processo

1. **SIGTERM (Signal Terminate)**:
   - **Uso**: O sinal `SIGTERM` solicita educadamente que um processo termine. Este é o sinal padrão enviado pelo comando `kill` e permite que o processo execute qualquer procedimento de limpeza necessário antes de encerrar.
   - **Como Usar**: Para enviar um `SIGTERM` a um processo:

```bash
kill 1234
```

2. **SIGKILL (Signal Kill)**:
   - **Uso**: O sinal `SIGKILL` força a terminação imediata de um processo. Este sinal não pode ser interceptado ou ignorado pelo processo, garantindo que ele seja encerrado imediatamente. É geralmente usado como último recurso se o processo não responder ao `SIGTERM`.
   - **Como Usar**: Para enviar um `SIGKILL` a um processo:

```bash
kill -9 1234
```

Esses sinais são mecanismos essenciais para a gestão e controle de daemons em sistemas Linux, permitindo que os administradores de sistema gerenciem eficientemente os serviços em execução.

Em sistemas que usam `systemd`é possível também usar o comando `sustemctl` seguido de `reload [service name]` ou seguido de `stop [service name]`

## 8. Num sistema Linux com systemd, como se determina quais as unidades a ativar no arranque do sistema? Na sua resposta deixe claro se existe algum ficheiro com a lista de unidades a ativar, se é necessário ler todos os ficheiros de unidade presentes nas diretorias standard, ou se é de outra forma.

Em um sistema Linux com `systemd`, as unidades (services, targets, timers, etc.) a serem ativadas no arranque do sistema são determinadas por uma combinação de configurações de arquivos e dependências entre unidades. Abaixo estão as principais formas de como isso é determinado:

### 1. Diretórios de Unidade

As unidades que devem ser ativadas no arranque são definidas nos seguintes diretórios padrão:

- `/etc/systemd/system/`
- `/lib/systemd/system/`
- `/usr/lib/systemd/system/`

Esses diretórios contêm os arquivos de unidade (unit files) que descrevem os serviços e outros componentes do sistema.

### 2. Arquivos de Unidade e Links Simbólicos

Os serviços e unidades a serem iniciados no arranque são geralmente ativados por meio de links simbólicos nos seguintes diretórios:

- `/etc/systemd/system/default.target.wants/`
- `/etc/systemd/system/multi-user.target.wants/`
- `/etc/systemd/system/graphical.target.wants/`

Esses diretórios contêm links simbólicos para os arquivos de unidade que devem ser ativados no arranque. O `default.target` é um link para o alvo que deve ser inicializado pelo systemd, como `multi-user.target` ou `graphical.target`.

### 3. Comandos para Gerenciar Unidades

- Para **listar todas as unidades** habilitadas para iniciar no arranque, você pode usar:

```bash
systemctl list-unit-files --state=enabled
```

- Para **verificar as unidades** que são dependências de um alvo específico, como `multi-user.target`, use:

```bash
systemctl list-dependencies multi-user.target
```

- Para **habilitar** uma unidade para iniciar no arranque, use:

```bash
sudo systemctl enable nome_da_unidade.service
```

- Para **desabilitar** uma unidade, use:

```bash
sudo systemctl disable nome_da_unidade.service
```

### Resumo

Em resumo, o `systemd` determina as unidades a serem ativadas no arranque lendo os links simbólicos presentes nos diretórios de `wants` (e outros diretórios específicos). Esses links apontam para os arquivos de unidade que descrevem os serviços e outras unidades que devem ser iniciadas.

## 9. Considere a seguinte afirmação, retirada de documentação disponível em suse.com, e indique a palavra em falta, explicando que vantagem tem o uso desta técnica de virtualização.

> To support ________, OSes need to be modified to implement an API that lets them exchange
hypercalls with the hypervisor. [This kind of] hypervisors [...] need OS support and special
drivers, which are now built into the Linux kernel as well as other OSes.

A palavra em falta, é paravirtualization

## 10. Num sistema Linux com o serviço docker ativo, está em execução um contentor baseado na imagem node:current-alpine3.18 e um baseado na imagem php:8.2-alpine3.18. Nos respectivos Dockerfile, ambas as imagens começam com FROM alpine:3.18 , pelo que partilham a imagem base do Linux Alpine. Se for executado um shell com docker exec -it <container_name> sh num dos contentores e nesse shell for pedida a instalação de um pacote adicional do Alpine, como por exemplo o fortune (apk add fortune), este pacote fica de imediato disponível nos dois contentores? Explique.

Não, o pacote adicional não ficará disponível nos dois contentores de imediato. Cada contentor Docker é um ambiente isolado com seu próprio sistema de arquivos. Isso significa que as alterações feitas dentro de um contentor (como a instalação de um pacote) não afetam os outros contentores, mesmo que eles partilhem a mesma imagem base.

Vamos entender isso em detalhes:

### Estrutura de Imagens e Contentores

1. **Imagem Base**: A imagem `alpine:3.18` é usada como ponto de partida para criar outras imagens, como `node:current-alpine3.18` e `php:8.2-alpine3.18`. Essas imagens derivadas têm suas próprias camadas adicionais além da base `alpine:3.18`.

2. **Contentores**: Quando você inicia um contentor a partir de uma imagem, ele cria um novo sistema de arquivos a partir dessa imagem e adiciona uma camada de gravação no topo. Qualquer alteração feita dentro do contentor (como instalar pacotes) ocorre apenas nessa camada de gravação.

### Isolamento dos Contentores

Quando você executa um comando como `apk add fortune` dentro de um contentor, o pacote `fortune` é instalado apenas nesse contentor específico. Os outros contentores permanecem inalterados, mesmo que sejam baseados na mesma imagem inicial. Isso é devido ao isolamento proporcionado pelo Docker, que garante que cada contentor seja independente em termos de sistema de arquivos, rede, e outros recursos.

### Como Tornar um Pacote Disponível para Vários Contentores

Se você quiser que o pacote `fortune` esteja disponível em ambos os contentores, você terá que instalá-lo separadamente em cada um deles, ou criar uma nova imagem personalizada com o pacote pré-instalado e iniciar os contentores a partir dessa nova imagem.

Por exemplo, você pode criar uma nova imagem Docker com o `Dockerfile` seguinte:

```Dockerfile
FROM alpine:3.18
RUN apk add --no-cache fortune
```

E então construir e iniciar os contentores a partir dessa imagem:

```bash
docker build -t custom-alpine-fortune .
docker run -it custom-alpine-fortune
```

Isso garante que todos os contentores baseados nessa imagem personalizada terão o pacote `fortune` instalado.

## 11. Considere o Dockerfile apresentado:

```Dockerfile
FROM eclipse-temurin:21-jdk-alpine
RUN mkdir -p /opt/isel/tvs/hello
COPY README.md /opt/isel/tvs/hello/
WORKDIR /build/app/src
COPY src/ ./
RUN javac -d ../bin Hello.java
WORKDIR /build/app/bin
RUN jar cvfe Hello.jar Hello Hello.class
WORKDIR /opt/isel/tvs/hello
RUN cp /build/app/bin/Hello.jar .
CMD ["java", "-jar", "Hello.jar"]
```

## a. Indique que camadas (não-vazias) do sistema de ficheiros são criadas sobre a imagem eclipse-temurin.

O Dockerfile apresentado cria várias camadas de sistema de ficheiros sobre a imagem base `eclipse-temurin:21-jdk-alpine`. Cada instrução `RUN`, `COPY` e `WORKDIR` cria uma camada (layer) adicional. Vou listar as camadas não-vazias criadas:

1. **FROM eclipse-temurin:21-jdk-alpine**
   - Esta instrução não cria uma nova camada, mas define a imagem base.

2. **RUN mkdir -p /opt/isel/tvs/hello**
   - Cria a camada que inclui a criação do diretório `/opt/isel/tvs/hello`.

3. **COPY README.md /opt/isel/tvs/hello/**
   - Cria a camada que copia o arquivo `README.md` para o diretório `/opt/isel/tvs/hello/`.

4. **WORKDIR /build/app/src**
   - Define o diretório de trabalho. Esta instrução em si não altera o sistema de ficheiros, então não cria uma camada não-vazia.

5. **COPY src/ ./**
   - Cria a camada que copia o conteúdo do diretório `src` do host para o diretório de trabalho atual no contêiner (`/build/app/src`).

6. **RUN javac -d ../bin Hello.java**
   - Cria a camada que compila o arquivo `Hello.java` e coloca os arquivos compilados no diretório `/build/app/bin`.

7. **WORKDIR /build/app/bin**
   - Define o diretório de trabalho. Esta instrução em si não altera o sistema de ficheiros, então não cria uma camada não-vazia.

8. **RUN jar cvfe Hello.jar Hello Hello.class**
   - Cria a camada que empacota a classe `Hello` em um arquivo JAR chamado `Hello.jar`.

9. **WORKDIR /opt/isel/tvs/hello**
   - Define o diretório de trabalho. Esta instrução em si não altera o sistema de ficheiros, então não cria uma camada não-vazia.

10. **RUN cp /build/app/bin/Hello.jar .**
    - Cria a camada que copia o arquivo `Hello.jar` para o diretório `/opt/isel/tvs/hello`.

11. **CMD ["java", "-jar", "Hello.jar"]**
    - Define o comando padrão a ser executado quando o contêiner é iniciado. Esta instrução em si não altera o sistema de ficheiros, então não cria uma camada não-vazia.

### Resumo das Camadas Não-Vazias Criadas:

1. RUN mkdir -p /opt/isel/tvs/hello
2. COPY README.md /opt/isel/tvs/hello/
3. COPY src/ ./
4. RUN javac -d ../bin Hello.java
5. RUN jar cvfe Hello.jar Hello Hello.class
6. RUN cp /build/app/bin/Hello.jar .

Essas são as camadas de sistema de ficheiros que são criadas sobre a imagem `eclipse-temurin:21-jdk-alpine`. Cada uma delas adiciona novas alterações ao sistema de ficheiros dentro do contêiner.

## b. Modifique o Dockerfile para que as alterações ao ficheiro README.md tenham um impacto mínimo no reaproveitamento da build cache e  elimine a instrução que passa a ser desnecessária depois disso. Indique as camadas (não-vazias) da versão final.

Para minimizar o impacto das alterações ao arquivo `README.md` no reaproveitamento da build cache, podemos mover a instrução `COPY README.md /opt/isel/tvs/hello/` para o final do Dockerfile, antes da instrução `CMD`. Isso garantirá que as camadas principais que contêm a compilação do código Java não sejam recompiladas sempre que o `README.md` for alterado.

Também podemos eliminar a instrução `RUN mkdir -p /opt/isel/tvs/hello`, já que o diretório será criado quando copiarmos o `README.md`.

Aqui está a versão modificada do Dockerfile:

```Dockerfile
FROM eclipse-temurin:21-jdk-alpine

WORKDIR /build/app/src
COPY src/ ./
RUN javac -d ../bin Hello.java

WORKDIR /build/app/bin
RUN jar cvfe Hello.jar Hello Hello.class

WORKDIR /opt/isel/tvs/hello
RUN cp /build/app/bin/Hello.jar .
COPY README.md /opt/isel/tvs/hello/

CMD ["java", "-jar", "Hello.jar"]
```

### Camadas Não-Vazias da Versão Final

1. **FROM eclipse-temurin:21-jdk-alpine**
   - Define a imagem base. (Não cria uma nova camada).

2. **WORKDIR /build/app/src**
   - Define o diretório de trabalho. (Não cria uma camada não-vazia).

3. **COPY src/ ./**
   - Cria a camada que copia o conteúdo do diretório `src` do host para o diretório de trabalho atual no contêiner.

4. **RUN javac -d ../bin Hello.java**
   - Cria a camada que compila o arquivo `Hello.java` e coloca os arquivos compilados no diretório `/build/app/bin`.

5. **WORKDIR /build/app/bin**
   - Define o diretório de trabalho. (Não cria uma camada não-vazia).

6. **RUN jar cvfe Hello.jar Hello Hello.class**
   - Cria a camada que empacota a classe `Hello` em um arquivo JAR chamado `Hello.jar`.

7. **WORKDIR /opt/isel/tvs/hello**
   - Define o diretório de trabalho. (Não cria uma camada não-vazia).

8. **RUN cp /build/app/bin/Hello.jar .**
   - Cria a camada que copia o arquivo `Hello.jar` para o diretório `/opt/isel/tvs/hello`.

9. **COPY README.md /opt/isel/tvs/hello/**
   - Cria a camada que copia o arquivo `README.md` para o diretório `/opt/isel/tvs/hello`.

10. **CMD ["java", "-jar", "Hello.jar"]**
    - Define o comando padrão a ser executado quando o contêiner é iniciado. (Não cria uma camada não-vazia).

Com essas alterações, a imagem deve reaproveitar melhor o cache para as etapas de compilação e criação do JAR, sendo recompiladas apenas quando realmente necessário.

## 12. Se os contentores em execução numa máquina Linux partilham o mesmo kernel, como é possível que se observem simultaneamente identificadores (pid) iguais em processos de diferentes contentores?

Embora os contentores em execução numa máquina Linux compartilhem o mesmo kernel, eles utilizam uma técnica chamada namespaces para fornecer isolamento. Um dos namespaces usados é o PID namespace. Isso permite que cada contentor tenha sua própria visão isolada dos identificadores de processos (PIDs), o que significa que PIDs iguais podem existir simultaneamente em diferentes contentores.

### Como Funciona o PID Namespace

1. **Isolamento de Processos**:
   - O PID namespace cria uma hierarquia de namespaces de processos. Dentro de um namespace PID, os processos têm identificadores independentes dos PIDs fora desse namespace. Isso significa que um processo dentro de um contentor pode ter um PID que parece ser o mesmo que um processo em outro contentor ou no host, mas, na verdade, estão em espaços de nomes diferentes.

2. **Hierarquia de PID Namespaces**:
   - Quando um novo contentor é criado, é gerado um novo PID namespace. O processo inicial dentro deste contentor (geralmente PID 1 dentro do contentor) terá um PID único dentro desse namespace, mas poderá ter um PID completamente diferente no namespace do host.
   - Por exemplo, o processo PID 1 dentro de um contentor pode ser visto como PID 5001 no host.

### Vantagens e Segurança

- **Isolamento**: Garante que os processos dentro de um contentor não interfiram diretamente com os processos fora desse contentor ou em outros contentor. Cada contentor opera como se tivesse seu próprio conjunto de PIDs, contribuindo para a segurança e estabilidade.
  
- **Gerenciamento Simplificado**: Facilita o gerenciamento de processos dentro de contentores, já que cada um pode ter seus próprios processos e IDs de processos sem conflito com outros contentores.

### Exemplo Prático

Suponha que você tenha dois contentores em execução:

- **Contentor A**:
  - PID 1 (dentro do contentor)
  - Pode ser visto como PID 1001 no host

- **Contentor B**:
  - PID 1 (dentro do contentor)
  - Pode ser visto como PID 2001 no host

Embora ambos os contentores tenham um processo com PID 1 dentro de seus namespaces, os PIDs reais no namespace do host são diferentes, mostrando o isolamento proporcionado pelos namespaces de PID.

Essa abordagem permite que sistemas baseados em contentores, como Docker, ofereçam um ambiente altamente isolado e seguro, mesmo compartilhando o mesmo kernel.
