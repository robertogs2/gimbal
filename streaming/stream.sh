#!/bin/bash
### RPI3 Stream JPEG
gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480,framerate=\(fraction\)30/1 ! videoconvert ! jpegenc ! tcpserversink host=10.42.0.252 port=5000

### Ubuntu stream client JPEG
#gst-launch-1.0 -v tcpclientsrc host=10.42.0.252 port=5000 ! jpegdec ! videoconvert ! autovideosink sync=false