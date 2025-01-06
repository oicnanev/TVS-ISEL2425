# TVS 2022/2023 -Teste Parcial 2 - Exemplo 2

## 1. O código abaixo implementa um serviço, a operar como daemon, para guardar e recuperar 4 bytes. No entanto, a forma como o named pipe é utilizado está fundamentalmente errada. Porquê?

```c
char data[4];
int main() {
	char req[REQ_LEN];
	umask(0111); mkfifo("/tmp/tp2-req", 0666);
	int fd = open("/tmp/tp2-req", O_RDWR);
	for (;;) {
		memset(req, 0, REQ_LEN);
		int len; do { len = read(fd, req, REQ_LEN); } while (len == 0);
		switch (req[0]) {
			case 'S': memcpy(data, &req[1], 4); write(fd, "OK\n", 3); break;
			case 'G': write(fd, data, 4); break;
			case 'Q': write(fd, "OK\n", 3); unlink("/tmp/tp2-req"); exit(0);
			default: write(fd, "ERR\n", 4); break;
		}
	}
}
```

A utilização do named pipe (FIFO) no código apresentado está fundamentalmente errada principalmente devido ao uso de `O_RDWR` ao abrir o pipe e à falta de controle adequado de clientes concorrentes. Vamos examinar essas questões em mais detalhes:

### Problema

O problema fundamental no uso do named pipe (FIFO) neste código está no modo como o descritor de arquivo do FIFO (fd) foi aberto com a flag O_RDWR. Isso cria um cenário problemático porque o processo lê e escreve no mesmo FIFO, potencialmente resultando em auto-feedback e bloqueios desnecessários, além de violar o propósito típico de um FIFO como **canal de comunicação unidirecional** entre processos.

### Razões Fundamentais do Erro

Auto-feedback (leitura da própria escrita):
- Quando o servidor escreve uma resposta no mesmo FIFO com write(fd, ...), essa resposta pode ser lida pelo próprio servidor na próxima iteração do loop, em vez de ser enviada ao cliente. Isso ocorre porque tanto leitura quanto escrita estão a ser realizadas no mesmo descritor de arquivo (fd), que é associado ao FIFO /tmp/tp2-req.

### FIFO como canal unidirecional:

Por definição, um FIFO é projetado para comunicação unidirecional entre dois processos:
- Um processo abre o FIFO para escrita.
- Outro processo abre o FIFO para leitura.

Abrir um FIFO com `O_RDWR` vai contra esse modelo. Embora tecnicamente permitido, essa prática raramente faz sentido e pode levar a comportamentos inesperados.
 - Bloqueio na leitura e escrita: O read bloqueia até que outro processo escreva no FIFO, o que é esperado. No entanto, ao abrir o FIFO com `O_RDWR`, o próprio processo pode acabar bloqueando sua escrita devido à ausência de um leitor externo.
- Potencial falha ao comunicar com múltiplos clientes: Se um cliente escreve no FIFO e o servidor responde pelo mesmo canal, o cliente pode não receber a resposta imediatamente devido à forma como os dados são geridos no buffer do FIFO. Para comunicação bidirecional com múltiplos clientes, o uso de um único FIFO para ambos os fluxos (entrada e saída) é inadequado.

### Como corrigir o problema?

**Solução 1**: Usar dois FIFOs (um para entrada e outro para saída)
Separe os canais de entrada e saída para evitar conflitos:

```c
mkfifo("/tmp/tp2-req-in", 0666);
mkfifo("/tmp/tp2-req-out", 0666);

int fd_in = open("/tmp/tp2-req-in", O_RDONLY);
int fd_out = open("/tmp/tp2-req-out", O_WRONLY);

for (;;) {
    memset(req, 0, REQ_LEN);
    int len = read(fd_in, req, REQ_LEN);
    if (len > 0) {
        switch (req[0]) {
            case 'S': memcpy(data, &req[1], 4); write(fd_out, "OK\n", 3); break;
            case 'G': write(fd_out, data, 4); break;
            case 'Q': write(fd_out, "OK\n", 3); unlink("/tmp/tp2-req-in"); unlink("/tmp/tp2-req-out"); exit(0);
            default: write(fd_out, "ERR\n", 4); break;
        }
    }
}
```

Essa abordagem separa completamente os fluxos de entrada (comandos) e saída (respostas), eliminando o risco de auto-feedback.

**Solução 2**: Comunicação baseada em cliente-servidor com FIFOs dinâmicos
Outra abordagem seria o cliente criar um FIFO temporário para receber a resposta do servidor. O fluxo seria:

- Cliente cria um FIFO único (ex.: /tmp/resp-<pid>).
- Cliente envia o nome desse FIFO junto com o comando no FIFO principal (/tmp/tp2-req).
- O servidor lê o comando, processa-o, e responde no FIFO temporário do cliente.

### Conclusão
O problema fundamental é que o FIFO foi aberto com `O_RDWR`, permitindo que o servidor leia e escreva no mesmo descritor de ficheiro. Isso contraria a funcionalidade esperada de um FIFO como canal unidirecional, introduzindo problemas como auto-feedback e bloqueios. A solução mais adequada é separar os fluxos de entrada e saída ou implementar uma comunicação cliente-servidor usando FIFOs dinâmicos.

## 2. Vários processos relacionados comunicam através de um espaço de memória partilhada. Um dos processos aloca espaço para um objeto com malloc, preenche devidamente todos os campos do objeto e publica o ponteiro para o objeto na zona de memória partilhada. Há processos a tentar consultar os campos do objeto publicado e a terminar a execução com indicação de segmentation fault. Qual é o problema?

O problema aqui é que o ponteiro que foi publicado na zona de memória partilhada pelo processo que alocou o objeto com `malloc` refere-se a um endereço de memória que só é válido no espaço de memória desse processo específico. Quando outros processos tentam acessar os campos do objeto usando esse ponteiro, eles estão tentando acessar um endereço de memória que não pertence ao seu próprio espaço de memória, resultando em um *segmentation fault*.

### Explicação Detalhada:

1. **Memória Privada vs. Memória Partilhada**:
   - Quando um processo usa `malloc` para alocar memória, o ponteiro retornado por `malloc` aponta para uma região de memória dentro do espaço de memória desse processo. Esse endereço não é válido ou acessível por outros processos.

2. **Memória Partilhada**:
   - Para compartilhar dados entre processos usando memória partilhada, o próprio objeto (ou os dados) deve ser colocado na memória partilhada, não apenas um ponteiro para a memória privada de um processo.

### Solução:

- **Alocar o Objeto na Memória Partilhada**:
  - Em vez de usar `malloc`, utilize funções de alocação de memória partilhada, como `shmget` e `shmat` em sistemas Unix. Isso garante que todos os processos participantes possam acessar a mesma região de memória.

Aqui está um exemplo simplificado de como você pode alocar e acessar um objeto na memória partilhada:

```c
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SHM_SIZE 1024  // Tamanho da memória partilhada

int main() {
    // Criar memória partilhada
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    // Anexar memória partilhada ao processo
    char *shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat");
        return 1;
    }

    // Escrever dados na memória partilhada
    strcpy(shmaddr, "Hello, World!");

    // Outro processo anexa a mesma memória partilhada
    char *shmaddr2 = (char *)shmat(shmid, NULL, 0);
    if (shmaddr2 == (char *)-1) {
        perror("shmat");
        return 1;
    }

    // Ler dados da memória partilhada
    printf("Data read from shared memory: %s\n", shmaddr2);

    // Desanexar memória partilhada
    shmdt(shmaddr);
    shmdt(shmaddr2);

    // Remover a memória partilhada
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
```

Este exemplo demonstra como um objeto (ou dados) pode ser armazenado na memória partilhada e acessado por múltiplos processos sem causar segmentation fault.

# 3. No arranque do sistema, o systemd precisa de determinar o conjunto de serviços em estado enabled. Como se determina essa informação?

No arranque do sistema, o `systemd` determina o conjunto de serviços em estado **enabled** verificando os links simbólicos (symlinks) nas diretivas de inicialização para cada alvo (target). Aqui está como isso é feito:

### Diretórios de Configuração

O `systemd` utiliza vários diretórios para armazenar configurações de serviços e links simbólicos:

1. **/etc/systemd/system/**:
   - Contém as configurações específicas do administrador do sistema e links simbólicos para unidades habilitadas.

2. **/lib/systemd/system/** e **/usr/lib/systemd/system/**:
   - Contêm as unidades fornecidas pelo sistema e distribuídas pelos pacotes de software.

### Links Simbólicos para Targets

Os links simbólicos para os serviços habilitados são armazenados nos diretórios dos alvos (`targets`). Por exemplo:

- **/etc/systemd/system/default.target.wants/**:
   - Contém links simbólicos para as unidades que devem ser iniciadas no `default.target`.
   
- **/etc/systemd/system/multi-user.target.wants/**:
   - Contém links simbólicos para as unidades que devem ser iniciadas no `multi-user.target`.

### Processo de Verificação

1. **Leitura dos Diretórios de Configuração**:
   - Durante o arranque, o `systemd` lê os diretórios mencionados acima para identificar os links simbólicos associados aos diferentes alvos (targets).

2. **Determinação dos Serviços Habilitados**:
   - O `systemd` verifica os links simbólicos presentes nos diretórios de `wants` e `requires` para determinar quais unidades (serviços) estão habilitadas para iniciar automaticamente.

3. **Início dos Serviços**:
   - Com base nos links simbólicos encontrados, o `systemd` inicia os serviços habilitados conforme definido pelos alvos do sistema.

### Exemplos de Comandos

Para listar todas as unidades habilitadas no sistema, você pode usar:

```bash
systemctl list-unit-files --state=enabled
```

Este comando mostra todas as unidades que estão configuradas para iniciar automaticamente.

### Conclusão

No arranque do sistema, o `systemd` verifica os links simbólicos nos diretórios de `wants` e `requires` dos alvos (targets) para determinar quais serviços estão habilitados e devem ser iniciados automaticamente. Isso permite uma gestão eficiente e flexível dos serviços durante a inicialização do sistema.

## 4. Na definição de ficheiro de unidade para o systemd, é possível adicionar um serviço (especificado num ficheiro .service) a um target (especificado num ficheiro .target) sem editar, direta ou indiretamente, o ficheiro .target.

## a. Indique como se pode conseguir o efeito indicado.

É possível adicionar um serviço especificado num ficheiro `.service` a um target especificado num ficheiro `.target` sem editar diretamente ou indiretamente o ficheiro `.target`, utilizando links simbólicos (symlinks). 

Aqui está como você pode conseguir isso:

### Passos para Adicionar um Serviço a um Target Usando Links Simbólicos

1. **Criação do Serviço**:
   - Certifique-se de que o ficheiro de unidade do serviço (por exemplo, `meu_servico.service`) esteja devidamente configurado e localizado em `/etc/systemd/system/` ou `/lib/systemd/system/`.

2. **Criar um Link Simbólico no Diretório de Wants do Target**:
   - Utilize o comando `ln -s` para criar um link simbólico do ficheiro `.service` no diretório `wants` do target desejado.
   - Por exemplo, se você quiser adicionar `meu_servico.service` ao target `multi-user.target`, crie um link simbólico no diretório `multi-user.target.wants`:

```bash
sudo ln -s /etc/systemd/system/meu_servico.service /etc/systemd/system/multi-user.target.wants/meu_servico.service
```

### Benefícios de Usar Links Simbólicos

- **Sem Edição Direta de Ficheiros de Target**: Não é necessário editar o ficheiro `.target` diretamente ou indiretamente.
- **Facilidade de Gestão**: É fácil adicionar ou remover serviços de targets específicos, gerenciando apenas os links simbólicos.
- **Modularidade**: Mantém a modularidade e a separação clara entre a definição dos serviços e a configuração dos targets.

### Exemplo Prático

Para adicionar `meu_servico.service` ao target `multi-user.target`, execute os seguintes comandos:

1. Verifique se o serviço está presente em `/etc/systemd/system/` ou `/lib/systemd/system/`:

```bash
ls /etc/systemd/system/meu_servico.service
```

2. Crie o link simbólico no diretório `multi-user.target.wants`:

```bash
sudo ln -s /etc/systemd/system/meu_servico.service /etc/systemd/system/multi-user.target.wants/meu_servico.service
```

Com isso, `meu_servico.service` será iniciado automaticamente sempre que o `multi-user.target` for ativado, sem a necessidade de editar o ficheiro `.target` diretamente.

## b. Indique qual o propósito para a existência desta funcionalidade.

A funcionalidade de adicionar serviços a um target usando links simbólicos (sem editar diretamente ou indiretamente os ficheiros `.target`) tem várias finalidades importantes:

### Propósitos

1. **Modularidade e Flexibilidade**:
   - Permite adicionar ou remover serviços de targets de forma modular e independente, sem precisar modificar os próprios ficheiros de target. Isso facilita a personalização e configuração dos serviços conforme necessário.

2. **Facilidade de Gestão**:
   - Simplifica a gestão dos serviços e targets. Administradores podem habilitar ou desabilitar serviços para targets específicos de maneira rápida e eficiente, apenas gerenciando links simbólicos.

3. **Separação de Responsabilidades**:
   - Mantém uma separação clara entre a definição do serviço e a configuração do target. Isso é útil para manter uma organização lógica e limpa dos ficheiros de unidade, melhorando a manutenção e clareza do sistema.

4. **Escalabilidade**:
   - Facilita a adição de novos serviços ou a reconfiguração de targets sem a necessidade de modificar vários arquivos de configuração. Isso é especialmente útil em ambientes grandes ou dinâmicos, onde as configurações podem mudar frequentemente.

5. **Redução de Erros**:
   - Minimiza o risco de erros ao evitar alterações diretas nos ficheiros de target. Links simbólicos podem ser criados e removidos sem impactar diretamente a configuração do target, reduzindo a probabilidade de falhas de configuração.

### Resumo

Em resumo, essa funcionalidade permite uma gestão mais eficiente, modular e flexível dos serviços e targets no `systemd`, melhorando a facilidade de uso, manutenção e escalabilidade dos sistemas.

## 5. Os processadores ARM v8 de 64 bits (por vezes designados por aarch64) suportam um modo de tradução de endereços com duas fases, em que um endereço virtual é primeiro traduzido para um endereço intermédio e a seguir passa por uma segunda tradução para se obter o endereço físico final. Qual é o propósito deste esquema de tradução com duas fases?

O esquema de tradução de endereços em duas fases nos processadores ARM v8 de 64 bits (aarch64) tem como principal propósito a implementação eficiente de virtualização de hardware. Esse modo de tradução é especialmente útil em ambientes onde se executam máquinas virtuais, pois oferece uma camada adicional de abstração e controle sobre a memória.

### Propósitos e Benefícios

1. **Suporte à Virtualização**:
   - A primeira fase de tradução é realizada pelo sistema operacional convidado (guest OS) e traduz um endereço virtual (VA) para um endereço físico intermediário (IPA). A segunda fase é realizada pelo hipervisor e traduz esse endereço físico intermediário para o endereço físico real (PA) na memória física.
   - Isso permite que o hipervisor mantenha controle total sobre o espaço de memória física, enquanto cada sistema operacional convidado trabalha como se estivesse diretamente em um sistema físico.

2. **Isolamento e Segurança**:
   - A tradução em duas fases melhora o isolamento entre máquinas virtuais. Cada sistema operacional convidado opera em seu próprio espaço de endereçamento virtual, que é mapeado de forma independente pelo hipervisor, prevenindo acessos não autorizados entre diferentes máquinas virtuais.
   
3. **Flexibilidade na Gerência de Memória**:
   - Proporciona uma maior flexibilidade na gerência de memória, permitindo a sobrecarga e migração de memória entre diferentes máquinas virtuais de forma eficiente. O hipervisor pode reatribuir memórias físicas reais para diferentes IPAs sem necessidade de intervenção dos sistemas operacionais convidados.

### Exemplo Prático

Imagine um cenário com duas máquinas virtuais, VM1 e VM2, executando em um mesmo host. Cada uma dessas VMs tem seu próprio endereço virtual para os processos em execução. 

- **Fase 1**: A VM1 traduz seu VA para um IPA usando seu próprio mapeamento de tabelas de página.
- **Fase 2**: O hipervisor então traduz esse IPA para um PA real na memória física do host.

Por meio desse esquema, o hipervisor pode gerenciar e alocar a memória física real de maneira transparente para as VMs.

Essa abordagem é essencial para a eficiência e segurança de sistemas virtualizados, tornando a tradução de endereços em duas fases um componente crucial da arquitetura ARM v8 de 64 bits.

# 6. Comente a seguinte afirmação:

> Um dos custos incontornáveis do sistema de contentores Docker é o de precisar de uma máquina virtual auxiliar para correr um kernel Linux, que fica em execução em simultâneo com o kernel do host, seja num sistema Windows, Mac ou Linux.

Essa afirmação não é correta em todos os contextos e precisa de algumas nuances.

### Contexto de Sistemas Diferentes

1. **Sistemas Linux**:
   - No Linux, o Docker não precisa de uma máquina virtual auxiliar para executar contêineres. Todos os contêineres compartilham o mesmo kernel do host Linux. Portanto, não há sobrecarga de manter múltiplos kernels em execução.

2. **Sistemas Windows e macOS**:
   - No Windows e no macOS, a situação é um pouco diferente. Para fornecer um ambiente compatível com Linux, Docker utiliza uma camada de virtualização leve, como o Docker Desktop, que inclui um componente chamado Docker Desktop for Windows ou Docker Desktop for Mac.
   - **Windows**: Docker pode usar uma máquina virtual baseada no Hyper-V (ou WSL 2 - Windows Subsystem for Linux 2), que executa um kernel Linux leve para fornecer compatibilidade.
   - **macOS**: Docker Desktop usa uma máquina virtual baseada no Hypervisor.framework para executar um kernel Linux leve.

### Ponto Crítico

- **Linux**: Não há necessidade de um kernel adicional. Os contêineres compartilham o kernel do host.
- **Windows/macOS**: Existe sim uma máquina virtual leve para executar o kernel Linux, proporcionando o ambiente necessário para os contêineres Linux.

### Conclusão

A afirmação é verdadeira para sistemas Windows e macOS, onde uma máquina virtual leve é necessária para executar contêineres Docker. No entanto, ela não se aplica aos sistemas Linux, onde os contêineres usam diretamente o kernel do host.

## 7. Considere o Dockerfile apresentado abaixo e dois ficheiros, package.json e app.js, com uma aplicação para Node.js

```Dockerfile
FROM ubuntu
WORKDIR /opt/isel/tp2
RUN cp * /opt/isel/tp2/
RUN apt update
RUN apt install -y npm nodejs
RUN npm install
EXPOSE 80
CMD ["node", "app.js"]
```

- A construção da imagem falha na linha com RUN cp, que se pretendia que colocasse os ficheiros da aplicação Node.js na diretoria de destino. Indique porquê e corrija.
- Modifique ainda o Dockerfile para minimizar o número de reconstruções de camadas quando algum dos ficheiros da aplicação Node.js é alterado e reduza o número total de camadas.

A construção da imagem falha na linha com `RUN cp * /opt/isel/tp2/` porque, no momento da execução desta linha, o contexto de construção do Dockerfile não inclui os ficheiros `package.json` e `app.js`. Para corrigir isso, devemos usar a instrução `COPY` para copiar esses ficheiros da máquina host para o contêiner.

Além disso, podemos modificar o Dockerfile para minimizar o número de reconstruções de camadas, especialmente quando os ficheiros da aplicação são alterados, e reduzir o número total de camadas.

### Dockerfile Corrigido e Otimizado

Aqui está a versão corrigida e otimizada do Dockerfile:

```Dockerfile
FROM ubuntu
WORKDIR /opt/isel/tp2
COPY package.json .
RUN apt update && \
    apt install -y npm nodejs && \
    npm install
COPY . .
EXPOSE 80
CMD ["node", "app.js"]
```

### Explicação das Modificações

1. **COPY package.json .**:
   - Esta linha copia o ficheiro `package.json` para o diretório de trabalho dentro do contêiner. Isso garante que a instalação das dependências do Node.js seja eficiente e que o cache do Docker possa ser reaproveitado se apenas o código da aplicação for alterado.

2. **RUN apt update && apt install -y npm nodejs && npm install**:
   - Combinei múltiplas instruções `RUN` em uma só, usando o operador `&&`. Isso reduz o número de camadas e melhora a eficiência da construção da imagem.

3. **COPY . .**:
   - Esta linha copia todos os ficheiros da aplicação (incluindo `app.js`) para o diretório de trabalho dentro do contêiner. Como essa linha é executada após a instalação das dependências do Node.js, mudanças no código da aplicação não invalidam o cache das etapas de instalação, minimizando a reconstrução das camadas.

### Benefícios

- **Redução de Camadas**: Ao combinar instruções `RUN`, reduzimos o número total de camadas na imagem.
- **Eficiência de Cache**: Separando a cópia do `package.json` e a instalação das dependências do Node.js das outras cópias, utilizamos o cache de forma mais eficiente, o que resulta em construções mais rápidas quando apenas o código da aplicação é alterado.

Isso melhora significativamente a eficiência da construção da imagem Docker e assegura que os ficheiros da aplicação sejam corretamente copiados para o contêiner.

## 8. Uma solução para docker compose, composta por múltiplos serviços, utiliza redes distintas para vários grupos de serviços dessa solução. Consequentemente, os vários contentores da solução estarão em execução com definições distintas de rede, possivelmente todas elas diferentes das definições de rede do sistema anfitrião. No entanto, todos os processos desses contentores são também processos do sistema anfitrião. Como podem coexistir no mesmo sistema operativo processos com definições de rede diferentes?

Nos sistemas que utilizam Docker Compose, múltiplos contêineres podem operar em redes distintas devido a um recurso chamado **namespaces** no kernel do Linux. Em particular, o namespace de rede (network namespace) é usado para fornecer isolamento de rede entre contêineres, permitindo que processos dentro de contêineres diferentes tenham suas próprias pilhas de rede independentes, incluindo interfaces de rede, roteadores, tabelas de rotas e firewalls.

### Como os Network Namespaces Funcionam

1. **Isolamento de Rede**:
   - Cada contêiner Docker é executado em seu próprio namespace de rede. Isso significa que ele pode ter seu próprio conjunto de interfaces de rede, tabelas de roteamento, regras de firewall, etc. Essas definições de rede são independentes das definições de rede do host e de outros contêineres.

2. **Interfaces Virtuais e Pontes (Bridges)**:
   - Docker cria interfaces de rede virtuais e pontes para conectar contêineres a diferentes redes. Por exemplo, quando você define várias redes em um arquivo `docker-compose.yml`, o Docker cria pontes de rede (bridge networks) para cada rede definida e conecta os contêineres apropriados a essas pontes.
   - Cada contêiner se conecta a essas pontes através de interfaces virtuais, que são independentes do namespace de rede do host.

3. **Rotas de Rede**:
   - As rotas de rede dentro de um namespace de rede são específicas a esse namespace. Portanto, cada contêiner pode ter suas próprias rotas de rede que não interferem com as rotas de outros contêineres ou do host.

### Exemplo Prático

Considere um arquivo `docker-compose.yml` com várias redes:

```yaml
version: '3'
services:
  app1:
    image: myapp1
    networks:
      - frontend
  app2:
    image: myapp2
    networks:
      - backend
  app3:
    image: myapp3
    networks:
      - frontend
      - backend

networks:
  frontend:
  backend:
```

Neste exemplo:
- `app1` está na rede `frontend`.
- `app2` está na rede `backend`.
- `app3` está em ambas as redes `frontend` e `backend`.

### Coexistência no Sistema Operacional

Mesmo que todos os processos dos contêineres sejam tecnicamente processos do sistema anfitrião, eles coexistem com diferentes definições de rede devido ao isolamento proporcionado pelos namespaces de rede. Cada contêiner funciona como se estivesse em seu próprio ambiente de rede isolado, graças ao uso de namespaces.

Essa abordagem permite a coexistência de processos com definições de rede diferentes no mesmo sistema operativo sem interferência, proporcionando um ambiente seguro e eficiente para a execução de contêineres Docker.

