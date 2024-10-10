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
- Representação estática de programa em execução.
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

> A *file* is a sequence of bytes, nothing more and nothing less. Every I/O device, including disks, keyboards, displays, and even networks, is modeled as a file. All input and output in the system is performed by reading and writing files, using a small set of system calls known as Unix I/O. - Computer Systems a Programmers Perspective, Chapter 1.7.4 Files.

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
| 0 | stdin |
| 1 | stdout |
| 2 | stderr |

Em C, **stderr** é como que um "embrulho" da biblioteca que faz chegar dados ao *index* 2 do *File Descriptor Table* do programa em questão. Para imprimir o erro, usar a função ```perror("error user message");``` imprime a mensagem escolhida pelo programador acrescida do código de erro do sistema operativo.

Para alterar o *index* por defeito do *File Descriptor Table*:
```terminal
./prog output.txt 1> myStdout.txt
```
Neste exemplo, redirecionamos o stdout do *index* 1 para o ficheiro "myStdout.txt". "output.txt" é um parâmetro do programa "prog".
Também é possível redirecionar os outros *index*, "stdin"" e "stderr".

> Código exemplo da aula, criação de um **file descriptor** que remete para um ficheiro no *index* 3. [prog01-files.c](../ClassCode/01-files/prog01-files.c)

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

## 18SET2024

### Atributos do processo em UNIX
- **pid** - Process Identifier
- **ppid** - Parent pid
- **user** - owner
- **exit code** - valor númerico retornado após término
   
> Processo nº1 em UNIX - ```/sbin/init```

#### Comandos UNIX para listar processos em execução

- **ps -aux**
  ![ps -aux](../img/ps.png)
- **htop**
  ![htop](../img/htop.png)
- **btop**
  ![btop](../img/btop.png)

#### Exit code 
No Unix, o **exit code** (ou código de saída) é um valor numérico retornado por um processo quando termina. Este valor é usado para indicar se o processo foi concluído com sucesso ou se ocorreu algum erro. 

- **Exit Code**: É o valor numérico retornado por um processo ao terminar. Por convenção, um exit code de 0 indica que o processo foi concluído com sucesso, enquanto qualquer valor diferente de 0 indica que ocorreu um erro.
- **Exit Status**: É o estado final do processo, representado pelo exit code. Pode ser verificado usando a variável especial ```$?``` na shell, que armazena o exit code do último comando executado.
- **Exited**: Refere-se ao estado de um processo que terminou sua execução. Quando um processo “exits”, ele retorna um exit code ao sistema operacional.
- **Terminated**: Indica que um processo foi encerrado. Isso pode ocorrer de forma normal (quando o processo completa sua tarefa e retorna um exit code) ou anormal (quando o processo é interrompido por um sinal, como ```SIGINT``` ou ```SIGTERM```).


Alguns códigos comuns que ajudam a identificar rapidamente o motivo pelo qual um processo terminou e são essenciais para depuração e controle de fluxo em scripts e programas:

| Exit Code | Significado |
| --------- | ----------- |
| 0 | Sucesso (o comando foi executado sem erros) |
| 1 | Erro genérico |
| 2 | Uso incorreto do comando ou argumento |
| 126 | Permissão negada ou incapaz de executar |
| 127 | Comando não encontrado |
| 128+n | O comando foi terminado por um sinal (onde n é o número do sinal) |
| 130 | Terminado por Ctrl+C (sinal SIGINT) |
| 143 | Terminado por SIGTERM |

### Criação de processos programáticamente

Em **Windows** temos a função [CreateProcessA](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa). É composto por 10 parâmetros sendo que o único obrigatorio é o *lpApplicationName* ou *lpCommandLine*.

Em **UNIX** existem 2 funções, **fork()** e **exec\[\]\(...,...,....\)**

#### fork()
A função ```fork()``` é usada para criar um novo processo, chamado de processo filho. O processo filho é cópia quase exata do processo pai, exepto algumas diferenças como o pid e ppid.
- **Criação do Processo** - cria um novo processo que é cópia exata do processo que a chamou (processo corrente).
- **Retorno** - retorna duas vezes: uma vez no processo pai (com o pid do filho) e uma vez no processo filho (com 0). Se houver um erro, retorna -1 no processo pai.
- **Execução Paralela** - Após a chamada ```fork()```, tanto o processo pai quanto o processo filho continuam em execução a partir do ponto onde o ```fork()``` foi chamado.
- **Perda de parentalidade** - Caso o processo pai termine, o ppid do filho altera para um processo que esteja a correr.
- **Chamada ```wait()``` ou ```waitpid()```** - após chamar o ```fork()``` é necessário chamar uma destas funções por várias razões:
    * **Evitar processos zombies** - quando um processo filho termina, ele entra em um estado chamado “zombie” até que o processo pai leia seu status de término. Se o processo pai não chamar ```wait()``` ou ```waitpid()```, o processo filho permanecerá como zombie, ocupando uma entrada na tabela de processos do sistema. Isso pode eventualmente levar ao esgotamento dos recursos do sistema, caso  muitos processos zumbis se acumulem.
    * **Sincronização** - Permite que o processo pai se sincroniza com o processo filho, para que não termine enquanto o processo filho não tenha terminado também.
    * **Recuperação do exit code** - Permite ao processo pai verificar se o processo filho terminou com sucesso ou ocorreu algum erro.
    * **Gestão de recursos** - Ao chamar ```wait()``` ou ```waitpid()```, o processo pai pode liberar os recursos associados ao processo filho. Isso pode incluir a entrada na tabela de processos, bem como, outros recursos do sistema que foram alocados para o processo filho.

> Código de exemplos da aula:

> - fork [prog02-fork.c](../ClassCode/02-fork/prog02-fork.c)
> - waitpid [prog03-waitpid.c](../ClassCode/02-fork/prog03-waitpid.c)
> - same virtual memory space with different physical memory addresses [prog04-procmem.c](../ClassCode/02-fork/prog04-procmem.c)

#### exec()
A família de funções exec() substitui a imagem do processo atual por uma nova imagem de processo. Isso significa que o código e os dados do processo atual são substituídos pelo código e dados de um novo programa. No âmbito do processo corrente, destroi a sua representação virtual e cria um novo, no entanto reutiliza a sua tabela de descritores de ficheiros.

- **Substituição de Processos**: ```exec()``` carrega um novo programa no espaço de endereço do processo que o chama e começa a executá-lo.
- **Família de Funções**: Existem várias variantes de ```exec()``` que diferem na forma como os argumentos e o ambiente são passados para o novo programa, como:
	- ```execl()``` - usar quando sabemos o número de parãmetros a usar no processo novo. **l** de *list*?
	- ```execlp()``` - ao usar o **p**, procura o programa no *path* 
	- ```execle()``` - ao usar o **e**, passa-se um *poiter* para *array* extra com as variáveis de ambiente. **e** de *environment*.  
	- ```execv()``` - 1º arg. é o nome do programa, 2º, um *pointer* para *array* de argumentos que pode ser preenchido dinâmicamente. **v** de *variable_size*?
	- ```execvp()``` - ao usar o **p**, procura o programa no *path* 
	- ```execvpe()``` - ao usar o **e**, passa-se um *poiter* para *array* extra com as variáveis de ambiente. **e** de *environment*
- **Não Retorna**: Se ```exec()``` for bem-sucedido, ele não retorna ao programa que o chamou. Se falhar, ele retorna -1.

> Exemplo da aula:

> - exec [prog05-exec.c](../ClassCode/03-exec/prog05-exec.c)

### Redirecionamentos na shell

| Index | File Descriptor |
| ----- | --------------- |
| 0 | stdin |
| 1 | stdout |
| 2 | stderr |

#### Redirecionamento da saída
```terminal
comando 1> output.txt
```

#### Redirecionamento da entrada
```terminal
comando 0< input.txt
```

#### Redirecionamento da saída de erros
```terminal
comando 2> error.txt
```

#### Redirecionamento arbitrário
```terminal
comando 88> someFile.txt
```

#### Redirecionamento da saída e erro para o mesmo ficheiro
```terminal
comando &> all.txt
```

#### Exemplo de encadeamento de redirecionamentos
```terminal
input.txt < comando 1> output.txt 2> error.txt 88>something.txt
```

#### Função ```dup2(int oldfd, int newfd)```

É usada para duplicar um *file descriptor*, duplicando o ```oldfd``` para ```newfd```, Se o ```newfd``` já estiver aberto, será encerrado antes de ser reutilizado sem ser necessário chamar ```close()```. Se o ```oldfd``` não for um *file descriptor* válido, a função falha e o ```newfd``` não será encerrado.

> Exemplo de programa que usa **dup2** para fazer *redirection*:

> - redirection with dup2 [prog06-redir.c](../ClassCode/04-redir/prog06-redir.c)


## 24SET2024

### Pipes
**Pipes** são um mecanismo de comunicação entre processos em Unix. Permitem o fluxo unidirecional de dados entre processos, onde um processo escreve dados no *pipe* e outro lê esses dados.

![pipes](../img/pipe1.jpeg)

#### Funcionamento do ponto de vista do *Kernel*

![pipe](../img/pipe2.jpeg)

1. **Criação do Pipe**:
    - A operação *pipe* cria um objeto *pipe* na memória do *kernel*, retorna um para de *file descriptors*, um para ler e outro para escrever no *pipe*.
    - O *kernel* utiliza um sistema de ficheiros virtual ```pipefs``` para gerir os *pipes*. Este sistema de ficheirosé montado no espaço do *kernel* durante o *init*.
2. **Buffer de memória**:
    - O *kernel* usa a sua própria memória como *buffer* para transferir dados entre os espaços de endereços do *user* e do *kernel*. Quando um processo escreve dados no *pipe*, o *kernel* copia esses dados do espaço de endereço do *user* para o espaço de endereços do *kernel*. Da mesma forma, quando um processo lê do *pipe*, os dados são copiados do espaço de endereço do *kernel* para o espaço de endereços do *user*.
3. **Chamadas de Sistema**:
    - O *kernel* fornece várias chamadas de sistema para manipular operações de *pipe*, como ```pipe()```, ```read()```, e ```write()```. Essas chamadas garantem o funcionamento adequado dos *pipes* e permitem a transferência eficiente de dados entre processos.

#### Funcionamento do ponto de vista dos Processos
1. **Criação e uso**:
    - Um processo pode criar um *pipe* usando a chamada de sistema ```pipe()```, que retorna dois *file descriptors*: um para leitura e outro para escrita.
    - O processo de escrita 'empurra' dados para o *pipe*, enquanto o processo de leitura 'puxa' dados do outro extremo do *pipe*.
2. **Comunicação entre processos**
    - Os *pipes* são muitas vezes usados para comunicação entre processos pai e filho. Por exemplo, um processo pai pode criar um *pipe* antes de fazer ```fork()```. Após o ```fork()```, tanto o processo pai como o filho herdam os *file descriptors* do *pipe*, permitindo a comunicação entre eles.
3. **Exemplo prático**:
    - Considere uma *shell* que usa *pipes* para ligar a saída de um comando à entrada de outro. A *shell* cria processos filhos para cada comando e usa *pipes* para ligar a saída de um comando à entrada do próximo. Isso é feito configurando os *file descriptors* apropriados antes de chamar ```exec()``` para executar os comandos.
    ![exercicio](../img/exercicio_pipes.jpeg)
	- Tendo um processo com *file descriptor table*, onde se pede ao *kernel* para criar um *pipe*, vão ser criadas entradas na tabela que serão usads para manipular o mesmo (*pipe*):
	- Entrada 3 - permite ler do *pipe*
	- Entrada 4 - permite escrever do *pipe*
	- Ao fazer ```fork()``` no processo **P1**, criamos outro processo **P2** cuja tabela de *file descriptors* é cópia da tabela do processo pai (**P1**). Ficando assim com duas sessões ativas de leitura sobre o *pipe* e duas sessões de escrita sobre o *pipe*.

> Programas de exemplo do funcionamento dos *pipes*:

>- pipe basic [prog07-pipe-basic.c](../ClassCode/05-pipe/prog07-pipe-basic.c)
>- pipe with fork [prog08-pipe-fork.c](../ClassCode/05-pipe/prog08-pipe-fork.c)
>- pipe with fork, dup2 and exec [prog09-pipe-fork-dup2-exec.c](../ClassCode/05-pipe/prog09-pipe-fork-dup2-exec.c)
	
## 01OUT24

### Signals
![signals as virtual interrupts](../img/signals.jpeg)

- A iniciativa de ação é sempre do CPU, no entanto, nos sistemas reais existe um mecanismo para dispositivos I/O pedirem CPU - os *interrupts*.
- Existem dispositivos lentos ou dependentes de ações externas o que faz com que a iniciativa do CPU (Polling) se torne ineficiente. (Ex.: CD/DVD, placas de rede, etc.). Os *interrupts* permitem que não seja necessário o *polling* por parte do CPU e este é avisado diretamente pelo dispositivo através do mecanismo de *interrupts*.
- Nos servidores modernos, todas as ligações são assíncronas; é tudo notificado ao CPU através de *interrupts*.
- Como forma de ocultar o funcionamento dos *interrupts*, nas linguagens de programação modernas, começou a surgir o ```async/await``` primariamente no **C#**, depois **JavaScript**, no **Kotlin** são usadas as **corotinas**, no **Java**, as **virtual threads**, etc.
- Em termos de **processos**, existem também notificações assíncronas de forma semelhante aos *interrupts*. Este processo é chamado de *Signals*. O número tipico de sinais (*standard*) por processo são 31, no entanto podem existir mais ou menos. Ver: ```man 7 signal``` em sistemas Unix.
- A forma de trabalhar com os *signals* é em tudo semelhante à forma com que se trabalha com *interrupts*.

#### Signal dispositions
Cada *signal* tem uma *disposition* atual, que determina o seu comportamento do processo quando recebe o mesmo.

As entradas na coluna **Ação** da tabela abaixo, especificam a disposição padrão para cada sinal como:
- **Term** - terminar o processo
- **Ign** - ignorar o sinal, (ex: morte de processo filho)
- **Core** - terminar o processo e guardar informação para *debug*
- **Stop** - pausar (*stop*) o processo
- **Cont** - continuar o processo se o mesmo estiver pausado (*stoped*) no momento.

Um processo pode alterar a disposição de um *signal* através das funções [```sigaction()```](https://www.man7.org/linux/man-pages/man2/sigaction.2.html) ou [```signal()```](https://www.man7.org/linux/man-pages/man2/signal.2.html).

##### Standard Signals
| Signal | Action | Comment |
| ------ | ------ | ------- |
| **SIGABRT** | Core | Abort signal from abort(3) |
| **SIGALRM** | Term | Timer signal from alarm(2) |
| **SIGBUS** | Core | Bus error (bad memory access) |
| **SIGCHLD** | Ign | Child stopped or terminated |
| **SIGCLD** | Ign | A synonym for SIGCHLD |
| **SIGCONT** | Cont | Continue if stopped |
| **SIGEMT** | Term | Emulator trap |
| **SIGFPE** | Core | Floating-point exception |
| **SIGHUP** | Term | Hangup detected on controlling terminal or death of controlling process |
| **SIGILL** | Core | Illegal Instruction |
| **SIGINFO** | | A synonym for SIGPWR |
| **SIGINT** | Term | Interrupt from keyboard |
| **SIGIO** | Term | I/O now possible (4.2BSD) |
| **SIGIOT** | Core | IOT trap. A synonym for SIGABRT |
| **SIGKILL** | Term | Kill signal |
| **SIGLOST** | Term | File lock lost (unused) |
| **SIGPIPE** | Term | Broken pipe: write to pipe with no readers; see pipe(7) |
| **SIGPOLL** | Term | Pollable event (Sys V); synonym for SIGIO |
| **SIGPROF** | Term | Profiling timer expired |
| **SIGPWR** | Term | Power failure (System V) |
| **SIGQUIT** | Core | Quit from keyboard |
| **SIGSEGV** | Core | Invalid memory reference |
| **SIGSTKFLT** | Term | Stack fault on coprocessor (unused) |
| **SIGSTOP** | Stop | Stop process |
| **SIGTSTP** | Stop | Stop typed at terminal |
| **SIGSYS** | Core | Bad system call (SVr4); see also seccomp(2) |
| **SIGTERM** | Term | Termination signal |
| **SIGTRAP** | Core | Trace/breakpoint trap |
| **SIGTTIN** | Stop | Terminal input for background process |
| **SIGTTOU** | Stop | Terminal output for background process |
| **SIGUNUSED** | Core | Synonymous with SIGSYS |
| **SIGURG** | Ign | Urgent condition on socket (4.2BSD) |
| **SIGUSR1** | Term | User-defined signal 1 |
| **SIGUSR2** | Term | User-defined signal 2 |
| **SIGVTALRM** | Term | Virtual alarm clock (4.2BSD) |
| **SIGXCPU** | Core | CPU time limit exceeded (4.2BSD); see setrlimit(2) |
| **SIGXFSZ** | Core | File size limit exceeded (4.2BSD); see setrlimit(2) |
| **SIGWINCH** | Ign | Window resize signal (4.3BSD, Sun) |

Os sinais **SIGKILL** e **SIGSTOP** não permitem ser capturados, bloqueados ou ignorados.

#### Numeração dos *signals* para *standard signals*

| Signal | x86/ARM most others | Alpha/SPARC | MIPS | PARISC | Notes |
| ------ | - | - | - | - | - |
| SIGHUP | 1 | 1 | 1 | 1 |  |
| SIGINT | 2 | 2 | 2 | 2 |  |
| SIGQUIT | 3 | 3 | 3 | 3 |  |
| SIGILL | 4 | 4 | 4 | 4 |  |
| SIGTRAP | 5 | 5 | 5 | 5 |  |
| SIGABRT | 6 | 6 | 6 | 6 |  |
| SIGIOT | 6 | 6 | 6 | 6 |  |
| SIGBUS | 7 | 10 | 10 | 10 |  |
| SIGEMT | - | 7 | 7 | - |  |
| SIGFPE | 8 | 8 | 8 | 8 |  |
| SIGKILL | 9 | 9 | 9 | 9 |  |
| SIGUSR1 | 10 | 30 | 16 | 16 |  |
| SIGSEGV | 11 | 11 | 11 | 11 |  |
| SIGUSR2 | 12 | 31 | 17 | 17 |  |
| SIGPIPE | 13 | 13 | 13 | 13 |  |
| SIGALRM | 14 | 14 | 14 | 14 |  |
| SIGTERM | 15 | 15 | 15 | 15 |  |
| SIGSTKFLT | 16 | - | - | 7 |  |
| SIGCHLD | 17 | 20 | 18 | 18 |  |
| SIGCLD | - | - | 18 | - |  |
| SIGCONT | 18 | 19 | 25 | 26 |  |
| SIGSTOP | 19 | 17 | 23 | 24 |  |
| SIGTSTP | 20 | 18 | 24 | 25 |  |
| SIGTTIN | 21 | 21 | 26 | 27 |  |
| SIGTTOU | 22 | 22 | 27 | 28 |  |
| SIGURG | 23 | 16 | 21 | 29 |  |
| SIGXCPU | 24 | 24 | 30 | 12 |  |
| SIGXFSZ | 25 | 25 | 31 | 30 |  |
| SIGVTALRM | 26 | 26 | 28 | 20 |  |
| SIGPROF | 27 | 27 | 29 | 21 |  |
| SIGWINCH | 28 | 28 | 20 | 23 |  |
| SIGIO | 29 | 23 | 22 | 22 |  |
| SIGPOLL |   |   |   |   | Same as SIGIO |
| SIGPWR | 30 | 29/- | 19 | 19 |  |
| SIGINFO | - | 29/- | - | - |  |
| SIGLOST | - | -/29 | - | - |  |
| SIGSYS | 31 | 12 | 12 | 31 |  |
| SIGUNUSED | 31 | - | - | 31 |  |

Quando temos um processo que termina é possível ver o seu *exit code* através do comando shell ```echo $```. O número retornado, caso seja superior a 128 indica que o processo terminou com um *signal*. Por exemplo, *exit code* = 130, diz-nos que o processo terminou com um *signal* (128) e que este *signal* foi SIGINT (*signal* #2). 128 + 2 = 130.

**ATENÇÂO** ao SIGPIPE - depois dum ```<CTRL>+<C>``` pode existir a tentativa de escrever *file descriptors* que já não existem.

> Exemplos de manipulação dos *signals* programáticamente:

>- Send SIGINT or SIGTERM [sig01.c](../ClassCode/06-signal/sig01.c)
>- SIGALARM terminates program [sig02.c](../ClassCode/06-signal/sig02.c)
>- Change default signal disposition [sig03.c](../ClassCode/06-signal/sig03.c)
>- Set disposition to signal handling function [sig04.c](../ClassCode/06-signal/sig04.c)
>- Set an alarm every second once a procedure receives a SIGTERM. So handling SIGINT and SIGALARM [sig5.c](../ClassCode/06-signal/sig05.c)

## 08OUT24

### Níveis de privilegio que os processadores x86 usam controlo de acesso a segmentos

- O CPU tem um registo chamado **PL** no qual está o *Privilege-Level* em que está a correr.
- Nível 0 - privilégio máximo - kernel corre neste nível
- Nível 3 - privilégio mínimo - user processes correm neste nível
- Níveis 1 e 2 - são usados por *device drivers* e máquinas virtuais (VMWare, VirtualBox, etc.)
- Existe ainda hipervisores que podem ser instalados por baixo do sistema operativo (VMWare ESXi, Citrix XenServer, Microsoft Hyper-V) e que os programadores normalmente dizem que trabalham em nível -1

![x86 Privilege Levels](../img/x86_PL.jpeg)

### Arranque e funcionamento do Sistema Operativo do ponto de vista do processador

- CPU arranca sempre com nível de privilégio máximo (PL-0)
- Kernel inicia ainda em privilégio máximo (PL-0)
- Kernel coloca **init** (/sbin/init) em execução (PID=1) e muda para privilégio mínimo (PL-3) - kernel escreve no stack o retorno da interrupção e depois força uma interrupção, esta ao retornar vai para o nível 3
- init chama outros programas e quando tem necessidade faz chamadas ao kernel

### Formas de regressar ao privilégio máximo

- **Chamadas do sistema**
    * solução clássica - interrupção por software, em assembly instrução ```int```
        + Linux - int 0x80
        + Windows - int 0x2E
    * atualmente - através de instrução (assembly):
        + ```sysenter``` / ```sysexit``` para **IA-32**
        + ```syscall``` / ```sysret``` para **AMD-64**
- **Atendimento de interrupções**
- **Excepções** (caso particular de interrupção)

Quando retorna de uma interrupção, o CPU retorna também ao nível de privilégio anterior.

### Handler de interrupções (tabela)

- Tabela normalmente com 256 entradas
- As primeiras 32 entradas são para tratar excepções
- As restantes são para tratar interrupções
- Cada linha tem:
    * endereço do código do kernel para onde se deve saltar
    * nível de privilégio em que o CPU deve ser colocado quando a interrupção acontecer
- O local da memória onde reside esta tabela só é acessível em *kernel mode*

### Memory Management

O CPU tem um registo **CR3** - Control Register 3 que aponta para um dos espaço de endereçamento, o espaço de endereçamento da tread corrente.

Na mesma instrução para alterar a thread atual, também se altera este CR3 para ter o espaço de endereçamento atual 

![IA-32 Memory Management](../img/ia32_mem_management.jpeg)

![IA-32 Page Frame and Page](../img/ia32_mem_management2.jpeg)

## 09OUT24

### Programação de um "Hello World! em Kernel (Assembly)"

ver código **FALTA**

**Linux System Call Table (excerto)**

| %rax | System Call | %rdi | %rsi | %rdx |
| ---- | ----------- | ---- | ---- | ---- |
| 0 | sys_read | unsigned int fd | char *buf | size_t count |
| 1 | sys_write | unsigned int fd | const char *buf | size_t count |
| 2 | sys_open | const char \*filename | int flags | int mode |
| 3 | sys_close | unsigned int fd |  |  |
| ... | | | | |

Ver mais [aqui](https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/)

### Evolução do Memory Management

[Manual 2 AMD64](https://kib.kiev.ua/x86docs/AMD/AMD64/24593_APM_v2-r3.06.pdf)
 pag 189

page translation Long mode - pag 199, 202
none PAE - pag 189



