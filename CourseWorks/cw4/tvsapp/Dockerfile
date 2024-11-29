# Use the Node.js alpine image as the base image
FROM node:alpine

# Set the working directory
WORKDIR /home/node/app

# Copy package.json and package-lock.json to the working directory and install dependencies
COPY package*.json ./
RUN npm install

# Copy the rest of the application code to the working directory
COPY . .

# Change ownership of all files and directories to node user
RUN chown -R node:node /home/node

# Set the environment variable NODE_PORT to 8004
ENV NODE_PORT=8004

# Expose the port 8004
EXPOSE 8004

# Use the node user to run the application
USER node

# Set the entrypoint to always run `node tvsapp.js`
ENTRYPOINT ["node", "tvsapp.js"]
