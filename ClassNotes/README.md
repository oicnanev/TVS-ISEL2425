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
