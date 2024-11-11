# Permissoes, adicionar user e grupo

```bash
sudo groupadd tvsgrp
sudo usermod -aG tvsgrp isel
sudo chmod 770 /run/isel/tvsctld/request
```

## Para testar

```bash
sudo systemctl start tvsctld.socket
```

Usar o comando `tvsctl` para enviar pedidos ao servidor.

tvsctl reset - reinicia o servidor
tvsctl status - mostra o estado do servidor
tvsctl stop - termina o servidor
etc....
