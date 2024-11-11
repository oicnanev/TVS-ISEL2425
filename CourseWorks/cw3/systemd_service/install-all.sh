#!/bin/bash

# Install directories and permissions
sudo mkdir -p /opt/isel/tvs/scripts /run/isel/tvsctld
sudo cp /path/to/repository/scripts/* /opt/isel/tvs/scripts/
sudo chmod +x /opt/isel/tvs/scripts/*.sh
sudo chown root:tvsgrp /opt/isel/tvs/scripts/
sudo chmod 750 /opt/isel/tvs/scripts/

# Copy compiled daemon and client to /opt
sudo cp /path/to/tvsctld /opt/isel/tvs/
sudo cp /path/to/tvsctl /usr/local/bin/
sudo chown root:tvsgrp /opt/isel/tvs/tvsctld /usr/local/bin/tvsctl
sudo chmod 750 /opt/isel/tvs/tvsctld /usr/local/bin/tvsctl

# Set up systemd service and socket unit files
sudo cp /path/to/repository/tvsctld.service /etc/systemd/system/
sudo cp /path/to/repository/tvsctld.socket /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable tvsctld.socket
sudo systemctl start tvsctld.socket
