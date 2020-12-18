#!/bin/bash

# FLTK 1.1.x
rm -rf fltk-install
mkdir -p fltk-install
FLTKINSTALLFOLDER=`readlink -f ./fltk-install`
cd fltk
rm -rf build
mkdir -p build
cd build
# This assumes you have FLTK 1.3 installed. If not compile FLTK for the host and specify FLUID_PATH.
# The flto-partition option may not be needed in future versions of GCC, however with GCC 10.2 and Binutils 2.34 I sometimes receive errors while building if I don't disable LTO partitioning.
LTO_FLAGS="-fPIE -flto -fuse-linker-plugin -flto-partition=none -fno-rtti -Os -Wl,--gc-sections"
CFLAGS="-DNDEBUG -DPNG_NO_CONSOLE_IO -DNO_TRACK_MOUSE"

LINKER_FLAGS="$LTO_FLAGS"
STANDARD_LIBS=" -lkernel32 -luser32 -lcomctl32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"

cmake -DOPTION_USE_GL=OFF \
	-DOPTION_USE_SYSTEM_LIBJPEG=OFF \
	-DOPTION_USE_SYSTEM_LIBPNG=OFF \
	-DOPTION_USE_SYSTEM_ZLIB=OFF \
	-DOPTION_LARGE_FILE=OFF \
	-DCMAKE_SYSTEM_NAME=Windows \
	-DCMAKE_C_COMPILER=/usr/bin/i386-mingw32crt-gcc \
	-DCMAKE_CXX_COMPILER=/usr/bin/i386-mingw32crt-g++ \
	-DCMAKE_VERBOSE_MAKEFILE=ON \
	-DCMAKE_AR=/usr/bin/i386-mingw32crt-gcc-ar \
	-DCMAKE_RANLIB=/usr/bin/i386-mingw32crt-gcc-ranlib \
	-DCMAKE_NM=/usr/bin/i386-mingw32crt-gcc-nm \
	-DCMAKE_BUILD_TYPE=MinSizeRel \
	-DCMAKE_C_FLAGS="$CFLAGS $LTO_FLAGS" \
	-DCMAKE_CXX_FLAGS="$CFLAGS $LTO_FLAGS" \
	-DCMAKE_CXX_FLAGS="$CFLAGS $LTO_FLAGS" \
	-DCMAKE_EXE_LINKER_FLAGS="$LINKER_FLAGS" \
	-DCMAKE_MODULE_LINKER_FLAGS="$LINKER_FLAGS" \
	-DCMAKE_SHARED_LINKER_FLAGS="$LINKER_FLAGS" \
	-DCMAKE_INSTALL_PREFIX="$FLTKINSTALLFOLDER" \
	-DCMAKE_C_STANDARD_LIBRARIES="$STANDARD_LIBS" \
	-DCMAKE_CXX_STANDARD_LIBRARIES="$STANDARD_LIBS" \
	-DCMAKE_FIND_ROOT_PATH=/usr/i386-mingw32crt \
	-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=BOTH \
	-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
	-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
	-DUSE_OPENGL=OFF \
	-DBUILD_TESTING=OFF \
	-DBUILD_EXAMPLES=OFF \
	-DBUILD_SHARED_LIBS=OFF \
	-DMINGW=ON \
	..
make -j16
make install


cd ../../
make clean
make -j16

cp /usr/i386-mingw32crt/bin/libgcc_s_dw2-1.dll .
cp /usr/i386-mingw32crt/bin/libstdc++-6.dll .
i386-mingw32crt-strip *.exe *.dll
rm -rf disk1.img rend.zip
advzip -4 -a rend.zip rendera.exe unicows.dll
dd if=/dev/zero of=disk1.img count=1440 bs=1k
mkfs.msdos disk1.img
mcopy -i ./disk1.img rend.zip ::/
