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

Alguns comandos docker:

- `docker pull archlinux`- faz download da imagem archlinux
- `docker images` - listas as imagens instaladas no sistema
- `docker run ubuntu` - cria e corre um contentor ubuntu
- `docker run -t ubuntu` - run com acesso ao terminal (stdout)
- `docker run -it ubuntu` - run com acesso ao terminal e com stdin
- `docker ps -a` - mostra os contentores que estão a correr e os parados (-a --all). só ps mostra apenas os que estão a correr
- `docker start archlinux -ai` - corre um ou mais contentores parados com stdout/stdin (-a --attach) e interatividade (-i)
- `docker build -t tvs51n1 [Dockerfile directory]` - constroi uma imagem docker com o nome (tag) tvs51n1
- `docker run -it tvs51n1 /bin/bash` - run com substituição do [CMD] por /bin/bash
- `docker exec -it [nome do contentor] /bin/bash` - permite executar um comando (neste caso /bin/bash) no contentor a correr
- `docker prune [contentor]` - remove o contentor
- `docker run tvsapp -p 12345:12345` - run tvsapp expondo o porto interno 12345 para a máquina no porto 12345

No mundo docker chama-se sistema operativo ao *ambiente de execução*, tudo o que constitui o sistema operativo excepto o *kernel*

Cada contentor a correr tem um *file system* de **diff** que contem apenas aquilo que foi alterado em relação à imagem original (base)

![Base vs Diff](../img/docker_diff.png)

Contentores não são máquinas virtuais, são processos que correm num ambiente isolado de execução, com sistema de ficheiros limitado

#### Construção de imagens docker

![docker image](../img/image.png)

- uma imagem é constituída por dois componentes
    * file system
    * meta dados / meta informação
- é construida através de instruções num ficheiro Dockerfile
- a primeira instrução de um Dockerfile é "em que imagem a nossa imagem se basea"
    * existe uma base vazia chamada **scratch**, `FROM scratch` 
- normamente é usado um `ENTRYPOINT` vazio que é concatenado com `CMD`, onde metemos o comando que queremos correr
    * `CMD` pode ser substituido em run, `ENTRYPOINT` não pode ser substituido
- pode ser necessário forçar o `build` porque o docker quando faz `build` aproveita *cache* de *builds* anteriores
- o último *user* usado no Dockerfile é o que fica como *user* da *image*

#### Exemplo Dockerfile

```Dockerfile
FROM       ubuntu
LABEL      org.opencontainers.image.author="joao.trindade@isel.pt"
USER       root
RUN        apt-get -qq -y update && \
	        apt-get upgrade && \
	        apt-get -y autoclean && \
	        apt-get -y autoremove && \
           run -rf /var/lib/apt/list/*

# create user tvs
RUN        useradd -m tvs && \
           cp /root/.bashrc /home/tvs/ && \
           mkdir /home/tvs/work && \
           chown -R --from=root tvs /home/tvs
ENV	    HOME=/home/tvs
WORKDIR    ${HOME}/work
USER	    tvs
ENTRYPOINT [""]
CMD	    ["/bin/sh"]  			
```

### 04DEC2024

#### Redes docker

É possível associar um contentor a uma 'visão de rede'. Para isso é necesário criar primeiro essa rede. Existem vários tipos (drivers) de redes no docker:

- **null** - sem rede
- **host** - rede do host
- **bridge** - rede default (uma espécie de NAT entre a rede do host e a rede docker)
- ouros - ex. alterar MAC address, redes que ligam diferentes dockerd em máquinas distintas, etc.

Para cada 'solução docker' (conhunto de containers relacionados entre si) deve ser criada uma rede isolada para que a rede docker não misture as várias soluções na mesma rede.

- ```docker network ls``` - lista as redes (iniciais) do docker
- ```docker network create [nome da rede] -- driver bridge``` - cria uma rede do tipo bridge mas diferente da bridge default (tem serviço de DNS, no endereço xxx.xxx.xxx.11)
- ```docker run -it --network tvsnet1 busybox```- para correr um contentor numa rede própria
- ```nslookup [container hostname] 127.0.0.11``` - dns resolver no busybox

#### docker compose

Docker compose serve para criar uma solução docker de forma descritiva. Para isso é usado um ficheiro *yaml* com os campos:

- name - o nome da solução
- service - contentores
- networks - a(s) rede(s) usada(s)
- volumes - referencias permanentes e não voláteis para o sistema de ficheiros da máquina (ex. uma base de dados)

Exemplo:

```yaml
name: "tvs"

services:
	entry:
		build:
			context: ./nginx
		depends_on:
			- webapp
		 ports:
			- "2024:2024"
		netwoks:
			- frontnet
	webapp:
		build:
			context: ./webapp
		depends_on:
			- storage
		environment:
			- ELASTIC_URL=http://tvs-storage-1:9200
		networks:
			- frontnet
			- backnet
	storage:
		image: elasticsearch:8.16.1
		environment
			- discovery.type=single-node
			- xpack.security.enable=false
		volumes:
			esdata:/usr/share/elasticsearch/data
		networks:
			backnet

volumes:
	esdata:
		driver: local

networks:
	frontnet: {}
	backnet: {}
```

**Comandos**

- ```docker compose up -d``` - build and run the solution
- ```docker compose down``` - stop the solution
- ```docker compose up -d --scale webapp=3 --no-recreate```- run the solution with 3 instances of the service webapp
- ```seq 32 | xargs -I{} curl -s http://localhost:2024/ | grep "HOST" | sed "s/<\/\?[a-z]\+>//g" | sed "s/^[[:space:]]*//" | sort | uniq -c``` - check how many webapps are running

 