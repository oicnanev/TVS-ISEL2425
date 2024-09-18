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
- Entidade administrativa do SO responsável pelo programa em execução.
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

## 17SET24

### Ambiente virtual para a disciplina

(L)Ubuntu 24.04 LTS

- Imagem VirtualBox x86
- WSL 2 com Ubuntu 24.04
 	* Windows Subsystem Linux
 	* Começou nos anos 80 por exigência do governo USA de suporte a aplicações que usavam sistemas *Posix*
 	* Atualmente, *Microsoft* volta a investir em subsistemas devido provavelmente ao sucesso do *Docker*
 	* Usa uma máquina virtual oculta dentro do *Windows*
 	* Distribui o **kernel** *Linux* dentro do *Windows*

> Em Linux para vermos qual a distribuição usada executar o seguinte comando: ```cat /etc/os-release```

> Em Windows para aceder ao *file system* do subsistema linux, escrever na barra do *Explorador do Windows*: ```\\wsl$\<nome_da_imagem>```

### Processo - File Descriptor Table
![file descriptor table](../img/filedescriptor.jpeg)

Todas as informações administrativas dos processos estão do lado do **kernel**

**File Descriptor Table** - cada processo tem as suas sessões (acessos a *files*). Cada sessão é identificada por um *index*, 0...X.
**File** - ficheiro, em *Linux*, qualquer entidade que permita **Input/Output**. Em Windows, existem ainda outros objectos do sistema que não permitem I/O como semáforos, enventos, etc. Em Windows, estas entradas não são identificadas por *index* mas sim por um *handle*, uma espécie de mapa em que cada vez que um processo é criado, para interagir com um objecto do sistema, recebe uma 'chave'.

### Operações de I/O implementadas pelo Sistema Operativo

O sistema operativo UNIX foi construído com umas regras muito simples mas que dão muito trabalho a cumprir. Exemplo disso, é na linguagem C, as funções ```maloc``` e ```free```, ao invés do *garbage colector* de linguagens mais recentes.

Para efetuar operações de I/O é necessário passar de **user mode** para **kernel mode**, e os Sistemas Operativos, bem como a linguagem C, implementam de forma diferente essas funções.
  ![i/o operations](../img/io_operation.jpeg)

| C function | UNIX (Posix) | Windows | Description |
| ---------- | ------------ | ------- | ----------- |
| fopen | open | CreateFile | Pedir acesso |
| fread | read | ReadFile | Ler dados (input) |
| fwrite | write | WriteFile | Escrever dados (output) |
| fclose | close | CloseHandle | Terminar acesso |
| ioctl(?) | ioctl | DeviceIoControl | Configurações gerais, ex:. set comunication baudrate |  

#### UNIX File Descriptor Table Default Index Allocation
| index # | Default allocation |
| ------- | ------------------ |
| 1 | stdin |
| 2 | stdout |
| 3 | stderr |

Em C, **stderr** é como que um "embrulho" da biblioteca que faz chegar dados ao *index* 2 do *File Descriptor Table* do programa em questão. Para imprimir o erro, usar a função ```perror("error user message");``` imprime a mensagem escolhida pelo programador acrescida do código de erro do sistema operativo.

Para alterar o *index* por defeito do *File Descriptor Table*:
```terminal
./prog output.txt 1> myStdout.txt
```
Neste exemplo, redirecionamos o stdout do *index* 1 para o ficheiro "myStdout.txt". "output.txt" é um parâmetro do programa "prog".
Também é possível redirecionar os outros *index*, "stdin"" e "stderr".

### Permissões em UNIX

| owner | group | others |     |
| ----- | ----- | ------ | --- |
| rwx | rwx | rwx | read/write/execute |
| 110 | 100 | 100 | notação binária |
| 6	 | 4 | 4 | notação octal |

Notação em C:
- **644** - decimal
- **0644** - octal
- **0x644** - hexadecimal	

Quando executamos ```ls -l``` em Linux ou apenas ```ll``` em Ubuntu, a listagem que aparece mostra:
- tipo de ficheiro, permissões dono, permissoes grupo e permissões outros
- dono
- grupo
- tamanho
- data de criação
![ls -l](../img/ll.png)

#### Tipos de ficheiros oficiais em UNIX

Em Linux, o tipo de ficheiro é identificado por um caractere específico. Os oficiais são:

 - **Ficheiro Regular (-)**: Representa ficheiros comuns, como documentos, scripts e programas executáveis.
 - **Diretoria (d)**: Representa diretórios que contêm outros ficheiros.
 - **Link Simbólico (l)**: Representa links simbólicos que apontam para outros ficheiros ou diretórios.
 - **Ficheiro de Dispositivo de Caractere (c)**: Representa dispositivos que manipulam dados como caracteres (bytes), como teclados e ratos.
 - **Ficheiro de Dispositivo de Bloco (b)**: Representa dispositivos que manipulam dados em blocos, como discos rígidos.
 - **Socket (s)**: Utilizado para comunicação entre processos, permitindo a troca de dados.
 - **FIFO (Named Pipe) (p)**: Utilizado para comunicação entre processos, operando em uma base de primeiro a entrar, primeiro a sair (FIFO).

