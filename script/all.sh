#!/bin/bash -x
#scp ../wpa_supplicant.conf to the /etc/wpa_supplicant.conf in the rasp
mknod /dev/spi c 60 0
chmod 666 /dev/spi
insmod /lib/modules/extra/spi.ko

ifup wlan0
myip="$(ifconfig | grep -A 1 'wlan0' | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1)"
### RPI3 Stream JPEG
./usr/bin/main & gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480,framerate=\(fraction\)30/1 ! videoconvert ! jpegenc ! tcpserversink host=$myip port=5000






### Ubuntu stream client JPEG
#gst-launch-1.0 -v tcpclientsrc host=10.42.0.252 port=5000 ! jpegdec ! videoconvert ! autovideosink sync=false

# ###
# ### RPI3 Stream JPEG
# gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480,framerate=\(fraction\)30/1 ! videoconvert ! jpegenc ! tcpserversink host=192.168.43.21 port=5000

# ### Ubuntu stream client JPEG
# #gst-launch-1.0 -v tcpclientsrc host=10.42.0.252 port=5000 ! jpegdec ! videoconvert ! autovideosink sync=false