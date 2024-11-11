#!/bin/bash

# Start Elasticsearch
sudo systemctl start elasticsearch
echo "Elasticsearch started."

# Enable Nginx configuration for the site
sudo ln -sf /etc/nginx/sites-available/tvsapp /etc/nginx/sites-enabled/tvsapp
sudo systemctl reload nginx
echo "Nginx site configuration activated."

# Start all instances on configured ports (43211 - 43214)
for port in {43211..43214}; do
  NODE_PORT=$port nohup npm start --prefix /opt/isel/tvs/tvsapp/app >/dev/null 2>&1 &
  echo "Started instance on port $port"
done
