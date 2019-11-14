SUMMARY = "Linux module for BCM2835 timer"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=e49f4652534af377a713df3d9dec60cb"
inherit module
SRC_URI = "file://Makefile \
			file://spi.c \
			file://COPYING \
			"
S = "${WORKDIR}"
# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
