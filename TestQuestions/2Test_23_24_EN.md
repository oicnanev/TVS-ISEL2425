# TVS, Inverno de 2023/2024, Época Normal — Teste Parcial #2 — Teste Global, parte 2

## 1. Num sistema Linux é possível utilizar sockets de domínio Unix (Unix Domain Sockets). ndique qual é a diferença fundamental entre este tipo de sockets e os sockets de Internet, incluindo qual a sua principal vantagem e a principal limitação.

### Diferença Fundamental
- **Unix Domain Sockets**: São utilizados para a comunicação entre processos (IPC) que residem no mesmo host (máquina). Em vez de utilizar endereços IP e números de porta como os sockets de Internet, eles utilizam caminhos do sistema de ficheiros para identificar os endpoints de comunicação.
- **Sockets de Internet**: São utilizados para a comunicação entre processos que podem estar em diferentes hosts. Eles utilizam endereços IP e números de porta para identificar os endpoints de comunicação na rede.

### Principal Vantagem dos Unix Domain Sockets
- **Desempenho**: Por serem utilizados para comunicação local dentro do mesmo sistema, os Unix Domain Sockets têm menos overhead e, portanto, oferecem um desempenho melhor em comparação com os sockets de Internet. Eles são geralmente mais rápidos e eficientes para a comunicação entre processos no mesmo host.

### Principal Limitação dos Unix Domain Sockets
- **Alcance Limitado**: A principal limitação dos Unix Domain Sockets é que eles não podem ser utilizados para comunicação entre diferentes hosts. Eles só funcionam para a comunicação dentro do mesmo sistema operativo. Para comunicação entre hosts, é necessário utilizar sockets de Internet.

## Qual é a forma dos endereços que são usados com sockets de domínio Unix e como diferem dos endereços de sockets de Internet

### Endereços de Sockets de Domínio Unix (Unix Domain Sockets)
Os endereços utilizados com os sockets de domínio Unix são caminhos no sistema de ficheiros. Estes caminhos apontam para entradas do tipo *socket* no sistema de ficheiros, permitindo que os processos se comuniquem entre si através destes pontos de acesso. Aqui está um exemplo de endereço de um socket de domínio Unix:
```
/tmp/socket_unix
```
Este caminho `/tmp/socket_unix` seria utilizado pelos processos para se conectarem ao socket.

### Endereços de Sockets de Internet
Os endereços utilizados com os sockets de Internet são compostos por endereços IP e números de porta. Estes endereços permitem que processos em diferentes máquinas se comuniquem através da rede. Aqui está um exemplo de endereço de um socket de Internet:
```
192.168.1.10:8080
```
Neste caso, `192.168.1.10` é o endereço IP e `8080` é o número da porta utilizada para a comunicação.

### Diferenças Fundamentais
- **Formato**:
  - **Unix Domain Sockets**: Utilizam caminhos de sistema de ficheiros, como `/tmp/socket_unix`.
  - **Sockets de Internet**: Utilizam uma combinação de endereços IP e números de porta, como `192.168.1.10:8080`.

- **Utilização**:
  - **Unix Domain Sockets**: São utilizados para comunicação entre processos no mesmo sistema.
  - **Sockets de Internet**: São utilizados para comunicação entre processos em diferentes sistemas.

Esta diferenciação fundamental nos endereços reflete a distinta finalidade e alcance destes dois tipos de sockets. 

## 2. Alguns processos ativos num sistema Linux correspondem a serviços a operar como daemons. Indique a consequência dessa situação para os canais de standard output e de standard error desses processos e que mecanismo deve incluir um gestor de serviços avançado (como, por exemplo, o systemd) para lidar com os dados que possam surgir nesses canais.

Quando um processo opera como daemon num sistema Linux, ele geralmente é dissociado do terminal onde foi iniciado. Isso significa que os seus canais de **standard output** (stdout) e **standard error** (stderr) não estão ligados ao terminal padrão. Sem um mecanismo para redirecionar esses dados, qualquer output gerado pelo daemon seria perdido.

### Consequências para Stdout e Stderr
- **Standard Output (stdout)**: As mensagens de saída normal do processo, que seriam normalmente exibidas no terminal, não são visíveis.
- **Standard Error (stderr)**: Mensagens de erro que seriam exibidas no terminal também não são visíveis.

### Mecanismos no Systemd
O systemd, um gestor de serviços avançado, inclui mecanismos para lidar com esses dados:

1. **Redirecionamento de Logs**:
   - **Journald**: systemd integra-se com o `journald`, um sistema de logging que coleta e armazena logs de todos os serviços geridos pelo systemd. O stdout e stderr dos daemons são redirecionados para o `journald`, garantindo que qualquer output é capturado e pode ser analisado posteriormente.
   - **Configuração Unit File**: Nos ficheiros de configuração das unidades de serviço (unit files) do systemd, pode-se especificar como tratar stdout e stderr usando as opções `StandardOutput` e `StandardError`. Por exemplo, podem ser redirecionados para o `journal`, um ficheiro específico, ou mesmo para o `syslog`.

2. **Configurações Específicas**:
   - **StandardOutput**: Pode ser configurado como `journal`, `syslog`, `kmsg`, `console`, ou um ficheiro específico.
   - **StandardError**: Similar ao `StandardOutput`, pode ser configurado para redirecionar erros para diferentes destinos.

Exemplo de configuração em um unit file:
```ini
[Service]
ExecStart=/usr/bin/meu_daemon
StandardOutput=journal
StandardError=journal
```

Estas configurações garantem que toda a saída e erros dos daemons são capturados e podem ser facilmente acessados e revisados através das ferramentas do systemd, como o `journalctl`.

## 3. Num sistema Linux com systemd, o ficheiro /etc/systemd/system/tp2.socket, na sua secção [Install] tem a linha WantedBy=sockets.target . Para que serve a secção Install, porquê WantedBy e o que representa sockets.target ?

Excelente pergunta! Vamos detalhar cada um desses componentes no contexto do systemd em Linux.

### Secção [Install]
A secção `[Install]` num ficheiro de unidade (unit file) do systemd define informações específicas sobre como e quando a unidade deve ser instalada ou ativada. Essencialmente, esta secção especifica as condições sob as quais a unidade deve ser iniciada automaticamente durante o processo de inicialização ou ativação do sistema.

### WantedBy
A linha `WantedBy=sockets.target` na secção `[Install]` é crucial para a ativação da unidade. Ela indica ao systemd que a unidade `tp2.socket` deve ser iniciada automaticamente sempre que o `sockets.target` for ativado.

- **WantedBy**: Esta configuração cria um `want` (desejo) do target especificado para a unidade atual. Em termos simples, adiciona a unidade à lista de dependências do target especificado. Quando o target é ativado, todas as unidades listadas em seu `WantedBy` também são ativadas.

### sockets.target
- **sockets.target**: Este é um target padrão do systemd que agrupa todos os sockets que devem ser ativados quando o sistema inicia. É um ponto de sincronização para todos os sockets de sistema, garantindo que todos os serviços de socket relevantes estão preparados antes de qualquer tentativa de conexão.

### Exemplo de Configuração do tp2.socket
Aqui está um exemplo simplificado de como pode ser o ficheiro `/etc/systemd/system/tp2.socket` com a secção `[Install]` configurada:
```ini
[Unit]
Description=Socket for TP2 Service

[Socket]
ListenStream=/run/tp2.sock

[Install]
WantedBy=sockets.target
```

### Resumo
- **[Install]**: Define quando e como a unidade deve ser ativada.
- **WantedBy=sockets.target**: Especifica que a unidade deve ser ativada quando o target `sockets.target` é ativado.
- **sockets.target**: Um target que agrupa todos os sockets que devem ser preparados ao iniciar o sistema.

Com estas configurações, o serviço `tp2.socket` será automaticamente iniciado sempre que o `sockets.target` for ativado, garantindo que o sistema está pronto para lidar com conexões de socket.

## 4. Os hipervisores são tipicamente classificados como sendo de Tipo 1 ou de Tipo 2. Um destes tipos, pelas suas características, é geralmente mais eficiente do que o outro na gestão e consumo de recursos computacionais da máquina base. Qual destes tipos tende a ser mais eficiente, porquê e porque motivo(s), apesar disso, existem os dois tipos de hipervisores, por vezes até vindos da mesma empresa?

### Tipos de Hipervisores

#### Hipervisores Tipo 1 (Bare-Metal)
- **Mais eficiente**: Os hipervisores Tipo 1 são geralmente mais eficientes na gestão e consumo de recursos computacionais.
- **Porquê**: São instalados diretamente sobre o hardware físico do host, eliminando a necessidade de um sistema operativo subjacente. Isto permite que o hipervisor interaja diretamente com o hardware, reduzindo a sobrecarga e aumentando a performance. Acesso direto ao hardware também resulta em melhor alocação de recursos e menores latências.
- **Exemplo**: VMware ESXi, Microsoft Hyper-V (quando utilizado como bare-metal), Xen.

#### Hipervisores Tipo 2 (Hosted)
- **Porquê menos eficiente**: Os hipervisores Tipo 2 são instalados sobre um sistema operativo já existente. Isto significa que há uma camada adicional entre o hardware e o hipervisor, o que introduz overhead e pode reduzir a eficiência e performance. Dependem do sistema operativo subjacente para acesso ao hardware e gestão de recursos.
- **Exemplo**: VMware Workstation, VirtualBox, Microsoft Hyper-V (quando utilizado como aplicação sobre Windows).

### Por que existem ambos os tipos?

1. **Flexibilidade e Facilidade de Uso**:
   - Os hipervisores Tipo 2 são mais fáceis de instalar e utilizar em ambientes de teste e desenvolvimento, onde a eficiência máxima não é um fator crítico. Podem ser instalados como qualquer outra aplicação, tornando-os acessíveis para uma variedade de utilizadores, incluindo desenvolvedores e entusiastas.

2. **Casos de Uso Específicos**:
   - Empresas podem preferir hipervisores Tipo 1 para servidores e centros de dados, onde a eficiência e a performance são cruciais. 
   - Para uso em desktops e laptops, onde a flexibilidade e a conveniência são mais importantes, hipervisores Tipo 2 são uma escolha natural.

3. **Compatibilidade**:
   - Hipervisores Tipo 2 podem ser utilizados em sistemas operativos já existentes sem necessidade de reconfigurar o hardware ou criar ambientes separados. Isto facilita a integração e o uso em sistemas já em produção.

4. **Custo**:
   - A implantação de hipervisores Tipo 1 pode ser mais cara devido aos requisitos de hardware e configuração. Hipervisores Tipo 2 são mais acessíveis para usuários individuais ou pequenas empresas que precisam de virtualização para tarefas específicas.

### Conclusão
Ambos os tipos de hipervisores têm suas próprias vantagens e desvantagens. A escolha entre eles depende das necessidades específicas do usuário e do ambiente. A eficiência dos hipervisores Tipo 1 os torna ideais para grandes implementações de servidores, enquanto a flexibilidade dos hipervisores Tipo 2 os torna valiosos para ambientes de desenvolvimento, teste e uso pessoal.

## 5. Num sistema Linux com o serviço docker ativo, estão em execução dois contentores baseados na imagem node:current-alpine3.18 e um baseado na imagem php:8.2-alpine3.18. Nos respectivos Dockerfile, ambas as imagens começam com a linha FROM alpine:3.18 . Havendo neste cenário duas imagens e três contentores, justifique quantas cópias do sistema de ficheiros definido pela imagem base alpine:3.18 deverão existir no disco do sistema host?

Em um cenário Docker, as imagens são construídas em camadas, e cada camada é armazenada de forma única no disco do sistema host. Portanto, mesmo que várias imagens usem a mesma base, as camadas comuns são compartilhadas.

### Análise do Cenário:
1. **Imagens Utilizadas:**
   - `node:current-alpine3.18`
   - `php:8.2-alpine3.18`
2. **Base Comum:**
   - Ambas as imagens começam com `FROM alpine:3.18`

### Justificação:
- **Imagens Docker**: A imagem `alpine:3.18` será baixada uma vez e armazenada como uma camada no sistema de arquivos do Docker. Essa camada será compartilhada por ambas as imagens `node:current-alpine3.18` e `php:8.2-alpine3.18`.
- **Contentores**: Os três contentores baseados em `node:current-alpine3.18` e `php:8.2-alpine3.18` usarão a mesma camada base `alpine:3.18`.

### Número de Cópias no Disco do Sistema Host:
- Haverá **apenas uma cópia** do sistema de ficheiros definido pela imagem base `alpine:3.18` no disco do sistema host. Essa cópia será compartilhada pelas camadas das imagens `node:current-alpine3.18` e `php:8.2-alpine3.18`, assim como pelos contentores em execução.

Este compartilhamento de camadas é um dos principais benefícios do Docker, pois economiza espaço em disco e permite um gerenciamento mais eficiente das imagens e contentores.

## 6.  Considere o Dockerfile apresentado e três ficheiros, presentes na subdiretoria src: package.json e app.js, com uma aplicação para Node.js, e um README.md

```Dockerfile
FROM node:alpine
WORKDIR /home/node/app
COPY src/ ./
RUN npm install
WORKDIR /home/node
RUN chown -R node.node .
USER node
EXPOSE 80
WORKDIR /home/node/app
CMD ["node", "app.js"]
```

## 6.1 Indique que camadas (não-vazias) do sistema de ficheiros são criadas sobre a imagem base node:alpine

Aqui estão as camadas (não-vazias) do sistema de ficheiros que são criadas sobre a imagem base `node:alpine` no seu Dockerfile:

1. **FROM node:alpine**
   - Esta linha utiliza a imagem base `node:alpine`.

2. **WORKDIR /home/node/app**
   - Define o diretório de trabalho para `/home/node/app`. Esta instrução não cria uma camada de sistema de ficheiros, mas configura o contexto para os comandos subsequentes.

3. **COPY src/ ./**
   - Copia os ficheiros da subdiretoria `src` no contexto do build para o diretório de trabalho atual no container (`/home/node/app`). Esta ação cria uma camada que contém:
     - `package.json`
     - `app.js`
     - `README.md`

4. **RUN npm install**
   - Executa o comando `npm install` para instalar as dependências listadas no `package.json`. Esta ação cria uma camada que inclui todos os ficheiros e diretórios adicionados pelo `npm install` (por exemplo, a pasta `node_modules` e todos os pacotes instalados).

5. **WORKDIR /home/node**
   - Define o diretório de trabalho para `/home/node`. Assim como anteriormente, esta instrução não cria uma camada de sistema de ficheiros, mas altera o contexto do diretório de trabalho.

6. **RUN chown -R node.node .**
   - Altera o proprietário de todos os ficheiros e diretórios no diretório `/home/node` (recursivamente) para o usuário `node`. Esta ação cria uma camada que altera as permissões dos ficheiros e diretórios.

7. **USER node**
   - Altera o usuário sob o qual os comandos subsequentes são executados para `node`. Esta instrução não cria uma camada de sistema de ficheiros, mas configura o contexto para comandos subsequentes.

8. **EXPOSE 80**
   - Informa que o container escutará na porta 80. Esta instrução não cria uma camada de sistema de ficheiros, mas documenta a porta exposta.

9. **WORKDIR /home/node/app**
   - Redefine o diretório de trabalho de volta para `/home/node/app`. Assim como anteriormente, não cria uma camada de sistema de ficheiros.

10. **CMD ["node", "app.js"]**
    - Define o comando padrão que será executado quando o container iniciar (`node app.js`). Esta instrução não cria uma camada de sistema de ficheiros.

### Resumo das Camadas Não-Vazias Criadas:
1. `COPY src/ ./`: Copia os ficheiros `package.json`, `app.js`, `README.md` para `/home/node/app`.
2. `RUN npm install`: Instala as dependências do Node.js e cria a pasta `node_modules` e outros ficheiros necessários.
3. `RUN chown -R node.node .`: Altera as permissões dos ficheiros e diretórios no diretório `/home/node`.

Estas são as camadas não-vazias adicionadas sobre a imagem base `node:alpine` no seu Dockerfile.

# 6.2 Modifique o Dockerfile para tirar melhor proveito da build cache relativa à atual linha 4, minimizando o número de camadas finais (não-vazias) no sistema de ficheiros, indicando quais serão.

Para otimizar o uso do cache de build e minimizar o número de camadas finais (não-vazias) no Dockerfile, é importante colocar as instruções que não mudam frequentemente (como a instalação de dependências) antes das que mudam frequentemente (como a cópia do código-fonte). Além disso, podemos reduzir o número de comandos `WORKDIR` ao mínimo necessário. 

Vamos ajustar o Dockerfile seguindo estas diretrizes:

### Dockerfile Original

```Dockerfile
FROM node:alpine
WORKDIR /home/node/app
COPY src/ ./
RUN npm install
WORKDIR /home/node
RUN chown -R node.node .
USER node
EXPOSE 80
WORKDIR /home/node/app
CMD ["node", "app.js"]
```

### Dockerfile Otimizado

```Dockerfile
FROM node:alpine

# Definindo o diretório de trabalho e copiando apenas o package.json primeiro
WORKDIR /home/node/app
COPY src/package.json ./

# Instalando as dependências (essa camada será cacheada enquanto package.json não mudar)
RUN npm install

# Copiando o resto dos arquivos do código-fonte
COPY src/ ./

# Mudando o proprietário dos arquivos e diretórios para o usuário node
WORKDIR /home/node
RUN chown -R node.node /home/node/app

# Configurando para rodar o container como usuário node
USER node

# Expondo a porta 80
EXPOSE 80

# Definindo o diretório de trabalho e o comando padrão
WORKDIR /home/node/app
CMD ["node", "app.js"]
```

### Camadas Não-Vazias Criadas:

1. **COPY src/package.json ./**:
   - Copia apenas o arquivo `package.json` para o diretório de trabalho.

2. **RUN npm install**:
   - Instala as dependências do Node.js (criando a pasta `node_modules` e outros arquivos necessários).

3. **COPY src/ ./**:
   - Copia o restante dos arquivos do código-fonte (`app.js` e `README.md`).

4. **RUN chown -R node.node /home/node/app**:
   - Altera as permissões dos arquivos e diretórios dentro de `/home/node/app`.

### Racional
Ao mover a instrução `COPY src/package.json ./` antes do `RUN npm install`, garantimos que a camada de instalação de dependências (`RUN npm install`) seja cacheada, a menos que o `package.json` seja modificado. Isto reduz a necessidade de reinstalar as dependências sempre que os arquivos de código-fonte mudam, minimizando as camadas criadas durante a build.

## 7. Um ficheiro docker-compose.yml, para especificação de uma solução composta com o nome tp2, contém três serviços: svca, svcb e svcc, todos colocados na rede svcnet de tipo bridge. Os serviços svca e svcc têm uma instância, mas svcb foi lançado com scale=2. Indique os nomes de serviços e de contentores que são registados no serviço de DNS interno e que endereço(s) refere cada um desses nomes.

Em uma configuração com Docker Compose, os nomes de serviços e contentores são gerenciados automaticamente pelo Docker Compose, e os serviços são registados no DNS interno da rede.

### Nomes de Serviços e Contentores:

- **Serviços:**
  1. `svca`
  2. `svcb`
  3. `svcc`

- **Contentores:**
  - `svca`:
    - Nome do contentor: `tp2_svca_1`
  - `svcb` (com scale=2):
    - Nome dos contentores: `tp2_svcb_1` e `tp2_svcb_2`
  - `svcc`:
    - Nome do contentor: `tp2_svcc_1`

### Endereços e Resolução de DNS:

- **Nomes de Serviços**:
  - `svca`: Refere-se ao endereço do contentor `tp2_svca_1`
  - `svcb`: Refere-se a um dos contentores `tp2_svcb_1` ou `tp2_svcb_2`, dependendo do mecanismo de load balancing do Docker.
  - `svcc`: Refere-se ao endereço do contentor `tp2_svcc_1`

- **Nomes de Contentores**:
  - `tp2_svca_1`: Endereço do contentor `svca`
  - `tp2_svcb_1`: Endereço de um dos contentores `svcb`
  - `tp2_svcb_2`: Endereço do outro contentor `svcb`
  - `tp2_svcc_1`: Endereço do contentor `svcc`

### Funcionamento do DNS Interno:
No Docker Compose, todos os serviços registados na rede `svcnet` podem resolver uns aos outros pelo nome do serviço. Por exemplo:
- O serviço `svca` pode se conectar ao serviço `svcb` utilizando o nome `svcb`.
- O serviço `svcc` pode se conectar ao serviço `svca` utilizando o nome `svca`.

Os nomes dos contentores são únicos e seguem o padrão `<nome_compose>_<nome_serviço>_<índice>`, onde `<nome_compose>` é o nome da solução composta especificada (neste caso, `tp2`), `<nome_serviço>` é o nome do serviço, e `<índice>` é o índice do contentor (iniciado em 1).

Esta configuração permite que os serviços se comuniquem facilmente, utilizando os nomes dos serviços registados no DNS interno da rede `svcnet`.

