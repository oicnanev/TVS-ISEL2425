#!/bin/bash

# start elasticsearch if not running ---------------------------------------
echo "Starting elasticsearch..."
systemctl start elasticsearch

# start nginx if not running -----------------------------------------------
echo "Starting nginx..."
ln -s /etc/nginx/sites-available/tvsapp /etc/nginx/sites-enabled/ &> /dev/null # does nothing if file exists, supresses errors (file exists)
systemctl start nginx  # does nothing if already started

# start all tvsapp instances -----------------------------------------------
CONFIG_FILE="/etc/nginx/sites-enabled/tvsapp"  # path for nginx tvsapp conf

# Extract the number of the ports from the block upstream tvsapp
PORTS=$(awk '/upstream tvsapp_backend {/,/}/ {
    if ($1 == "server") {
        split($2, a, ":");
        print a[2]
    }
}' "$CONFIG_FILE" | tr -d ';')

# Check if any port found
if [ -z "$PORTS" ]; then
    echo "No ports found in the 'upstream tvsapp_backend' block in the configuration file."
    exit 1
fi

echo "Starting tvsapp@ services..."
# For each port founded, start the tvsapp@[port] service
for port in $PORTS; do
    systemctl start tvsapp@"$port".service
    if [ $? -eq 0 ]; then
        echo "tvsapp@$port started."
    else
        echo "Fail to start the service tvsapp@$port."
    fi
done

echo "Reloading nginx..."
systemctl reload nginx # reload nginx configuration
