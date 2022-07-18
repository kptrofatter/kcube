#!/bin/bash

PATH="/usr/local/bin:/usr/bin:/bin"

ARCH=x86_64
PLATFORM=w64
TOOLCHAIN=mingw32
TARGET=${ARCH}-${PLATFORM}-${TOOLCHAIN}
SYSROOT=/usr/${TARGET}/sys-root/mingw

PATH="${PATH}:${SYSROOT}/bin"

CC=${TARGET}-gcc
CXX=${TARGET}-g++
PKG_CONFIG=${TARGET}-pkg-config

rm -rf build
rm -rf /home/${USER}/.${TARGET}/*

mkdir build
cd build

echo // building libconfuse =======================================================//
# build libconfuse
git clone https://github.com/libconfuse/libconfuse
cd libconfuse
./autogen.sh
./configure \
	CC=${CC} \
	PKG_CONFIG=${PKG_CONFIG} \
	PKG_CONFIG_LIBDIR="${SYSROOT}/lib/pkgconfig:${SYSROOT}/share/pkgconfig" \
	--host=${TARGET} \
	--with-sysroot=${SYSROOT} \
	--prefix=/home/${USER}/.${TARGET}
cd src
make
make install
cd ..
cd ..
echo

exit 0

# build libusb
echo // building libusb ===========================================================//
git clone https://github.com/libusb/libusb
cd libusb
./bootstrap.sh
./configure \
	CC=${CC} \
	CFLAGS=-Wno-array-bounds \
	CXX=${CXX} \
	PKG_CONFIG=${PKGCONFIG} \
	PKG_CONFIG_LIBDIR="${SYSROOT}/lib/pkgconfig:${SYSROOT}/share/pkgconfig" \
	--host=${TARGET} \
	--with-sysroot=${SYSROOT} \
	--prefix=/home/${USER}/.${TARGET}
make
make install
cd ..
echo

# build libftdi
echo // building libftdi ==========================================================//
git clone https://github.com/lipro/libftdi
cd libftdi
mkdir build
cd build

/usr/bin/cmake -DCMAKE_TOOLCHAIN_FILE=/home/${USER}/kcube/toolchain_x86_64-w64-mingw32.cmake \
	-DCMAKE_INSTALL_PREFIX="/homes/${USER}/.${PREFIX}/" .. \
	-DPKG_CONFIG_EXECUTABLE=/home/${USER}/kcube/x86_64-w64-mingw32-pkg-config
	
make
make install
cd ..
echo

echo "[DONE]"
