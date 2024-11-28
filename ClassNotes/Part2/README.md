# Part 2

## 11NOV2024 - Mecanismos de comunicação entre processos UNIX

- Mecanismos de comunicação entre processos
- Como funciona a gestão da parte do sistema operativo que trabalha em **user mode**
- Modos de virtualização e elementos de virtualização que podemos usar nos nossos sistemas

### Formas dos processos poderem comunicar entre si:

- **pipe** (anonymous) - comunicação entre processos hierarquicamente relacionados
- **mapeamento de ficheiros em memória** - não há sincronização entre processos que partilham dados nesta zona
- **sockets** - exagerado para a comunicação dentro da mesma máquina
 
### Mecanismos usados em UNIX, baseados nos processos acima que resolvem os problemas dos mesmos:

- **fifo** (named pipe) - igual ao *pipe* mas tem um nome. É necessário fazer `open()`
- **shm** (shared memory) - tipo `mmap` mas com nome e que pode ser partilhado entre vários ficheiros
- **uds** (UNIX Domain Socket) - socket específico para comunicação entre processos
 
Todos estes mecanismos resolvem o pproblema do nome da mesma forma, são nomes no sistema de ficheiros, são **UNIX files**

- **-** ficheiro normal, inclui `shm`
- **d** directoria (mapa de ficheiros)
- **c** char file (I/O byte a byte - ex. porta serie)
- **b** char file (I/O bloco a bloco - ex. disco rigido)
- **l** symbolik link (atalho)
- **s** socket, inclui `uds`
- **p** pipe, inclui `fifo` 

### mkfifo

`int mkfifo(const char *pathname)`

`mkfifo [name]` - utilitario unix, cria um *named pipe*

 ![fifo](../img/fifo.png)

### shm

- vão aparecer em `/dev/shm`
- são apagados cada vez que é feito reboot à máquina
- necessitam a montagem de um arsenal de sincronização
- tem problemas com *pointers*, têm de ter um offset em vez de um endereço

![shm](../img/shm1.png)

Exemplo:

```c
// Include the necessary headers

int main() {
    const char *name = "/my_shared_memory";
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Set the size of the shared memory region
    size_t size = 1024;

    // Resize the shared memory object to the desired size
    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate");
        return 1;
    }

    // Map the shared memory object into the process address space
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Write data to the shared memory
    sprintf(ptr, "Hello, shared memory!");

    // Print the contents of the shared memory
    printf("Shared memory contents: %s\n", (char *)ptr);

    // Unmap the shared memory object
    if (munmap(ptr, size) == -1) {
        perror("munmap");
        return 1;
    }

    // Close the shared memory file descriptor
    if (close(shm_fd) == -1) {
        perror("close");
        return 1;
    }

    // Unlink the shared memory object
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
        return 1;
    }

    return 0;
}
```

 ###  uds

Existem sockets de dois tipos:

- stream - ideia de ligação, bytes por orderm (TCP)
- datagram - ideia de endereços, enviam-se mensagens soltas (UDP)

![socket](../img/socket1.png)

#### socket family

![family](../img/socket2.png)

## 12NOV2024 - Linix init systems

- **daemon** - processo ou família de processos relacionados que estão a resolver um determinado problema

### Init em Linux

#### systemVinit vs systemd

##### systemVinit

- Tudo scripts shell em directorias numeradas
- O que arranca primeiro está na directoria **1**, depois, o que está na directoria **2** e assim sucessivamente

##### systemd

- Replica o **launchd** da **apple**
- Objetivos, arrancar menos e arrancar mais em paralelo
- Se há serviços que necessitam de sockets, criar apenas os sockets, o serviço é criado à posteriori aquando da escrita no socket
- Não usa scripts bash, para não criar processos
- Usa ficheiros de configuração chamados **unit files**
- Permite a interação através de um cliente, **systemctl**
- Permite visualização de logs através de **journalctl**
- - [*post*](https://0pointer.de/blog/projects/systemd.html) do criador do systemd

### systemd

- `systemctl` - lista as units que estão disponíveis na máquina
    * devices
    * mounts
    * cpus.path
    * services
    * slices
    * sockets
    * targets - agrupamentos de unidades. (podem corresponder a níveis de operacionalidade)

- `systemctl list-dependencies basic.target`
- `sudo systemctl isolate multi-user.target`
- os ficheiros de configuração da máquina podem ser encontrados em:
    * `/lib/systemd/system -> /usr/lib/...`
    * `/etc/systemd/system`
    * `/run/` - unidades em runtime
    * `/home` - unidades apenas do user

- `systemd` procura primeiro no `/etc` e só depois em `/lib`. `/lib` é mais generalista.

## 19NOV2024 - Systemd Unit Files

### Unit Files

```service
[Unit]
Description=TVS Control Application
Requires=tvsctld.socket
After=tvsctld.socket

[Service]
ExecStart=/opt/isel/tvs/tvsctld/bin/tvsctld
Restart=no

[Install]
Also=tvsctld.socket
```

- Unit - coisas gerais sobre a unidade
    * Requires - dependências obrigatórias
    * Wants - dependências opcionais (se os serviços estiverem presentes na máquina devem arrancar antes deste serviço)
    * After - indicação relativa à ordem de arranque / ativação do sistema
- Service - especificações próprias do serviço
- Install - target necessário para correr o serviço
    * WantedBy - associa o serviço a um target, ex m̀ulti-user.tagert`
    * Also - indica que o se tentarem fazer enable ao serviço este manda fazer enable ao aqui indicado

## 27NOV2024 - Virtualização e Sistemas de Contentores

Sistemas de contentores nascem de um conjunto de iniciativas para colocar programas a funcionar com um isolamento superior aquele que o sistema operativo proporciona.

O isolamento por **processo** isola a memória e controla os CPUs através da ideia de *thread*. E o isolamento acaba aqui. O *file system* é o mesmo para todos os processos. Os recursos de rede também podem entrar em conflito, por exemplo, dois serviços configurados para usar o mesmo porto, faz com que o segundo, dẽ erro e não arranque - porto indisponível.

Como tal, foi-se ao longo do tempo, tentando arranjar formas de aumentar o isolamento. por exemplo, o `chroot` em Linux que cria um ambiente isolado do *file system*, alterando a pasta `/` para uma nova pasta especificada. Programas que correm aqui, só vêm o *file system* a partir desta localização para baixo.

Também já se começou a usar virtualização da rede. há processos, por exemplo, que julgam que a máquina tem um *hostname* diferente.

**Docker** e outros mecanismos de contentores, tiram partido destas técnicas de isolamento para simular que estão numa máquina diferente. 

![Contentores vs Máquinas Virtuais](../img/contentoresVSvms.png)

Assim como um ficheiro executável tem a imagem de um programa e é possível correr isoladamente várias instâncias do programa, num sistema de contentores, podemos correr isoladamente vários contentores referentes à mesma imagem. Podemos dizer que o contentor está para a imagem assim como o processo está para o programa. 

![Programa vs Imagem](../img/programaVSimagem.png)

### Docker



