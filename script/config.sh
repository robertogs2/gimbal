myip=10.42.0.252
scp wpa_supplicant.conf root@$myip:/etc/wpa_supplicant.conf
scp spi.ko root@$myip:/lib/modules/extra/spi.ko
scp all.sh root@$myip:/home/root/all.sh
scp rc.local root@$myip:/etc/rc.local