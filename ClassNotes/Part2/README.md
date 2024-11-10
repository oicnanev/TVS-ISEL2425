# Part 2

## 11NOV2024

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


