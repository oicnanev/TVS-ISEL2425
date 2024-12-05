# Use the Node.js alpine image as the base image
FROM node:alpine

# Set the working directory
WORKDIR /home/node/app

# Copy all except tvsapp.js to the working directory, upgrade the system and install dependencies
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

