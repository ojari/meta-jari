FILESEXTRAPATHS:prepend := "${THISDIR}/mosquitto:"

SRC_URI += "file://mosquitto.conf"

do_install:append() {
    install -d ${D}/etc/mosquitto
    install -m 0644 ${WORKDIR}/mosquitto.conf ${D}/etc/mosquitto/
}