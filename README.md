# kcube
Libkcube is an open-source and cross-platform alternative to the Thorlabs Kinesis Software library. It is written in C, has a Python interface, and can target Linux, Cygwin64, and Windows. The code is organized into a message encode/decode library, and a multithreaded server that manages comminucation between the host and devices. The server currently supports only one controlling thread. Currently supports the KPZ101 and KSG101. Depends on libusb and libftdi.

Development of this software was funded by Neurophos Inc.

Author: K. Parker Trofatter

## todo
- functionalize init, zeroing, printing status (C or Python?)
- calibration script
- consolidate makefiles and build scripts
- set udev rules so sudo isn't needed to access usb in linux
- mutex triple buffer status updates
- prevent hangs on open (perhaps due to duplicate process owning resources)

---

## linux install
### clone repo
1. `$ clone https://github.com/kptrofatter/kcube`
2. `$ cd kcube`

### build and run test executable
1. `$ ./make_test`
2. `$ sudo ./kcube_test`

### build and use shared library
1. `$ ./make_so`
2. `$ sudo python3 ./kcube_test.py`

---

## windows install
### install cygwin64
1. download [cygwin64](https://www.cygwin.com/setup-x86_64.exe)
2. run installer
3. use defaults
4. Choose a Download Site: pick a nearby university for speed
5. Select Packages: set View to "Category", then set "All/Devel" to "Install"
6. finish install

### usb driver
1. connect Kinesis devices to cpu with USB cables
2. power on devices
3. download [zadig](https://github.com/pbatard/libwdi/releases/download/v1.4.1/zadig-2.7.exe)
4. run installer
5. select from the menubar "Options/List All Devices"
6. select from the dropdown menu any Kinesis device (e.g. "Kinesis Piezo Controller")
7. select driver "libusbK (v3.1.0.0)"
8. press "Replace Driver"

### clone repo
1. open a cygwin64 terminal
2. `$ git clone https://github.com/kptrofatter/kcube`
3. `$ cd kcube`

### build dependencies
1. `$ bash ./kcube_build.sh

### build and run cygwin executable
1. `$ bash ./make_cygexe`
2. `$ ./kcube_test`

### build and use cygkcube.dll
1. `$ bash ./make_cyg`
2. `$ python3 kcube_test.py`

### build and run executable
1. `$ bash ./make_exe`
2. close cygwin64 terminal
3. run kcube_test.exe in Powershell or from file browser

### build and use libkcube.dll
1. `$ bash ./make_dll`
2. close cygwin64 terminal
3. run Python (e.g. in Powershell, VS Code, IPython, Jupyter, Spyder, etc.)

## version history
- 2022-07-xx alpha
