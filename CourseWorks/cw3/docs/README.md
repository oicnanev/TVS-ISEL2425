## ISEL - System Virtualization Techniques
### Coursework Assignment 3

#### Instalation

To install our solution, run the `install-all.sh` script

```bash
sudo ./install-all-sh
```

Used that way the, the script assumes, that is already installed on the system, nodejs, npm, nginx and elasticsearch.

This will add the **tvsgrp** group to the system and add the current user to that group, install the nodejs dependencies to run the **TVSapp**, configure **nginx** as a reverse proxy server listening at the TCP port 44444, create a *unix socket* to connect the service **tvsctld** that run as a *daemon* and a program to send commands to that *daemon*, **tvsctl** runable by users that belong to the tvsgrp group.

##### Pre setup (nginx, nodejs, elasticsearch)

The script can, in case of missing all or some of the needed packages to run the tvs solution, install them for us:

```bash
sudo ./install-all.sh --setup
```

#### Desinstalation

To unistall the tvs solution, run the script `uninstall-all.sh`

```bash
sudo ./uninstall-all.sh
```

This will only uninstall the tvs solution (unix socket, daemon and tvsctl), will not remove the **tvsgrp** group, nor uninstall the packages needed for setup.

##### Remove tvsgrp

To enable the tvsgrp removal, the script allow the passing of the argument `--group`

```bash
sudo ./uninstall-all.sh
``` 

##### Remove setup packages

The uninstalation of the setup packages is not supported in the bash script. The uninstall the setup packages run:

```bash
sudo apt remove -y nginx nodejs npm elasticsearch
sudo apt autoclean -y && sudo apt autoremove -y
```




