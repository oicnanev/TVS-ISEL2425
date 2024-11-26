#!/bin/bash

# Verbose status with argument '-v'
if [[ "$1" == "-v" ]]; then
	systemctl status nginx
	systemctl status elasticsearch
	systemctl status tvsapp@*
else
	# Check NginX Status
	if systemctl is-active --quiet nginx; then
  		echo "nginx: Running"
	else
  		echo "nginx: Stopped"
	fi

	# Check ElasticSearch Status
	if systemctl is-active --quiet elasticsearch; then
  		echo "elasticsearch: Running"
	else
  		echo "elasticsearch: Stopped"
	fi

	# Lists `tvsapp@*service`
	CONFIG_FILE="/etc/nginx/sites-enabled/tvsapp"  # path for nginx tvsapp conf

	# Extract the number of the ports from the block upstream tvsapp
	PORTS=$(awk '/upstream tvsapp_backend {/,/}/ {
	    if ($1 == "server") {
	        split($2, a, ":");
	        print a[2]
	    }
	}' "$CONFIG_FILE" | tr -d ';')

	# Check if any port is found
	if [ -z "$PORTS" ]; then
	    echo "No ports found in the 'upstream tvsapp' block in the configuration file."
	    exit 1
	fi

	# For each port found, checks the status of the tvsapp@[port] service
	for port in $PORTS; do
		if systemctl is-active --quiet tvsapp@"$port"; then
	        echo "tvsapp@$port: Running."
	    else
	        echo "tvsapp@$port: Stopped."
	    fi
	done
fi

