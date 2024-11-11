#!/bin/bash

# Set default delta to 1
delta=${1:-1}

# Get a list of ports currently in use by Node.js instances, sorted in descending order
ports=($(pgrep -fl "NODE_PORT" | awk '{print $NF}' | sed 's/[^0-9]*//g' | sort -nr))

# Ensure we keep at least one instance running
if ((${#ports[@]} <= delta)); then
  echo "Cannot stop $delta instances. Only ${#ports[@]} running, minimum 1 required."
  exit 1
fi

# Stop delta instances with the highest ports
for ((i = 0; i < delta; i++)); do
  pid=$(pgrep -f "NODE_PORT=${ports[$i]}")
  kill -9 "$pid" 2>/dev/null
  echo "Stopped instance on port ${ports[$i]}"
done

echo "Decreased by $delta instance(s)."
