## First step: Create the docker-compose.yml file:

```yaml
version: '3.9'

services:
  entry:
    image: nginx:alpine
    ports:
      - "2024:80"
    volumes:
      - ./nginx/default.conf:/etc/nginx/conf.d/default.conf
    depends_on:
      - webapp

  webapp:
    build:
      context: ./tvsapp
    environment:
      - NODE_PORT=8004
    expose:
      - "8004"
    depends_on:
      - storage

  storage:
    image: elasticsearch:8.16.1
    environment:
      - discovery.type=single-node
      - xpack.security.enabled=false
    volumes:
      - esdata:/usr/share/elasticsearch/data

volumes:
  esdata:
    driver: local
```


### Explanation

- **Version**: Specifies the version of Docker Compose.
- **Services**:
	* **entry**: Uses the `nginx:alpine` image. It binds port 2024 on the host to port 80 on the container and uses a modified configuration file.
	* **webapp**: Built from the Dockerfile in the `./tvsapp` directory. It sets the `NODE_PORT` environment variable, exposes port 8004, and depends on the `storage` service.
	* **storage**: Uses the `elasticsearch:8.16.1 image, sets the necessary environment variables, and mounts a volume to persist data.
- **Volumes**:
	* **esdata**: A named volume to ensure Elasticsearch data persistence.

### Default Nginx Configuration (default.conf)

Create a file `nginx/default.conf with the following content:

```nginx
upstream webapp {
    server webapp:8004;
}

server {
    listen 80;

    location / {
        	set $TVSSVC http://webapp:8000;
			proxy_pass $TVSSVC;
			resolver 127.0.0.11 valid=5s;
    }
}
```

This configuration file sets up Nginx to proxy requests to the `webapp` service.

## Running the Solution

1. Navigate to the cw4 directory:

```sh
cd cw4
```

2. Start the services:

```sh
docker compose up -d
```

3. Scale the `webapp` service:

```sh
docker compose up -d --scale webapp=4 --no-recreate
```

## Access Control

- `entry` can access `webapp`.
- `webapp` can access `storage`.
- `entry` cannot directly access `storage` because there's no networking setup between them.

This setup will allow you to run the solution with a single port (2024) for all incoming requests, and you can scale the number of `webapp` replicas without affecting the other services.

