#!/bin/bash

# machine user
CURRENT_USER=$(logname)
echo $CURRENT_USER

# setup -----------------------------------
echo "updating package manager..."
apt update
if ! npm -v npm -v &> /dev/null; then
	echo "installing node package manager..."
	apt install npm
fi
if ! node -v &> /dev/null; then
	echo "instaling nodejs..."
	apt install nodejs
fi
if ! nginx -v &> /dev/null; then
	echo "instaling nodejs..."
	apt install nginx
fi
if ! dpkg -l | grep elasticsearch &> /dev/null; then
	echo "instaling elasticsearch..."
	wget https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-8.15.3-amd64.deb
	dkpg -i elasticsearch*.deb
	rm elasticsearch*.deb
fi
if grep -q "xpack.security.enabled: true" /etc/elasticsearch/elasticsearch.yml; then
	sed -i 's/xpack.security.enabled: true/xpack.security.enabled: false/' /etc/elasticsearch/elasticsearch.yml
fi	

# group tvsgrp and add current user to group
echo "1"
groupadd tvsgrp &> /dev/null
echo "2"
usermod -aG tvsgrp "$CURRENT_USER"
echo "3"
# Apply group changes immediately for the current session
#exec sg tvsgrp newgrp
echo "4"

# tvsapp ----------------------------------
echo "copying app to /opt/..."
mkdir -p /opt/isel/tvs/tvsapp
cp -r ./tvsapp/app /opt/isel/tvs/tvsapp/
chown -R isel:isel /opt/isel
echo "instaling node tvsapp dependencies..."
npm install --prefix /opt/isel/tvs/tvsapp/app

# start elasticsearch ----------------------
echo "starting elasticsearch..."
systemctl start elasticsearch
systemctl enable nginx

# tvsapp service template -------------------
echo "copy tvsapp@ service to systemd..."
cp ./tvsapp/etc/service/tvsapp@.service /etc/systemd/system/
systemctl daemon-reload

# nginx --------------------------------------
echo "configuring nginx..."
cp ./tvsctl-srv/etc/nginx/sites-available/tvsapp /etc/nginx/sites-available/
ln -s /etc/nginx/sites-available/tvsapp /etc/nginx/sites-enabled/
systemctl enable nginx
systemctl start nginx
systemctl reload nginx  # case already been running

# tvsapp services -----------------------------
# TODO: possivelmente em vez de fazer isto basta chamar o script start!!!
echo "starting web app instances..."
CONFIG_FILE="/etc/nginx/sites-available/tvsapp"
PORTS=$(awk '/upstream tvsapp_backend {/,/}/ {
	if ($2 ~ /server/) {
		split($2, a, ":");
		print a[2]
	}
}' "$CONFIG_FILE" | tr -d ";")

for port in $PORTS; do
	systemctl start tvsapp@"$port"
	if [$? -eq 0]; then
		echo "tvsapp@$port running"
	else
		echo "tvsapp@$port failed to start"
	fi
done

systemctl reload nginx

# tvsctld --------------------------------------
echo "configuring tvsctld daemon..."
mkdir -p /opt/isel/tvs/tvsctld/bin
cp -r ./tvsctl-srv/bin/scripts /opt/isel/tvs/tvsctld/bin/
cp ./tvsctl-srv/etc/service/tvsctld.service /etc/systemd/system/
gcc ./tvsctl-srv/src/tvsctld.c -o /opt/isel/tvs/tvsctld/bin/tvsctld
systemctl daemon-reload
systemctl start tvsctld.service

# tvsctl ----------------------------------------
echo "installing tvsctl..."
mkdir -p /opt/isel/tvs/tvsctl/
gcc ./tvsctl-cli/src/tvsctl.c -o /opt/isel/tvs/tvsctl/tvsctl
NEW_PATH="/opt/isel/tvs/tvsctl"
ENV_FILE="/etc/environment"
if grep -q "$NEW_PATH" "$ENV_FILE"; then
	echo "The directory $NEW_PATH is already on the PATH."
else
	sed -i.bak "s|^PATH=\"\(.*\)\"|PATH=\"\1:$NEW_PATH\"|" "$ENV_FILE"
  	echo "Added $NEW_PATH to PATH."
  	#echo "Backup of previous file: $ENV_FILE.bak"
  	source /etc/environment
fi
echo "done"
