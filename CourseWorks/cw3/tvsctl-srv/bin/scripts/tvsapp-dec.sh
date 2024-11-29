#!/bin/bash

# Default parameter for delta
DELTA=${1:-1}

# Configuration file path
CONFIG_FILE="/etc/nginx/sites-available/tvsapp"

if ! [[ -f "$CONFIG_FILE" ]]; then
    echo "Configuration file not found: $CONFIG_FILE"
    exit 1
fi

# Get a list of current ports used by tvsapp@ in the configuration
PORTS=($(awk '/server 127.0.0.1:/ {
    split($2, port, ":");
    gsub(";", "", port[2]);
    ports[port[2]] = 1
}
END {
    for (p in ports) print p
}' "$CONFIG_FILE" | sort -n))

# Ensure there are enough instances to remove
TOTAL_INSTANCES=${#PORTS[@]}
if ((TOTAL_INSTANCES <= 1)); then
    echo "Only one instance left, cannot remove any more."
    exit 1
fi

# Calculate how many instances to actually remove, ensuring at least one remains
REMOVABLE_INSTANCES=$((TOTAL_INSTANCES - 1))
DELTA=$((DELTA > REMOVABLE_INSTANCES ? REMOVABLE_INSTANCES : DELTA))

# Select the highest ports to remove
PORTS_TO_REMOVE=("${PORTS[@]: -DELTA}")

# Update the config file by removing the selected servers from the `upstream tvsapp` block
echo "Removing $DELTA instances with the highest ports from $CONFIG_FILE"
awk -v ports_to_remove="${PORTS_TO_REMOVE[*]}" '
BEGIN {
    split(ports_to_remove, remove_ports, " ")
    for (i in remove_ports) ports[remove_ports[i]] = 1
    in_upstream_block = 0
}
/upstream tvsapp_backend/ {
    in_upstream_block = 1
    print $0
    next
}
in_upstream_block && /}/ {
    in_upstream_block = 0
    print $0
    next
}
in_upstream_block {
    split($2, port, ":")
    gsub(";", "", port[2])
    if (!(port[2] in ports)) print $0
    next
}
{
    print $0
}
' "$CONFIG_FILE" >"$CONFIG_FILE.tmp" && mv "$CONFIG_FILE.tmp" "$CONFIG_FILE"

# Stop the selected tvsapp@ services
echo "Stopping $DELTA tvsapp instances with ports: ${PORTS_TO_REMOVE[*]}"
for PORT in "${PORTS_TO_REMOVE[@]}"; do
    echo "Stopping tvsapp@${PORT}.service"
    systemctl stop "tvsapp@${PORT}.service"
done

# Reload nginx
echo "Reloading nginx..."
systemctl reload nginx

echo "tvsapp instances reduced by $DELTA, removed ports: ${PORTS_TO_REMOVE[*]}"
