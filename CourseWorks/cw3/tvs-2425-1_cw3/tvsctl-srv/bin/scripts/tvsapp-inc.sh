#!/bin/bash

# Default parameter for delta
DELTA=${1:-1}

# Configuration file path
CONFIG_FILE="/etc/nginx/sites-enabled/tvsapp"

if ! [[ -f "$CONFIG_FILE" ]]; then
    echo "Configuration file not found: $CONFIG_FILE"
    exit 1
fi

# Find the highest port currently in use by tvsapp in the configuration
LAST_PORT=$(awk '/server localhost:/ {
    split($2, port, ":");
    gsub(";", "", port[2]);
    if (port[2] > max_port) max_port = port[2];
}
END {print max_port}' "$CONFIG_FILE")

# If no ports are found, set the base port to the default value (35000)
BASE=${LAST_PORT:-35000}

# Calculate the new ports range
NEW_BASE=$((BASE + 1))

# Update the config file by adding new servers to the `upstream tvsapp` block
echo "Adding $DELTA new instances to $CONFIG_FILE, starting at port $NEW_BASE"
awk -v delta="$DELTA" -v base="$NEW_BASE" '
BEGIN {
    in_upstream_block = 0
}
/upstream tvsapp_backend/ {
    in_upstream_block = 1
    print $0
    next
}
in_upstream_block && /}/ {
    in_upstream_block = 0
    # Keep existing servers and add the new ones
    for (i = 0; i < delta; i++) {
        print "    server localhost:" base + i ";"
    }
    print $0
    next
}
{
    print $0
}
' "$CONFIG_FILE" >/tmp/tvsapp_nginx_config && mv /tmp/tvsapp_nginx_config "$CONFIG_FILE"

# Start new tvsapp@ services for the new range of ports
echo "Starting $DELTA new tvsapp instances starting at port $NEW_BASE..."
for ((i = 0; i < DELTA; i++)); do
    PORT=$((NEW_BASE + i))
    echo "Starting tvsapp@${PORT}.service"
    systemctl start "tvsapp@${PORT}.service"
done

# Reload nginx
echo "Reloading nginx..."
systemctl reload nginx

echo "tvsapp instances incremented by $DELTA, starting from port $NEW_BASE"
