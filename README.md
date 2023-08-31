# Ubus ESP Controler (RutOS Ready)

This project allows the user to control pins on an ESP controler preloaded with [this](https://github.com/janenas-luk/esp_control_over_serial) firmware. Serial communication is handled with `libserialport`. The program itself is a ubus module called `esp_control`.

## Requirements:

* Any router with OpenWrt or (RutOS).
* OpenWRT (RutOS) build system.
* ESP8266 device preloaded with [firmaware](https://github.com/janenas-luk/esp_control_over_serial)

## OpenWRT (RutOS) build system:

* Download and update sources:
```bash
git clone https://git.openwrt.org/openwrt/openwrt.git
cd openwrt
git pull
```
* Update the feeds, select firmware and build your system:
```bash
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig
make
```

## Install:

* Specify the package in `make menuconfig`:
```bash
cd openwrt
make menuconfig
```
![menuconfig](https://github.com/OvidijusStankaitis/Tuya-IoT-Cloud-Daemon-RutOS-Ready/assets/82393494/3aeb84e1-2ddf-457d-b90f-880a8350889e)
* You need to find your package and while it is selected you press Y and then save. The package will be specified.



* Then you compile the packages:
```bash
make package/<package_name>/compile
```
* If you encounter errors - compile with V=s flag for extra verbosity:
```bash
make package/<package_name>/compile V=s
```
* The resulting ipk will be in the `bin/targets/[target]/[subtarget]/packages` directory.
* To install the file onto the router you will need to copy the package and install it.
* Connect your router to your computer via ethernet cable and run:
```bash
scp <package_name_post_compilation>.ipk root@192.168.0.1:/tmp
```
Or (Depending on what is your router's default IP address):
```bash
scp <package_name_post_compilation>.ipk root@192.168.1.1:/tmp
```
If your router uses a different IP address - use that.

* Installing the package on your router:
```bash
ssh root@192.168.0.1
```
Or (Depending on what is your router's default IP address):
```bash
ssh root@192.168.1.1
```
If your router uses a different IP address - use that.

* After connecting to the router:
```bash
cd /tmp
opkg install <package_name_post_compilation>.ipk
```
To force dependencies or reinstall you can add these flags `--force-depends --force-reinstall` after `opkg install`. Something like:
```bash
opkg install --force-depends --force-reinstall <package_name_post_compilation>.ipk
```

## Usage:

* Start the service:
```bash
/etc/init.d/ubus_esp_controler start
```
* List devices:
```bash
ubus call esp_device devices
```
* Turn a pin on:
```bash
ubus call esp_device on '{"port":"/dev/ttyUSBx", "pin":y}'
```
* Turn a pin off:
```bash
ubus call esp_device off '{"port":"/dev/ttyUSBx", "pin":y}'
```