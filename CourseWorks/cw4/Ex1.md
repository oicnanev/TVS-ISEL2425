## Step 1 Create tvsapp folder and the Dockerfile

First we copied the folder /cw3/tvsapp/ to /cw4/

Inside the `cw4` folder we created the `Dockerfile`:

```Dockerfile
# Use the Node.js alpine image as the base image
FROM node:alpine

# Set the working directory
WORKDIR /home/node/app

# Copy all except tvsapp.js to the working directory and install dependencies
COPY ./tvsapp/app/files ./files/ 
COPY ./tvsapp/app/package.json . 
RUN apk update && \
	apk add --upgrade apk-tools && \
	apk upgrade --available && \
   apk add fortune && \
	npm install -g npm@latest && \
	npm install

# Set the environment variable NODE_PORT to 8004
ENV NODE_PORT=8004

# Expose the port 8004
EXPOSE 8004

# Copy tvsapp.js (usually is updated more usuall than the rest of the files)
COPY ./tvsapp/app/tvsapp.js .

# Change ownership of all files and directories to node user
RUN chown -R node:node /home/node

# Use the node user to run the application
USER node

# Set the entrypoint to always run `node tvsapp.js`
ENTRYPOINT ["node", "tvsapp.js"]
```

To ensure that `node tvsapp.js` **always** runs when the container starts, we choose to use `ENTRYPOINT` instead of `CMD`.

To run fortune in the webapp, we modified one line in the tvsapp.js:

```js
///////////////////////////////////////////////////////
//
// Get fortune: 
//   - runs 'fortune' and collects what gets written
//     to standard output
//

async function getFortune() {
	return new Promise((resolve) => {
		// for alpine
		exec("/usr/bin/fortune", (error, stdout, stderr) => { resolve(stdout) })
		// for ubuntu
		//exec("/usr/games/fortune", (error, stdout, stderr) => { resolve(stdout) })			
	})
}
```

In alpine linux, the fortune executable is not placed in the same place as in ubuntu.

By organizing the Dockerfile efficiently, we try to minimize the number of layers and optimize the build process to better utilize the cache.
This setup ensures that the dependency installation step (`npm install`) is only re-run when there are changes to `tvsapp.js. Any changes to the application code will trigger a rebuild of the subsequent layers, but the layers related to dependencies will remain cached, leading to faster builds and a more efficient workflow.

## Step 2: Build the Docker image

Inside the `cw4` directory, build the Docker image with the following command:

```sh
$ docker build -t tvsapp .
```

## Step 3: Run the Containers

Run four instances of the `tvsapp container, making the application available on different ports (12001, 12002, 12003, 12004) of the host system:

```sh
$ docker run -d -p 12001:8004 --name tvsapp_12001 tvsapp
$ docker run -d -p 12002:8004 --name tvsapp_12002 tvsapp
$ docker run -d -p 12003:8004 --name tvsapp_12003 tvsapp
$ docker run -d -p 12004:8004 --name tvsapp_12004 tvsapp
```

- `-d` Run container in background and print container ID
- `-p [host_port]:[container_port]` Publish a container's port(s) to the host
- `--name [string]` Assign a name to the container 

## Explanation of HOST and PORT

In each container:

- **HOST**: Refers to the container's internal network address (e.g., localhost inside the container). However, each container's network is isolated from the others unless linked explicitly.
- **PORT**: The internal port 8004 is exposed to the host's ports 12001, 12002, 12003, and 12004, respectively. This allows accessing the application on these different ports through the host's network interface.

Each instance of the application listens on port 8004 internally within the container, but due to Docker's port mapping, they're accessible through different ports on the host machine.