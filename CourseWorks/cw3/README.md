## Preparação

Para a **etapa ii** de configurar a aplicação web como um serviço e executar 4 instâncias em portas específicas, foram efetuados estes passos:

1. Copiar a aplicação para o novo local

```bash
sudo mkdir -p /opt/isel/tvs/tvsapp
sudo cp -r cw3/tvsapp/app /opt/isel/tvs/tvsapp/
```

2. Criar um ficheiro de Serviço para cada instância

```bash
cd /etc/systemd/system
sudo touch tvsapp43211.service
sudo touch tvsapp43212.service
sudo touch tvsapp43213.service
sudo touch tvsapp43214.service
```

Cada arquivo de serviço deve seguir o modelo abaixo, substituindo o PORT_NUMBER pela porta específica (43211, 43212, etc.):

```ini
# /etc/systemd/system/tvsappPORT_NUMBER.service
[Unit]
Description=TVS App Instance on Port PORT_NUMBER
After=network.target elasticsearch.service

[Service]
Environment=NODE_PORT=PORT_NUMBER
WorkingDirectory=/opt/isel/tvs/tvsapp/app
ExecStart=/usr/bin/npm start
Restart=always
User=www-data
Group=www-data

[Install]
WantedBy=multi-user.target
```


3. Fazer *reload* ao **systemd** e iniciar as instâncias

```bash
sudo systemctl daemon-reload
sudo systemctl start tvsapp43211
sudo systemctl start tvsapp43212
sudo systemctl start tvsapp43213
sudo systemctl start tvsapp43214
```

Para os serviços arrancarem com o *boot* do sistema operativo

```bash
sudo systemctl enable tvsapp43211
sudo systemctl enable tvsapp43212
sudo systemctl enable tvsapp43213
sudo systemctl enable tvsapp43214
```

4. Verificar se estão a funcionar corretamente:

```bash
Copy code
sudo systemctl status tvsapp43211
sudo systemctl status tvsapp43212
sudo systemctl status tvsapp43213
sudo systemctl status tvsapp43214
```

Caso exista algum erro, depurar, no meu caso não tinha o *user isel*:

```bash
sudo useradd isel
```

Repetir os comandos `enable` e `status`

Por fim, abrir um browser e navegar por cada um dos URLs:

http://localhost:43211
http://localhost:43212
http://localhost:43213
http://localhost:43214

Se tudo foi configurado corretamente, cada URL deve exibir a aplicação a trabalhar na respetiva porta.


Para a **etapa iii** de configurar o Nginx como um *load balancer* para as instâncias da aplicação tvsapp, seguimos os passos abaixo:

1. Criação do ficheiro de configuração do novo *site*
Navegue até o diretório /etc/nginx/sites-available:

```bash
cd /etc/nginx/sites-available
```

Criação de um novo ficheiro de configuração para o *site* tvsapp na porta 44444:

```bash
sudo nano tvsapp
```

No ficheiro tvsapp, adicionou-se a configuração de *load balancer* para distribuir os pedidos entre as instâncias tvsapp nas portas 43211, 43212, 43213 e 43214:

```nginx
upstream tvsapp_backend {
    server localhost:43211;
    server localhost:43212;
    server localhost:43213;
    server localhost:43214;
}

server {
    listen 44444;
    server_name localhost;

    location / {
        proxy_pass http://tvsapp_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

O bloco **upstream tvsapp_backend** define o grupo de servidores (instâncias tvsapp) que atuarão como backend para o *load balancer*.

O bloco **server** configura o Nginx para escutar no porto 44444 e redirecionar os pedidos para o grupo tvsapp_backend.

2. Ativação  da nova configuração do site

Criar um link simbólico na pasta `/etc/nginx/sites-enabled` para ativar o novo site:

```bash
sudo ln -s /etc/nginx/sites-available/tvsapp /etc/nginx/sites-enabled/
```

Verificar se a configuração do Nginx contém erros de sintaxe:

```bash
sudo nginx -t
```

Se houver algum erro, corrigir o ficheiro `tvsapp` antes de continuar.

Reiniciar o serviço Nginx para aplicar as novas configurações:

```bash
sudo systemctl restart nginx
```

3. Testar o *Load balancer*

Abrir o browser e aceder a http://localhost:44444. 

Ao atualizar a página, a saída deve estar a alternar entre as instâncias nas portas 43211, 43212, 43213, e 43214.



