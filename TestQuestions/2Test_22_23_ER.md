# TVS, Inv 2022/2023 - Época de Recurso, Teste Parcial 2

## 6. Considere o código, apresentado abaixo, de um programa cliente que comunica com um servidor que atende pedidos no socket de domínio Unix localizado em /run/tp2-server.sock Não contando com a falta das diretivas de #include nem com a falta do tratamento de erros, indique os erros notórios no código apresentado, justificando devidamente porque motivo são erros.

```c
struct sockaddr_un cli_addr;
int main(int argc, char * argv[]) {
	int conn_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	cli_addr.sun_family = AF_UNIX;
	sprintf(cli_addr.sun_path, "/run/tp2-client-%d.sock", getpid());
	cli_addr.sun_port = 5600;
	connect(conn_fd, (struct sockaddr *)&cli_addr, sizeof (cli_addr));
	/* utilização de conn_fd */
	close(conn_fd);
	return 0;
}
```

O código do programa cliente apresentado contém vários erros notórios.

### Erros e Justificações

1. **Uso de `cli_addr.sun_port`**:
   - **Erro**: A estrutura `sockaddr_un` não possui o campo `sun_port`.
   - **Justificação**: O campo `sun_port` é utilizado em sockets de domínio de Internet (`sockaddr_in`) para especificar o número da porta. Para sockets de domínio Unix (`sockaddr_un`), esse campo não existe e, portanto, não deve ser utilizado.

2. **Conectar-se ao Endereço do Cliente**:
   - **Erro**: O código tenta conectar-se ao endereço do cliente (`cli_addr`) em vez do endereço do servidor.
   - **Justificação**: O endereço do cliente (`/run/tp2-client-%d.sock`) é definido corretamente, mas o `connect()` deve ser usado para conectar-se ao endereço do servidor (`/run/tp2-server.sock`).

3. **Falta de `bind()` para o Socket do Cliente**:
   - **Erro**: O cliente não faz o `bind()` do seu socket ao seu próprio endereço (`/run/tp2-client-%d.sock`).
   - **Justificação**: Antes de usar `connect()`, o cliente deve associar seu socket a um endereço local usando `bind()`. Sem isso, o cliente não possui um endereço definido para a comunicação.

### Correção do Código

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

struct sockaddr_un cli_addr, srv_addr;

int main(int argc, char * argv[]) {
    int conn_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    // Configurar o endereço do cliente
    cli_addr.sun_family = AF_UNIX;
    sprintf(cli_addr.sun_path, "/run/tp2-client-%d.sock", getpid());
    unlink(cli_addr.sun_path); // Remover o socket anterior, se existir

    // Fazer o bind do socket do cliente
    bind(conn_fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

    // Configurar o endereço do servidor
    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, "/run/tp2-server.sock");

    // Conectar ao servidor
    connect(conn_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

    /* utilização de conn_fd */

    // Fechar o socket
    close(conn_fd);
    unlink(cli_addr.sun_path); // Remover o socket do cliente

    return 0;
}
```

### Explicação da Correção

- **Remoção de `cli_addr.sun_port`**: O campo `sun_port` foi removido.
- **Adição de `srv_addr`**: Um novo `sockaddr_un` (`srv_addr`) foi adicionado para armazenar o endereço do servidor.
- **Uso de `bind()`**: O socket do cliente é associado ao seu próprio endereço local usando `bind()`.
- **Conexão ao Servidor**: A função `connect()` agora utiliza o endereço do servidor (`srv_addr`).

Essas correções garantem que o cliente se comunique corretamente com o servidor através do socket de domínio Unix.

## 7. A especificação de um serviço systemd pode incluir opcionalmente a diretiva ExecStop, geralmente omissa, para especificar um comando explícito para terminar o serviço. No entanto, mesmo sem esta diretiva, o comando systemctl stop nome_do_serviço consegue geralmente terminá-lo. Como?

Mesmo sem a diretiva `ExecStop`, o comando `systemctl stop nome_do_serviço` geralmente consegue terminar um serviço devido ao comportamento padrão do `systemd`. Aqui está como isso funciona:

### Comportamento Padrão do `systemd`

1. **Envio do Sinal SIGTERM**:
   - Quando o comando `systemctl stop nome_do_serviço` é executado, o `systemd` envia automaticamente o sinal `SIGTERM` ao processo principal do serviço. O `SIGTERM` é um sinal de terminação que indica ao processo que ele deve encerrar de forma graciosa, permitindo que ele libere recursos e finalize suas operações atuais.

2. **Aguardar o Encerramento**:
   - Após enviar o `SIGTERM`, o `systemd` aguarda um período configurável para que o processo termine. Esse período é definido pela diretiva `TimeoutStopSec` (com um valor padrão de 90 segundos). Se o processo não terminar dentro desse período, o `systemd` toma medidas adicionais.

3. **Envio do Sinal SIGKILL**:
   - Se o processo não terminar após o `SIGTERM` e o tempo de espera expirar, o `systemd` envia um sinal `SIGKILL` para forçar a terminação imediata do processo. O `SIGKILL` não pode ser ignorado pelo processo e termina o processo imediatamente.

### Quando a Diretiva `ExecStop` é Necessária

A diretiva `ExecStop` é necessária quando o encerramento do serviço requer comandos ou procedimentos específicos que não são cobertos pelo simples envio do sinal `SIGTERM`. Por exemplo:

- Se o serviço precisar executar uma sequência de comandos de limpeza antes de terminar.
- Se o serviço utilizar processos filhos ou subprocessos que também precisam ser terminados explicitamente.

### Exemplo de Comportamento Padrão

Considere um serviço simples que não inclui a diretiva `ExecStop`:

```ini
[Unit]
Description=Meu Serviço Simples

[Service]
ExecStart=/usr/bin/minha_aplicacao

[Install]
WantedBy=multi-user.target
```

Mesmo sem a diretiva `ExecStop`, o comando `systemctl stop` ainda pode encerrar o serviço enviando o `SIGTERM` para o processo principal.

### Exemplo de Uso de ExecStop

Se o serviço precisar de um comando específico para encerrar, você pode adicionar a diretiva `ExecStop`:

```ini
[Unit]
Description=Meu Serviço Complexo

[Service]
ExecStart=/usr/bin/minha_aplicacao
ExecStop=/usr/bin/minha_aplicacao --encerrar-graciosamente

[Install]
WantedBy=multi-user.target
```

Nesse caso, o `systemd` executará o comando especificado em `ExecStop` para encerrar o serviço de forma adequada.

Dessa forma, o `systemd` garante o encerramento controlado e ordenado dos serviços, mesmo na ausência da diretiva `ExecStop`.

## 8. Na secção de instalação do ficheiro que descreve um serviço em systemd, explique porque razão é preferível usar WantedBy, em vez de RequiredBy, para associar esse serviço a um target.

### WantedBy vs RequiredBy

Ao configurar um serviço no `systemd`, especialmente na secção [Install], é geralmente preferível usar a diretiva `WantedBy` em vez de `RequiredBy` para associar o serviço a um target. Aqui estão as razões:

#### 1. **Flexibilidade e Tolerância a Falhas**

- **WantedBy**:
  - Indica que o serviço deve ser iniciado quando o target especificado é ativado, mas não é essencial para o target. Se o serviço falhar ao iniciar, o target ainda pode continuar funcionando. Isso oferece maior flexibilidade e tolerância a falhas no sistema, pois não impede a ativação do target.

- **RequiredBy**:
  - Indica que o serviço é essencial para o target e deve ser iniciado com o target. Se o serviço falhar ao iniciar, o target também falhará. Isso pode ser problemático, pois impede que o target seja ativado se houver qualquer problema com o serviço, reduzindo a resiliência do sistema.

#### 2. **Gestão de Dependências**

- **WantedBy**:
  - É usado para declarar dependências desejáveis. Isso significa que o target funciona melhor com o serviço em execução, mas não depende exclusivamente dele. Essa abordagem é benéfica para serviços que melhoram a funcionalidade do target, mas cuja ausência não causa falhas críticas.

- **RequiredBy**:
  - É usado para declarar dependências obrigatórias. Esse nível de dependência deve ser reservado para serviços essenciais ao target, cuja ausência impediria o funcionamento adequado do target. Usar `RequiredBy` de forma excessiva pode aumentar a complexidade e o risco de falhas durante a ativação do target.

### Exemplo Prático

Considere um serviço `tp2.service` que deve ser ativado junto com o target `multi-user.target`:

```ini
[Unit]
Description=Meu Serviço TP2

[Service]
ExecStart=/usr/local/bin/tp2

[Install]
WantedBy=multi-user.target
```

Neste exemplo, `tp2.service` será iniciado quando `multi-user.target` for ativado, mas o `multi-user.target` não depende criticamente de `tp2.service`. Isso torna o sistema mais robusto, pois outros serviços no `multi-user.target` ainda podem ser iniciados mesmo que `tp2.service` encontre problemas.

### Conclusão

Em resumo, **`WantedBy`** é preferível a **`RequiredBy`** na maioria dos casos porque oferece maior flexibilidade, tolerância a falhas e uma gestão de dependências mais suave, sem comprometer a ativação dos targets. Essa abordagem equilibra a necessidade de iniciar serviços com a resiliência geral do sistema.

## 9. O código fonte do kernel Linux pode ser compilado com diversas opções de configuração. Uma dessas opções está documentada com o seguinte texto: 

> «This changes the kernel so it can modify itself when it is run under* a hypervisor, potentially improving performance significantly over full virtualization. However, when run without a hypervisor the kernel is theoretically slower and slightly larger.» in https://github.com/torvalds/linux/blob/master/arch/x86/Kconfig
> * neste contexto significa “sobre” ou “debaixo do controlo de”

Que nome se dá ao tipo de solução de virtualização que está implícito neste texto? Na sua resposta, refira as partes relevantes do texto acima que permitem identificar o tipo de solução/otimização em causa.

O tipo de solução de virtualização implícito no texto é chamado **paravirtualização**.

### Partes Relevantes do Texto que Identificam a Solução:

1. **"The kernel so it can modify itself when it is run under* a hypervisor"**:
   - Esta frase indica que o kernel está ciente do hipervisor e pode ajustar seu comportamento para aproveitar as características específicas do ambiente virtualizado, algo típico da paravirtualização. Na paravirtualização, o sistema operacional convidado (guest OS) é modificado para ser consciente do hipervisor e se comunicar diretamente com ele.

2. **"Potentially improving performance significantly over full virtualization"**:
   - A menção à melhoria de desempenho em comparação com a virtualização completa (full virtualization) sugere que a abordagem permite maior eficiência. Na paravirtualização, a eliminação da necessidade de emular completamente o hardware subjacente proporciona um desempenho mais próximo ao do hardware real.

3. **"When run without a hypervisor the kernel is theoretically slower and slightly larger"**:
   - Esta frase sugere que as modificações no kernel introduzem alguma sobrecarga quando o sistema não está em um ambiente virtualizado, o que é coerente com a paravirtualização. Em sistemas paravirtualizados, o kernel modificado tem instruções adicionais para lidar com o hipervisor, o que pode resultar em um kernel ligeiramente maior e um pouco mais lento em hardware físico.

### Conclusão

Portanto, a descrição no texto corresponde às características da paravirtualização, onde o kernel do sistema operacional convidado é modificado para interagir eficientemente com o hipervisor, melhorando o desempenho em um ambiente virtualizado.

## 10. Descreva sucintamente 5 a 7 passos que considera relevantes na compreensão do que acontece num sistema Linux desde que se executa o comando docker run -it ubuntu:latest até que o respetivo contentor está em execução. NOTA: Existem múltiplas respostas válidas.


### 1. **Download da Imagem**
   - **Verificação Local**: O Docker verifica se a imagem `ubuntu:latest` já está presente localmente.
   - **Download do Repositório**: Se a imagem não estiver localmente, o Docker faz o download da imagem `ubuntu:latest` do Docker Hub.

### 2. **Criação do Contentor**
   - **Isolamento de Namespace**: Docker cria um contentor isolado usando namespaces para isolar processos, rede, mount points, etc.
   - **Configuração do Sistema de Ficheiros**: Um sistema de ficheiros em camadas é configurado usando o overlayFS, combinando a imagem base com as mudanças específicas do contentor.

### 3. **Configuração de Recursos**
   - **Cgroups**: Docker configura grupos de controle (cgroups) para limitar e isolar recursos (CPU, memória, I/O) do contentor.
   - **Configuração de Rede**: Docker cria interfaces de rede virtuais e configura a rede do contentor (tipicamente `bridge`, `host` ou `none`).

### 4. **Execução do Comando**
   - **Processo de Inicialização**: Docker cria o processo inicial (PID 1) no contentor e executa o comando especificado (`/bin/bash` no caso de `-it`).
   - **Interatividade**: Devido às opções `-it`, o Docker conecta o terminal do usuário ao terminal do contentor, permitindo entrada e saída interativas.

### 5. **Monitoramento e Gerenciamento**
   - **Docker Daemon**: O daemon do Docker (`dockerd`) monitora o contentor, gerenciando o ciclo de vida dete e recursos associados.
   - **Logs e Status**: O Docker recolhe logs do contentor e permite acesso ao status e métricas através de comandos Docker (`docker ps`, `docker logs`).

## 11. Um ficheiro docker-compose.yml, para especificação de uma solução composta com o nome tp2, contém, entre outros elementos, a especificação de um serviço com o nome svc, associado a uma rede tp2net de tipo bridge. Ao levantar a solução com up, este serviço poderá ter associado mais do que 1 contentor? Se sim, como? Se não, o que poderia ser feito para que isso fosse possível? Em qualquer caso, que nome ou nomes podem ser usados na rede tp2net para chegar a esse(s) contentor(es)?

Sim, é possível ter mais de um contêiner associado ao serviço `svc` na rede `tp2net`. Para conseguir isso, você pode utilizar a diretiva `scale` no Docker Compose. Aqui está como você pode fazer isso:

### Escalando o Serviço

Para escalar o serviço `svc` e associar múltiplos contêineres a ele, você pode usar a opção `scale` no comando `docker-compose up` ou definir o número de réplicas diretamente no arquivo `docker-compose.yml`.

#### Usando o Comando `docker-compose up` com `scale`

```bash
docker-compose up --scale svc=3
```

Este comando inicia três instâncias do serviço `svc`.

#### Definindo Réplicas no `docker-compose.yml`

Você também pode definir o número de réplicas no arquivo `docker-compose.yml` da seguinte forma:

```yaml
version: '3'
services:
  svc:
    image: sua-imagem
    networks:
      - tp2net
    deploy:
      replicas: 3

networks:
  tp2net:
    driver: bridge
```

Neste exemplo, o campo `deploy: replicas: 3` define que três instâncias do serviço `svc` devem ser iniciadas.

### Nomes na Rede `tp2net`

- **Resolução de Serviço**: Quando você escala um serviço, o Docker Compose cria contêineres nomeados sequencialmente. Por exemplo, se você tiver três instâncias do serviço `svc`, elas serão nomeadas `tp2_svc_1`, `tp2_svc_2` e `tp2_svc_3`.

- **Resolução de DNS**: Na rede `tp2net`, você pode acessar qualquer uma das instâncias usando o nome do serviço `svc`. O Docker Compose utiliza um mecanismo de balanceamento de carga round-robin para distribuir as solicitações entre as diferentes instâncias do serviço. Além disso, você pode acessar cada instância diretamente usando seus nomes específicos:

  - **Nome do Serviço**: `svc`
    - Utilizado para balanceamento de carga entre as instâncias. Por exemplo, `ping svc`.
  
  - **Nomes Específicos das Instâncias**: `tp2_svc_1`, `tp2_svc_2`, `tp2_svc_3`
    - Utilizado para acessar instâncias específicas. Por exemplo, `ping tp2_svc_1`.

### Resumo

- Para ter mais de um contêiner associado ao serviço `svc`, use a opção `scale` no comando `docker-compose up` ou defina `deploy: replicas` no arquivo `docker-compose.yml`.
- Na rede `tp2net`, os nomes `svc`, `tp2_svc_1`, `tp2_svc_2`, e `tp2_svc_3` podem ser usados para chegar às diferentes instâncias do contêiner.

Isso permite que você gerencie e acesse múltiplas instâncias do serviço `svc` de maneira eficiente dentro da solução composta.

## 12. Considere o Dockerfile apresentado em baixo, os ficheiros package.json e app.js, com uma aplicação para Node.js, o ficheiro README.md, e uma pasta images com ficheiros de suporte à interface de utilizador da aplicação. Pretende-se que no sistema de ficheiros das instâncias da imagem todos os ficheiros e pastas abaixo de /home/node/webapp tenham como dono o utilizador node. Considera-se que há grande probabilidade de alterar o ficheiro app.js, o porto da aplicação e os ficheiros da pasta images. Pretende-se ainda, prioritariamente, otimizar a reutilização da cache de imagens intermédias e, dentro do possível, minimizar o número de camadas de overlay.

```Dokerfile
FROM node:alpine
WORKDIR /home/node/webapp
COPY README.md .
COPY images/* ./images
USER node
ENV PORT=80
COPY package.json app.js .
RUN chown -R node.node /home/node
RUN npm install
EXPOSE $PORT
CMD ["node", "app,js"]
```

- Identifique problemas no Dockerfile que contrariam os objetivos enunciados atrás.
- Corrija o Dockerfile de modo a concretizar os objetivos enunciados.

### Problemas no Dockerfile Original

1. **Instruções COPY Separadas**:
   - Várias instruções `COPY` são usadas para copiar arquivos individualmente, criando várias camadas de overlay.
  
2. **Alteração do Proprietário Após a Cópia de Arquivos**:
   - A instrução `chown -R node.node /home/node` é executada após a cópia de arquivos, criando uma camada adicional desnecessária.

3. **Execução do Comando chown e Instalação de Dependências Separadamente**:
   - Duas instruções `RUN` são usadas separadamente para `chown` e `npm install`, criando camadas adicionais.

4. **Ordem Subótima das Instruções COPY**:
   - Arquivos que mudam frequentemente, como `app.js` e os arquivos em `images`, são copiados após a instalação das dependências, o que invalida o cache das dependências ao alterar esses arquivos.

5. **Erro de Digitação no CMD**:
   - Há um erro de digitação em `CMD ["node", "app,js"]`, onde deve ser `app.js`.

### Dockerfile Corrigido e Otimizado

Para otimizar a reutilização da cache e minimizar o número de camadas de overlay, podemos reorganizar o Dockerfile como segue:

```Dockerfile
FROM node:alpine

WORKDIR /home/node/webapp

# Copiar package.json e instalar dependências antes de copiar os arquivos que mudam frequentemente
COPY package.json .

# Executar npm install e chown numa única instrução RUN
RUN npm install && chown -R node:node /home/node

# Copiar os arquivos que mudam frequentemente
COPY app.js .
COPY README.md .
COPY images ./images

# Definir o usuário node
USER node

# Definir a variável de ambiente PORT
ENV PORT=80

# Expor a porta
EXPOSE $PORT

# Definir o comando de inicialização
CMD ["node", "app.js"]
```

### Explicação das Correções

1. **Reordenação das Instruções COPY**:
   - `package.json` é copiado antes de `app.js` e `images`, permitindo que o cache da instalação do npm seja reutilizado sempre que apenas `app.js` ou `images` forem modificados.

2. **Combinação de Instruções RUN**:
   - A instrução `npm install` e `chown` são combinadas em uma única instrução `RUN`, reduzindo o número de camadas.

3. **Correção do CMD**:
   - Corrigido o erro de digitação em `CMD ["node", "app.js"]`.

### Camadas de Overlay Criadas no Dockerfile Corrigido

1. **FROM node:alpine**:
   - Utiliza a imagem base `node:alpine`.

2. **WORKDIR /home/node/webapp**:
   - Define o diretório de trabalho. Cria uma nova camada.

3. **COPY package.json .**:
   - Copia `package.json` para o diretório de trabalho. Cria uma nova camada.

4. **RUN npm install && chown -R node:node /home/node**:
   - Instala as dependências do npm e altera a propriedade dos arquivos. Cria uma nova camada.

5. **COPY app.js README.md images ./images**:
   - Copia `app.js`, `README.md` e os arquivos da pasta `images` para o diretório de trabalho. Cria uma nova camada.

6. **USER node**:
   - Muda o usuário para `node`. Cria uma nova camada.

7. **ENV PORT=80**:
   - Define a variável de ambiente `PORT`. Cria uma nova camada.

8. **EXPOSE $PORT**:
   - Expõe a porta. Cria uma nova camada.

9. **CMD ["node", "app.js"]**:
   - Define o comando para iniciar a aplicação. Cria uma nova camada.

Estas alterações ajudam a otimizar a reutilização da cache e minimizam o número de camadas de overlay, resultando em uma construção de imagem mais eficiente e rápida. 
