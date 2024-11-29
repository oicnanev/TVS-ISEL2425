#!/bin/bash

# Default parameters
SCALE=${1:-1}
BASE=${2:-35000}

# Stop all tvsapp@ services
echo "Stopping all tvsapp@ services..."
systemctl stop 'tvsapp@*.service'

# tvsapp nginx config file
CONFIG_FILE="/etc/nginx/sites-available/tvsapp"

if ! [[ -f "$CONFIG_FILE" ]]; then
    echo "Configuration file not found: $CONFIG_FILE"
    exit 1
fi

# Remove previous server and add new ones to the `upstream tvsapp` block
echo "Updating $CONFIG_FILE with scale=$SCALE and base=$BASE"
awk -v scale="$SCALE" -v base="$BASE" '
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
    # Adds new servers with with ports starting on `base` and up to `scale`
    for (i = 0; i < scale; i++) {
        print "    server 127.0.0.1:" base + i ";"
    }
    print $0
    next
}
!in_upstream_block {
    print $0
}
' "$CONFIG_FILE" >/tmp/tvsapp_nginx_config && mv /tmp/tvsapp_nginx_config "$CONFIG_FILE"

# Reload nginx
echo "Reloading nginx..."
systemctl reload nginx

echo "tvsapp reset completed with scale=$SCALE and base=$BASE"
