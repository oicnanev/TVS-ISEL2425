## Preparação

Para a **etapa ii** de configurar a aplicação web como um serviço e executar 4 instâncias em portas específicas, foram efetuados estes passos:

1. Copiar a aplicação para o novo local e correr `npm install`

```bash
sudo mkdir -p /opt/isel/tvs/tvsapp
sudo cp -r cw3/tvsapp/app /opt/isel/tvs/tvsapp/
sudo npm install --prefix /opt/isel/tvs/tvsapp/app
```


2. Copiar a template do serviço `tvsapp@` para `/etc/systemd/system`

```bash
sudo cp ./tvsapp/etc/service/tvsapp@.service /etc/systemd/system
```

Editar o ficheiro tvsapp@.service para mostrar em que porta está a correr:

```ini
# /etc/systemd/system/tvsapp@.service
[Unit]
Description=TVS App Application - %i
After=network.target elasticsearch.service

[Service]
Environment=NODE_PORT=%i
Type=simple
User=isel
ExecStart=/usr/bin/node /opt/isel/tvsapp/app/tvsapp.js
Restart=on-failure
```


3. Fazer *reload* ao **systemd** e iniciar as instâncias

```bash
sudo systemctl daemon-reload
sudo systemctl start tvsapp@43211
sudo systemctl start tvsapp@43212
sudo systemctl start tvsapp@43213
sudo systemctl start tvsapp@43214
```

Para os serviços arrancarem com o *boot* do sistema operativo

```bash
sudo systemctl enable tvsapp@43211
sudo systemctl enable tvsapp@43212
sudo systemctl enable tvsapp@43213
sudo systemctl enable tvsapp@43214
```

4. Verificar se estão a funcionar corretamente:

```bash
Copy code
sudo systemctl status tvsapp@43211
sudo systemctl status tvsapp@43212
sudo systemctl status tvsapp@43213
sudo systemctl status tvsapp@43214
```

Caso exista algum erro, depurar, no meu caso não tinha o *user isel*:

```bash
sudo useradd isel:tvsgrp
```

Repetir os comandos `enable` e `status`

Por fim, abrir um browser e navegar por cada um dos URLs:

http://localhost:43211
http://localhost:43212
http://localhost:43213
http://localhost:43214

Se tudo foi configurado corretamente, cada URL deve exibir a aplicação a trabalhar na respetiva porta.

Para a **etapa iii** de configurar o Nginx como um *load balancer* para as instâncias da aplicação tvsapp, seguimos os passos abaixo:

1. Copiar o ficheiro de configuração do novo *site*, `tvsapp` para `/etc/nginx/sites-available`:

```bash
sudo cp ./tvsctl-srv/etc/nginx/sites-available/tvsapp /etc/nginx/sites-available
```

Criação de um novo ficheiro de configuração para o *site* tvsapp na porta 44444:

```bash
sudo nano tvsapp
```

O bloco **upstream tvsapp_backend** define o grupo de servidores (instâncias tvsapp) que atuarão como backend para o *load balancer*.

O bloco **server** configura o Nginx para escutar no porto 44444 e redirecionar os pedidos para o grupo tvsapp_backend.

2. Ativação da nova configuração do site

Criar um link simbólico na pasta `/etc/nginx/sites-enabled` para ativar o novo site:

```bash
sudo ln -s /etc/nginx/sites-available/tvsapp /etc/nginx/sites-enabled/
```

Verificar se a configuração do Nginx contém erros de sintaxe:

```bash
sudo nginx -t
```

Se houver algum erro, corrigir o ficheiro `tvsapp` antes de continuar.

Reler as configurações do serviço Nginx para aplicar as novas alterações:

```bash
sudo systemctl reload nginx
```

3. Testar o *Load balancer*

Abrir o browser e aceder a http://localhost:44444. 

Ao atualizar a página, a saída deve estar a alternar entre as instâncias nas portas 43211, 43212, 43213, e 43214.

Também é possível:

```bash
seq 32 | xargs -I{} curl -s http://localhost:44444/ | grep "PORT" | sed "s/<\/\?[a-z]\+>//g" | sed "s/^[[:space:]]*//" | sort | uniq -c
```

## Edição dos scripts bash

Os scripts a editar encontram-se na pasta `tvsctl-srv/bin/scripts`

1. Depois de elaborados, os scripts devem ser copiados para `/opt/isel/`

```bash
sudo mkdir -p /opt/isel/tvs/tvsctl-srv/bin/scripts
sudo cp -r ./tvsctl-srv/bin/scripts/*.sh /opt/isel/tvs/tvsctl-srv/bin/scripts
sudo chmod +x /opt/isel/tvs/tvsctl-srv/bin/scripts/*.sh
```

## Alteração do *ownership* da pasta `/opt/isel`

```bash
sudo chown -R isel:tvsgrp /opt/isel/tvs
```

## Construção de um serviço `systemd` que recebe instruções num *Unix Domain Socket*

Foram editados os ficheiros:

- `/tvsctl-cli/srv/tvsctl.c`
- `/tvsctl-srv/src/tvsctld.c`

### Why does the client (tvsctl) create a Unix socket?

The client creates a Unix domain socket for the following reasons:

1. Interprocess Communication (IPC): The client (tvsctl) needs a way to send commands to the daemon (tvsctld). A Unix domain socket is a common IPC mechanism that allows processes on the same machine to communicate efficiently.

2. Socket Communication: When the client runs, it connects to the daemon via the pre-configured socket path (/run/isel/tvsctld/request). Once connected, the client sends the command string to the daemon, which processes it.

### Aspetos chave da implementação

1. Client (`tvsctl.c`)

	- O cliente tem de criarum *Unix socket* para se ligar ao  *daemon's socket* e enviar comandos.
	- Realiza as seguintes tarefaz:
		* Abre um  *Unix socket*.
		* Liga-se ao *socket* no *path* especificado (SOCKET_PATH).
		* Envia a *string* *command* para o daemon.
		* Encerra o *socket*.
	- O tempo de vida do *socket* do cliente é temporário e usado apenas durante a execução do comando. Liga-se ao *socket* do servidor e é destruído assim que a comunicação termina

2. Daemon (`tvsctld.c`)

- O *daemon* está à escuta de pedidos de ligação ao *socket Unix*.
- Quando o cliente se liga, o *daemon*:
	- Lê o comando enviado pelo cliente.
	- Executa o *script* correspondente. ```fork``` e ```execvp```
	
3. Permições:

	- O *socket Unix* (`/run/isel/tvsctld/request`) tem de ter * ownership* e permissoẽs apropriadas (e.g., root-owned and group-readable by tvsgrp) para prevenir acesso não autorizado.

4. *Daemon Socket Cleanup*:

- O *daemon* deve fazer *clean up* da *socket file* (`unlink(SOCKET_PATH)`) ao sair para evitar problemas com *socket* obsoletos.

### Copia dos ficheiros depois de compilados para `/opt/isel/`

```bash
sudo mkdir -p /opt/isel/tvs/tvsctl-cli/bin/
sudo cp ./tvsctl-cli/src/tvsctl-cli/bin/tvsctl /opt/isel/tvs/tvsctl-cli/bin/
sudo chwon isel:tvsgrp /opt/isel/tvs/tvsctl-cli/bin/tvsctl
sudo cp ./tvsctl-srv/src/tvsctld /opt/isel/tvs/tvsctl-src/bin/
```

### Edição do ficheiro `install-all.sh`

No ficheiro foram metidos todos os passos aqui referidos
 