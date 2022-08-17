#!/bin/bash

ARCH="x86_64"
PLATFORM="w64"
TOOLS="mingw32"
TARGET="${ARCH}-${PLATFORM}-${TOOLS}"
echo "TARGET = ${TARGET}"

SYSROOT="${TOOLSDIR}/sys-root/mingw"
echo SYSROOT = ${SYSROOT}

TOOLSDIR="/usr/${TARGET}"
PATH="${TOOLSDIR}/bin:/usr/local/bin:/usr/bin"
echo PATH = ${PATH}

echo "tools"
CC=${TARGET}-gcc
CXX=${TARGET}-g++
PKG_CONFIG=${TARGET}-pkg-config
which ${CC}
which ${CXX}
which pkg-config

echo "cleaning directories \"./build\" and \"${HOME}/.${TARGET}\""
rm -rf build/*
rm -rf ${HOME}/.${TARGET}/*

mkdir build
cd build

# build libusb
#echo // building libusb ===========================================================//
#git clone https://github.com/libusb/libusb
#cd libusb
#./bootstrap.sh
#./configure \
#	CC=${CC} \
#	CFLAGS=-Wno-array-bounds \
#	CXX=${CXX} \
#	PKG_CONFIG=${PKGCONFIG} \
#	PKG_CONFIG_LIBDIR="${SYSROOT}/lib/pkgconfig:${SYSROOT}/share/pkgconfig" \
#	--host=${TARGET} \
#	--with-sysroot=${SYSROOT} \
#	--prefix=${HOME}/.${TARGET}
#make
#make install
#cd ..
#echo

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
	--prefix=${HOME}/.${TARGET}
cd src
make
make install
cd ..
cp libconfuse.pc ${HOME}/.${TARGET}/lib/pkgconfig/libconfuse.pc
cd ..
echo

# build libftdi
echo // building libftdi ==========================================================//
git clone https://github.com/lipro/libftdi
cd libftdi
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=${HOME}/kcube/${TARGET}_toolchain.cmake \
	-DPKG_CONFIG_EXECUTABLE=${HOME}/kcube/${TARGET}-pkg-config \
	-DCMAKE_INSTALL_PREFIX="${HOME}/.${TARGET}/" \
    -Wno-dev \
    ..
make
make install
cd ..
cd ..
echo

echo "[DONE]"
