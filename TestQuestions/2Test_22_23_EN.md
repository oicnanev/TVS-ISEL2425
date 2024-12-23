# TVS, Inv 2022/2023 - Época Normal - Teste Parcial 2

## 6. Num sistema Linux, considere um serviço que atende os processos clientes através de um único socket de domínio Unix do tipo stream. É possível atender múltiplos clientes em simultâneo? Se sim, como se distingue cada um dos clientes no serviço (por exemplo, no momento de receber/enviar dados de/para cada um dos clientes)? Se não, como se pode desenhar uma solução para suportar este requisito?

Sim, é possível atender múltiplos clientes em simultâneo utilizando um único socket de domínio Unix do tipo stream. Essa abordagem é bastante comum em servidores de rede. Para distinguir e gerenciar cada cliente de forma independente, o serviço precisa criar um novo socket para cada conexão de cliente. Aqui está como isso funciona:

### Passos para Atender Múltiplos Clientes

1. **Criar o Socket Principal**:
   - O serviço cria um socket de domínio Unix principal e o configura para escutar conexões de clientes.

2. **Escutar Conexões**:
   - O socket principal é configurado para escutar (`listen()`) e aguarda conexões de entrada de clientes.

3. **Aceitar Conexões**:
   - Quando uma conexão de cliente é recebida, o serviço chama a função `accept()`, que cria um novo socket para essa conexão específica. O `accept()` retorna um novo descritor de socket (file descriptor) para cada cliente que se conecta, permitindo que o serviço distinga entre os diferentes clientes.

### Exemplo de Implementação

Aqui está um exemplo simples em C de como um servidor pode gerenciar múltiplos clientes usando sockets de domínio Unix:

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SOCKET_PATH "/run/my_socket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;

    // Criar o socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configurar o endereço do socket
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind do socket
    unlink(SOCKET_PATH); // Remover socket anterior se existir
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escutar por conexões
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Aceitar conexões de clientes
    while (1) {
        if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
            perror("accept");
            continue;
        }

        // Criar um novo processo para tratar o cliente
        if (fork() == 0) {
            // No processo filho
            close(server_fd);

            // Interação com o cliente
            char buffer[256];
            int n = read(client_fd, buffer, sizeof(buffer));
            if (n > 0) {
                buffer[n] = '\0';
                printf("Recebido do cliente: %s\n", buffer);
                write(client_fd, "Resposta do servidor", 20);
            }

            close(client_fd);
            exit(EXIT_SUCCESS);
        } else {
            // No processo pai
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
```

### Como Funciona

- **Criar e Escutar**: O servidor cria um socket, faz o bind a um caminho de socket, e começa a escutar por conexões de clientes.
- **Aceitar Conexões**: Quando uma conexão é aceita, o servidor cria um novo processo (usando `fork()`) para tratar o cliente. O descritor de socket retornado por `accept()` é único para cada cliente, permitindo a distinção e comunicação individual.
- **Tratar Clientes**: No processo filho, o servidor lê e escreve dados para o cliente usando o novo socket.

## 7. Alguns processos ativos num sistema Linux correspondem a serviços a operar como daemons. Indique três características associadas a este tipo de processos (não específicas de um gestor de serviços).

Daemons são processos em segundo plano que oferecem serviços ou realizam tarefas específicas em sistemas operativos baseados em Unix, como o Linux. Três características associadas a esses processos:

### 1. **Execução em Segundo Plano (Background Execution)**
   - Daemons geralmente são iniciados durante o processo de inicialização do sistema ou em resposta a certos eventos, e continuam em execução em segundo plano. Não têm interface de utilizador direta e não requerem interação contínua do utilizador.

### 2. **Desassociação de Terminal (Disassociation from Terminal)**
   - Um daemon, ao ser iniciado, desassocia-se do terminal de controle. Isso envolve a criação de um novo processo filho que se torna o daemon, enquanto o processo pai termina. Isso garante que o daemon não mantenha nenhum controle ou vínculo com o terminal onde foi iniciado, permitindo que continue a operar mesmo que o terminal seja fechado.

### 3. **Execução Contínua e Ciclo de Vida Longo (Continuous Operation and Long Lifespan)**
   - Daemons são projetados para ter um ciclo de vida longo e contínuo. Eles são frequentemente configurados para iniciar automaticamente no boot do sistema e continuar em execução até que o sistema seja desligado ou o daemon seja explicitamente interrompido. Eles devem ser capazes de lidar com várias requisições de serviços ao longo de seu ciclo de vida.

Essas características permitem que os daemons funcionem de forma eficiente e estável, fornecendo serviços essenciais ao sistema operativo sem a necessidade de interação direta com os utilizadores.

## 8. Num sistema Linux com systemd, o ficheiro /etc/systemd/system/tp2.service, na sua secção [Unit] tem a linha Requires=tp2.socket.

## a. Tendo em conta esta linha, que funcionalidade do systemd deverá estar a ser usada no serviço tp2 e porque aparece esta diretiva Requires no ficheiro .service?

No sistema `systemd`, a diretiva `Requires=tp2.socket` no ficheiro `/etc/systemd/system/tp2.service` indica que o serviço `tp2.service` depende do socket `tp2.socket`. A funcionalidade do `systemd` que é usada aqui chama-se **socket activation**.

### Funcionalidade: Socket Activation

**Socket activation** é uma técnica utilizada pelo `systemd` para iniciar serviços sob pedido quando uma conexão é recebida num socket. Em vez de iniciar o serviço `tp2.service` no boot ou manualmente, o `systemd` escuta no socket `tp2.socket` e ativa o serviço `tp2.service` apenas quando necessário, ou seja, quando uma conexão é recebida nesse socket.

### Por Que a Diretiva `Requires` Está Presente

- **Garantir a Disponibilidade do Socket**:
  - A diretiva `Requires=tp2.socket` assegura que o socket `tp2.socket` esteja disponível antes de iniciar o serviço `tp2.service`. Isso é importante porque o `tp2.service` provavelmente depende desse socket para receber conexões de clientes.

- **Gerenciamento de Dependências**:
  - A diretiva `Requires` estabelece uma relação de dependência explícita entre o serviço e o socket. O `systemd` sabe que deve iniciar e manter o socket `tp2.socket` enquanto o serviço `tp2.service` estiver ativo. Se o socket falhar ou for parado, o `systemd` também interromperá o serviço `tp2.service`.

- **Facilidade de Gestão**:
  - O uso da diretiva `Requires` facilita a configuração e gestão do serviço, garantindo que todos os componentes necessários estejam disponíveis e funcionando corretamente.

### Exemplo de Configuração

Aqui está um exemplo de como os ficheiros de unidade `.socket` e `.service` podem ser configurados:

**tp2.socket**:
```ini
[Unit]
Description=Socket for TP2 Service

[Socket]
ListenStream=/run/tp2.sock

[Install]
WantedBy=sockets.target
```

**tp2.service**:
```ini
[Unit]
Description=TP2 Service
Requires=tp2.socket
After=tp2.socket

[Service]
ExecStart=/usr/local/bin/tp2

[Install]
WantedBy=multi-user.target
```

Neste exemplo:
- O `tp2.socket` define um socket de domínio Unix que escuta no caminho `/run/tp2.sock`.
- O `tp2.service` depende do `tp2.socket` e é configurado para iniciar após o socket estar ativo.

Dessa forma, quando uma conexão é recebida no socket `/run/tp2.sock`, o `systemd` automaticamente ativa o serviço `tp2.service` para processar a solicitação.

## b. Deverá o ficheiro .socket ter uma linha equivalente (Requires=tp2.service)? Porquê?

Não, o ficheiro `.socket` não precisa ter uma linha equivalente `Requires=tp2.service`. A diretiva `Requires` no ficheiro `.service` já garante que o `tp2.service` depende do `tp2.socket`, mas a relação inversa não é necessária para o funcionamento correto. 

### Razões

1. **Funcionamento de Socket Activation**:
   - A funcionalidade de **socket activation** no `systemd` é projetada para iniciar o serviço (`.service`) automaticamente quando o socket (`.socket`) recebe uma conexão. Portanto, o `systemd` sabe que deve iniciar o `tp2.service` sempre que há atividade no `tp2.socket`.

2. **Ciclo de Vida Independente**:
   - O ciclo de vida do socket e do serviço são gerenciados de forma independente. O socket deve estar disponível para escutar conexões mesmo antes do serviço ser iniciado. Adicionar `Requires=tp2.service` no ficheiro `.socket` criaria uma dependência cíclica, potencialmente causando problemas de inicialização.

3. **Propósito da Dependência**:
   - A dependência `Requires=tp2.socket` no ficheiro `.service` garante que o socket esteja presente antes do serviço ser iniciado. No entanto, o inverso não é necessário porque o `systemd` já gerencia o início do serviço com base nas conexões do socket sem precisar de uma dependência explícita.

Portanto, a configuração correta é ter a dependência `Requires=tp2.socket` no ficheiro `.service`, enquanto o ficheiro `.socket` não precisa de uma linha equivalente.

## 9. A empresa VMware vende um produto, descrito como um hipervisor, cujas instruções de instalação contêm o seguinte texto: 

> «In a typical interactive installation, you boot the ESXi installer and respond to the installer prompts to install ESXi to the local host disk. The installer reformats and partitions the target disk and installs the ESXi boot image. If you have not installed ESXi on the target disk before, all data on the drive is overwritten, including hardware vendor partitions, operating system partitions, and associated data.»

Este é um hipervisor de tipo 1 ou de tipo 2? Justifique a sua afirmação com base no texto acima.

O VMware ESXi é um **hipervisor de tipo 1**.

### Justificação Baseada no Texto

O texto menciona que, durante a instalação:

1. **"You boot the ESXi installer and respond to the installer prompts to install ESXi to the local host disk."**
   - Isso indica que o ESXi é instalado diretamente no hardware do servidor (bare-metal), sem a necessidade de um sistema operativo subjacente.

2. **"The installer reformats and partitions the target disk and installs the ESXi boot image."**
   - Reformatar e particionar o disco de destino sugere que o ESXi assume controle total sobre o hardware, criando um ambiente virtualizado diretamente sobre o hardware físico.

3. **"If you have not installed ESXi on the target disk before, all data on the drive is overwritten, including hardware vendor partitions, operating system partitions, and associated data."**
   - A menção de sobrescrever partições do sistema operativo e dados associados reforça que o ESXi opera independentemente de qualquer sistema operativo existente, confirmando que é um hipervisor de tipo 1.

### Diferença entre Hipervisor de Tipo 1 e Tipo 2

- **Hipervisor de Tipo 1 (Bare-Metal)**:
  - Instala-se diretamente no hardware físico e gerencia os recursos do sistema, como CPU, memória e dispositivos de I/O.
  - Exemplos incluem VMware ESXi, Microsoft Hyper-V e Xen.
  - Oferece melhor desempenho e eficiência, pois não há uma camada adicional de sistema operativo entre o hipervisor e o hardware.

- **Hipervisor de Tipo 2 (Hosted)**:
  - Instala-se sobre um sistema operativo existente e depende desse sistema operativo para acessar os recursos do hardware.
  - Exemplos incluem VMware Workstation, Oracle VirtualBox e Parallels Desktop.
  - Pode ter mais overhead devido à presença do sistema operativo anfitrião.

Portanto, com base na descrição fornecida, é claro que o VMware ESXi é um hipervisor de tipo 1, pois é instalado diretamente no hardware do servidor e reformata o disco, removendo qualquer sistema operativo pré-existente.

## 10. Apresente duas vantagens que resultam da organização das imagens docker em camadas, suportadas por sistemas de ficheiros do tipo overlay. Apresente também uma possível desvantagem.

A organização das imagens Docker em camadas, suportadas por sistemas de ficheiros do tipo overlay, traz várias vantagens, mas também algumas desvantagens. Vamos analisar ambos os aspectos:

### Vantagens

1. **Eficiência de Armazenamento**:
   - **Redução do Espaço em Disco**: As imagens Docker são compostas por múltiplas camadas, onde cada camada é armazenada separadamente e pode ser compartilhada entre várias imagens. Isso significa que se várias imagens utilizarem a mesma base (por exemplo, uma imagem base de Ubuntu), essas camadas comuns são armazenadas apenas uma vez no disco, economizando espaço de armazenamento.
   - **Reutilização de Camadas**: Quando se criam novas imagens ou atualiza imagens existentes, o Docker reutiliza as camadas que já existem e só cria novas camadas para as mudanças, o que torna a construção de imagens mais rápida e eficiente.

2. **Velocidade de Construção e Deploy**:
   - **Construção Incremental**: Com a organização em camadas, o Docker pode construir imagens de maneira incremental. Ou seja, se uma camada intermediária não mudou, o Docker pode reutilizar a camada do cache em vez de reconstruí-la, acelerando o processo de construção.
   - **Deploys Mais Rápidos**: Ao fazer o deploy de contentores, apenas as camadas que não estão presentes no ambiente de destino precisam ser transferidas. Como resultado, os deploys se tornam mais rápidos e utilizam menos largura de banda.

### Desvantagem

1. **Complexidade de Gerenciamento de Camadas**:
   - **Acúmulo de Camadas**: Com o tempo, um número excessivo de camadas pode se acumular, especialmente se as imagens forem frequentemente atualizadas ou reconstruídas. Isso pode levar a um aumento na complexidade do gerenciamento de camadas antigas e inutilizadas.
   - **Desempenho de I/O**: Dependendo do número de camadas e do sistema de ficheiros overlay utilizado, o desempenho de I/O pode ser impactado. Cada camada adiciona uma sobrecarga adicional ao acessar arquivos, e um grande número de camadas pode resultar em operações de I/O mais lentas.

### Conclusão

A organização das imagens Docker em camadas oferece vantagens significativas em termos de eficiência de armazenamento e velocidade de construção e deploy. No entanto, também apresenta desafios em relação ao gerenciamento de camadas e potencial impacto no desempenho de I/O.

## 11. Considere o Dockerfile apresentado ao lado e três ficheiros: package.json e app.js, com uma aplicação para Node.js, e um README.md

```Dockerfile
FROM node:alpine
WORKDIR /home/node
COPY package.json .
RUN npm install
COPY app.js .
COPY README.md .
RUN chown -R node.node /home/node
USER node
EXPOSE 80
CMD ["node", "app,js"]
```

- Indique que camadas de overlay são criadas sobre a imagem base node:alpine
- Modifique o Dockerfile para minimizar o número de camadas de overlay
- Indique as camadas de overlay criadas sobre a base, considerando as alterações

### Camadas de Overlay Criadas no Dockerfile Original

No Dockerfile original, cada instrução cria uma nova camada. Aqui estão as camadas de overlay criadas sobre a imagem base `node:alpine`:

1. **FROM node:alpine**:
   - Utiliza a imagem base `node:alpine`.

2. **WORKDIR /home/node**:
   - Define o diretório de trabalho. Cria uma nova camada.

3. **COPY package.json .**:
   - Copia `package.json` para o diretório de trabalho. Cria uma nova camada.

4. **RUN npm install**:
   - Executa `npm install` para instalar dependências. Cria uma nova camada.

5. **COPY app.js .**:
   - Copia `app.js` para o diretório de trabalho. Cria uma nova camada.

6. **COPY README.md .**:
   - Copia `README.md` para o diretório de trabalho. Cria uma nova camada.

7. **RUN chown -R node.node /home/node**:
   - Altera a propriedade dos arquivos no diretório de trabalho. Cria uma nova camada.

8. **USER node**:
   - Muda o usuário para `node`. Cria uma nova camada.

9. **EXPOSE 80**:
   - Expõe a porta 80. Cria uma nova camada.

10. **CMD ["node", "app.js"]**:
    - Define o comando para iniciar a aplicação. Cria uma nova camada.

### Dockerfile Otimizado

Para minimizar o número de camadas de overlay, podemos combinar algumas instruções em uma única linha e reorganizar o Dockerfile:

```Dockerfile
FROM node:alpine
WORKDIR /home/node
COPY package.json .
RUN npm install && \
    chown -R node.node /home/node
COPY . .
USER node
EXPOSE 80
CMD ["node", "app.js"]
```

### Camadas de Overlay Criadas no Dockerfile Otimizado

Com as otimizações, aqui estão as camadas de overlay criadas sobre a imagem base `node:alpine`:

1. **FROM node:alpine**:
   - Utiliza a imagem base `node:alpine`.

2. **WORKDIR /home/node**:
   - Define o diretório de trabalho. Cria uma nova camada.

3. **COPY package.json .**:
   - Copia `package.json` para o diretório de trabalho. Cria uma nova camada.

4. **RUN npm install && chown -R node.node /home/node**:
   - Executa `npm install` e altera a propriedade dos arquivos em uma única instrução. Cria uma nova camada.

5. **COPY . .**:
   - Copia todos os arquivos (`app.js` e `README.md`) para o diretório de trabalho. Cria uma nova camada.

6. **USER node**:
   - Muda o usuário para `node`. Cria uma nova camada.

7. **EXPOSE 80**:
   - Expõe a porta 80. Cria uma nova camada.

8. **CMD ["node", "app.js"]**:
   - Define o comando para iniciar a aplicação. Cria uma nova camada.

### Benefícios das Modificações

- **Redução de Camadas**: As instruções `RUN npm install` e `RUN chown` foram combinadas, reduzindo o número total de camadas.
- **Eficiência**: O Docker pode reutilizar a cache mais eficientemente, especialmente quando apenas o código da aplicação é alterado.

## 12. Um ficheiro docker-compose.yml, para especificação de uma solução composta com o nome tp2, contém três serviços: svca, svcb e svcc, todos colocados na mesma rede, svcnet, de tipo bridge. Os serviços svca e svcc têm apenas uma instância cada um, mas o serviço svcb foi lançado com scale=4. Executando um shell (/bin/sh) no contentor do serviço svca, qual é a diferença observável entre executar nslookup svcb ou nslookup tp2-svcb-1 ?

Ao executar o comando `nslookup` no contentor do serviço `svca`, você observará diferentes comportamentos dependendo se você consulta `svcb` ou `tp2-svcb-1`. Vamos ver as diferenças:

### 1. `nslookup svcb`

- **Resolução de Nome de Serviço**:
  - Quando você executa `nslookup svcb`, está consultando o nome do serviço `svcb` que foi escalado com `scale=4`. Isso significa que o serviço `svcb` tem quatro instâncias em execução, nomeadas como `tp2-svcb-1`, `tp2-svcb-2`, `tp2-svcb-3` e `tp2-svcb-4`.
  - O Docker Compose configura um DNS interno que resolve o nome `svcb` para os endereços IP das quatro instâncias usando um mecanismo de balanceamento de carga round-robin. Portanto, cada execução de `nslookup svcb` pode retornar um endereço IP diferente, correspondendo a uma das quatro instâncias.

### 2. `nslookup tp2-svcb-1`

- **Resolução de Nome Específico da Instância**:
  - Quando se executa `nslookup tp2-svcb-1`, está-se consultando especificamente a instância `tp2-svcb-1` do serviço `svcb`. O DNS interno do Docker Compose resolve diretamente para o endereço IP dessa instância específica.
  - Este comando sempre retornará o mesmo endereço IP, que é o endereço da instância `tp2-svcb-1`, sem envolver balanceamento de carga entre as outras instâncias.

### Resumo das Diferenças

- **`nslookup svcb`**: O comando resolve o nome do serviço `svcb` e pode retornar um dos endereços IP das quatro instâncias (`tp2-svcb-1`, `tp2-svcb-2`, `tp2-svcb-3`, `tp2-svcb-4`) de forma round-robin.
- **`nslookup tp2-svcb-1`**: O comando resolve especificamente o nome da instância `tp2-svcb-1` e sempre retornará o mesmo endereço IP da instância `tp2-svcb-1`.

Isso permite verificar tanto o comportamento de balanceamento de carga (com `nslookup svcb`) quanto a resolução direta de uma instância específica (com `nslookup tp2-svcb-1`) dentro da mesma rede `svcnet` configurada no Docker Compose.

