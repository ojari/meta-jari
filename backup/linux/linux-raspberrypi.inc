require linux.inc
inherit linux-raspberrypi-base

DESCRIPTION = "Linux Kernel for Raspberry Pi"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

SRC_URI += " \
        file://defconfig \
        "

COMPATIBLE_MACHINE = "raspberrypi"

PE = "1"
PV = "${LINUX_VERSION}+git${SRCPV}"

# CMDLINE for raspberrypi
CMDLINE = "dwc_otg.lpm_enable=0 console=ttyAMA0,115200 root=/dev/mmcblk0p2 rootfstype=ext4 rootwait"

UDEV_GE_141 ?= "1"

# Set programmatically some variables during recipe parsing
# See http://www.yoctoproject.org/docs/current/bitbake-user-manual/bitbake-user-manual.html#anonymous-python-functions
python __anonymous () {
    kerneltype = d.getVar('KERNEL_IMAGETYPE', True)
    kerneldt = get_dts(d, d.getVar('LINUX_VERSION', True))
    d.setVar("KERNEL_DEVICETREE", kerneldt)

    # Add dependency to 'rpi-mkimage-native' package only if RPi bootloader is used with DT-enable kernel
    if kerneldt:
        if kerneltype != 'uImage' and len(kerneldt.strip()) > 1:
            depends = d.getVar("DEPENDS", True)
            depends = "%s rpi-mkimage-native" % depends
            d.setVar("DEPENDS", depends)
}

do_install_prepend() {
    #install -d ${D}/lib/firmware
}

do_deploy_append() {
    # Deploy cmdline.txt
    install -d ${DEPLOYDIR}/bcm2835-bootfiles
    echo "${CMDLINE}" > ${DEPLOYDIR}/bcm2835-bootfiles/cmdline.txt
}

do_rpiboot_mkimage() {
    if test "x${KERNEL_IMAGETYPE}" != "xuImage" ; then
        if test -n "${KERNEL_DEVICETREE}"; then
            # Add RPi bootloader trailer to kernel image to enable DeviceTree support
            ${STAGING_DIR_NATIVE}/usr/lib/rpi-mkimage/mkknlimg --dtok ${KERNEL_OUTPUT} ${KERNEL_OUTPUT}
        fi
    fi
}
addtask rpiboot_mkimage before do_install after do_compile
