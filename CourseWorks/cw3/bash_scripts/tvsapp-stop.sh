#!/bin/bash

# Optionally stop Elasticsearch if -db argument is provided
if [[ "$1" == "-db" ]]; then
  sudo systemctl stop elasticsearch
  echo "Elasticsearch stopped."
fi

# Disable the Nginx configuration for the site
sudo rm -f /etc/nginx/sites-enabled/tvsapp
sudo systemctl reload nginx
echo "Nginx site configuration deactivated."

# Stop all running Node.js instances
for pid in $(pgrep -f "NODE_PORT"); do
  kill -9 "$pid" 2>/dev/null
done
echo "Stopped all web app instances."
