#!/bin/bash

# Libunicows for Windows 9x compatibility.
cd libunicows/src/
make -f makefile.crossmingw32

# FLTK 1.3.x
mkdir -p fltk-install
FLTKINSTALLFOLDER=`readlink -f ./fltk-install`
cd fltk
mkdir -p build
cd build
# This assumes you have FLTK 1.3 installed. If not compile FLTK for the host and specify FLUID_PATH.
# The flto-partition option may not be needed in future versions of GCC, however with GCC 10.2 and Binutils 2.34 I sometimes receive errors while building if I don't disable LTO partitioning.
LTO_FLAGS="-Os -flto -fuse-linker-plugin -flto-partition=none"
CFLAGS="-DNDEBUG"
cmake -DOPTION_USE_GL=OFF \
	-DOPTION_USE_SYSTEM_LIBJPEG=OFF \
	-DOPTION_USE_SYSTEM_LIBPNG=OFF \
	-DOPTION_USE_SYSTEM_ZLIB=OFF \
	-DOPTION_LARGE_FILE=OFF \
	-DCMAKE_SYSTEM_NAME=Windows \
	-DCMAKE_C_COMPILER=/usr/bin/i586-w64-mingw32-gcc \
	-DCMAKE_CXX_COMPILER=/usr/bin/i586-w64-mingw32-g++ \
	-DCMAKE_VERBOSE_MAKEFILE=ON \
	-DCMAKE_AR=/usr/bin/i586-w64-mingw32-gcc-ar \
	-DCMAKE_RANLIB=/usr/bin/i586-w64-mingw32-gcc-ranlib \
	-DCMAKE_NM=/usr/bin/i586-w64-mingw32-gcc-nm \
	-DOPTION_BUILD_EXAMPLES=OFF \
	-DCMAKE_BUILD_TYPE=MinSizeRel \
	-DOPTION_BUILD_SHARED_LIBS=ON \
	-DCMAKE_C_FLAGS="$CFLAGS $LTO_FLAGS" \
	-DCMAKE_CXX_FLAGS="$CFLAGS $LTO_FLAGS" \
	-DCMAKE_CXX_FLAGS="$CFLAGS $LTO_FLAGS" \
	-DCMAKE_EXE_LINKER_FLAGS="$LTO_FLAGS" \
	-DCMAKE_EXE_LINKER_FLAGS="$LTO_FLAGS" \
	-DCMAKE_MODULE_LINKER_FLAGS="$LTO_FLAGS" \
	-DCMAKE_INSTALL_PREFIX="$FLTKINSTALLFOLDER" \
	..
make -j16
make install

cd ../../
make -j16
