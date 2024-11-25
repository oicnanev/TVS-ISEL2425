#!/bin/bash

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
if ! dkpg -l | grep elasticsearch &> /dev/null; then
	echo "instaling elasticsearch..."
	wget https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-8.15.3-amd64.deb
	dkpg -i elasticsearch*.deb
	rm elasticsearch*.deb
fi
if grep -q "xpack.security.enabled: true" /etc/elasticsearch/elasticsearch.yml; then
	sed -i 's/xpack.security.enabled: true/xpack.security.enabled: false/' /etc/elasticsearch/elasticsearch.yml
fi	

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
cp ./tvsctl-srv/etc/nginx/sites-available/tvsapp /etc/nginx/nginx/sites-available/
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
mkdir -p /opt/isel/tvs/tvsctld/bin/tvsctld
cp -r ./tvsctl-srv/bin/scripts /opt/isel/tvs/tvsctld/bin/
cp ./tvsctl-srv/etc/service/tvsctld.service /etc/systemd/system/
gcc ./tvsctl-srv/src/tvsctld.c -o /opt/isel/tvs/tvsctld/bin/tvsctld
systemctl daemon-reload
systemctl start tvsctld.service

# tvscli ----------------------------------------
echo "installing tvscli..."
gcc ./tvsctl-cli/src/tvsctl.c -o /opt/isel/tvs/tvsctl/tvsctl
# TODO: escrever em etc/profile o opt/isel/tvs/tvsctl/tvsctl e depois source

echo "done"