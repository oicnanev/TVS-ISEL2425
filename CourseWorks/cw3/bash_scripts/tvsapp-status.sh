#!/bin/bash

# Check Nginx status
if systemctl is-active --quiet nginx; then
  echo "Nginx: Running"
else
  echo "Nginx: Stopped"
fi

# Check Elasticsearch status
if systemctl is-active --quiet elasticsearch; then
  echo "Elasticsearch: Running"
else
  echo "Elasticsearch: Stopped"
fi

# List running Node.js instances with their ports
echo "Web App Instances:"
pgrep -fl "NODE_PORT" | awk '{print "Port: " $NF}'
