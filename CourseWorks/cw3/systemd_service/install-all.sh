#!/bin/bash

# Install directories and permissions
mkdir -p /opt/isel/tvs/scripts /run/isel/tvsctld
cp ./scripts/* /opt/isel/tvs/scripts/
chmod +x /opt/isel/tvs/scripts/*.sh
chown root:tvsgrp /opt/isel/tvs/scripts/
chmod 750 /opt/isel/tvs/scripts/

# Copy compiled daemon and client to /opt
cp /path/to/tvsctld /opt/isel/tvs/
cp /path/to/tvsctl /usr/local/bin/
chown root:tvsgrp /opt/isel/tvs/tvsctld /usr/local/bin/tvsctl
chmod 750 /opt/isel/tvs/tvsctld /usr/local/bin/tvsctl

# Set up systemd service and socket unit files
cp /path/to/repository/tvsctld.service /etc/systemd/system/
cp /path/to/repository/tvsctld.socket /etc/systemd/system/
systemctl daemon-reload
systemctl enable tvsctld.socket
systemctl start tvsctld.socket
