# #!/bin/bash

# Set defaults for scale and base port
scale=${1:-1}
base=${2:-35000}

# Stop all existing instances
for pid in $(pgrep -f "NODE_PORT"); do
  kill -9 "$pid" 2>/dev/null
done

# Start instances based on the scale
for ((i = 0; i < scale; i++)); do
  port=$((base + i))
  NODE_PORT=$port nohup npm start --prefix /opt/isel/tvs/tvsapp/app >/dev/null 2>&1 &
  echo "Started instance on port $port"
done

echo "Configured $scale instance(s) starting at base port $base."
