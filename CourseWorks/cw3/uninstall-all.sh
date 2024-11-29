#!/bin/bash

echo "removing tvs services and socket...."
rm -R /opt/isel
rm -R /run/isel
rm /etc/systemd/system/tvs*
systemctl daemon-reload
mv /etc/environment.bak /etc/environment
source /etc/environment

echo "removing tvsapp from nginx..."
rm /etc/nginx/sites-available/tvsapp
rm -f /etc/nginx/sites-enabled/tvsapp
systemctl reload nginx

if [[ "$1" == "--group" ]]; then
	echo "removing tvsgrp group..."
	groupdel tvsgrp
else
	echo "keeping tvsgrp..."
fi
echo "done"