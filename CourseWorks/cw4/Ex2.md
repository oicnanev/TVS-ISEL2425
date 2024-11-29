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

## Running the solution

To run the solution and ensure it uses all the replicas to process incoming requests, follow these steps:

### 1. Start the services:

```sh
docker compose up -d
```

### 2. Scale the webapp service:

```sh
docker compose up -d --scale webapp=4 --no-recreate
```

### 3.Confirm the solution is using all replicas:

```sh
seq 32 | xargs -I{} curl -s http://localhost:2024/ | grep "HOST" | sed "s/<\/\?[a-z]\+>//g" | sed "s/^[[:space:]]*//" | sort | uniq -c
```


## Additional Tasks

### 1. Check Service Logs:

```sh
docker compose logs -f
```

This command will follow the logs for all services in real time.

### 2. Run a Shell in Any Container:

```sh
docker exec -it <container_name> sh
```

Replace `<container_name>` with the name of the container (e.g., `webapp_1`). Inside the container, you can get the IP address and observe the running processes:

```sh
ifconfig
ps aux
```

### 3. Demonstrate Connectivity and Unreachability:

- From `entry` to `webapp`:

```sh
docker exec -it entry sh
ping webapp
```

- From `webapp` to `storage`:

```sh
docker exec -it webapp_1 sh
ping storage
```

- Attempt from `entry` to `storage` (should fail):

```sh
docker exec -it entry sh
ping storage
```

## Nginx Configuration Explanation

1. **http://webapp:PORT is Enough to Reach All Replicas**:
	- In the `nginx` configuration, `upstream webapp { server webapp:8004; }` defines a backend group named `webapp` that points to the `webapp` service on port 8004. Docker's internal DNS will resolve `webapp` to all instances of the `webapp` service, distributing requests among them.

2. **Purpose of `resolver 127.0.0.11 valid=5s`**:
	- The line `resolver 127.0.0.11 valid=5s`; in the Nginx configuration is used to specify the DNS resolver for Nginx. `127.0.0.11` is the default DNS resolver provided by Docker, which resolves service names to their IP addresses. The `valid=5s` part means that the resolved IP addresses are considered valid for 5 seconds. This helps in dynamic environments where services may scale up or down, ensuring Nginx always uses up-to-date addresses for the `webapp` service.
