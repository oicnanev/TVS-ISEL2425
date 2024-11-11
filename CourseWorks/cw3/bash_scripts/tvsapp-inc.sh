#!/bin/bash

# Set default delta to 1
delta=${1:-1}

# Find the highest port currently in use by Node.js instances
highest_port=$(pgrep -fl "NODE_PORT" | awk '{print $NF}' | sed 's/[^0-9]*//g' | sort -n | tail -1)
next_port=$((highest_port + 1))

# Start delta additional instances
for ((i = 0; i < delta; i++)); do
  NODE_PORT=$((next_port + i)) nohup npm start --prefix /opt/isel/tvs/tvsapp/app >/dev/null 2>&1 &
  echo "Started additional instance on port $((next_port + i))"
done

echo "Increased by $delta instance(s) starting from port $next_port."
