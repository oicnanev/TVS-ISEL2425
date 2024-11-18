#!/bin/bash

# Optionally stop elasticsearch if -db argument is provided
if [[ "$1" == "-db" ]]; then
  systemctl stop elasticsearch
  echo "elasticsearch stopped."
fi

# Disable the Nginx configuration for the site
rm -f /etc/nginx/sites-enabled/tvsapp
systemctl reload nginx
echo "nginx tvsapp site configuration deactivated."

# Stop all tvsapp@.... services
systemctl stop tvsapp@*
echo "all instances of tvsapp services stopped"
