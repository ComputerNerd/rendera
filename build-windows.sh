#!/bin/bash

# Libunicows for Windows 9x compatibility.
# If Windows 9x compatibility is not needed then this can be skipped.
cd libunicows/src/
make -f makefile.crossmingw32
cd ../../

UNICOWSLIBFOLDER=`readlink -f ./libunicows/lib/mingw32`

# FLTK 1.3.x
#rm -rf fltk-install
mkdir -p fltk-install
FLTKINSTALLFOLDER=`readlink -f ./fltk-install`
cd fltk
#rm -rf build
mkdir -p build
cd build
# This assumes you have FLTK 1.3 installed. If not compile FLTK for the host and specify FLUID_PATH.
# The flto-partition option may not be needed in future versions of GCC, however with GCC 10.2 and Binutils 2.34 I sometimes receive errors while building if I don't disable LTO partitioning.
LTO_FLAGS="-Os -flto -fuse-linker-plugin -flto-partition=none -Wl,--gc-sections"
CFLAGS="-DNDEBUG"
UNICOWSLINK="-L$UNICOWSLIBFOLDER"

LINKER_FLAGS="$LTO_FLAGS $UNICOWSLINK"
STANDARD_LIBS=" -lunicows -lkernel32 -luser32 -lcomctl32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"

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
	-DCMAKE_EXE_LINKER_FLAGS="$LINKER_FLAGS" \
	-DCMAKE_MODULE_LINKER_FLAGS="$LINKER_FLAGS" \
	-DCMAKE_SHARED_LINKER_FLAGS="$LINKER_FLAGS" \
	-DCMAKE_INSTALL_PREFIX="$FLTKINSTALLFOLDER" \
	-DCMAKE_C_STANDARD_LIBRARIES="$STANDARD_LIBS" \
	-DCMAKE_CXX_STANDARD_LIBRARIES="$STANDARD_LIBS" \
	-DCMAKE_FIND_ROOT_PATH=/usr/i586-w64-mingw32 \
	-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=BOTH \
	-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
	-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
	-DOPTION_PRINT_SUPPORT=OFF \
	-DOPTION_USE_SVG=OFF \
	-DOPTION_FILESYSTEM_SUPPORT=OFF \
	-DFLTK_BUILD_TEST=OFF \
	..
make -j16
make install


cd ../../
make -j16

cp /usr/i586-w64-mingw32/bin/libgcc_s_dw2-1.dll .
cp /usr/i586-w64-mingw32/bin/libstdc++-6.dll .
cp fltk-install/bin/libfltk_png.dll .
cp fltk-install/bin/libfltk_jpeg.dll .
cp fltk-install/bin/libfltk_z.dll .
cp fltk-install/bin/libfltk.dll .
i586-w64-mingw32-strip *.exe *.dll
rm Rendera.7z
7z a -t7z -m0=lzma -mx=9 -mlc=7 -mmc=1000000000 -mfb=273 -ms=on Rendera.7z libfltk_jpeg.dll libfltk_png.dll libfltk.dll libfltk_z.dll libgcc_s_dw2-1.dll libstdc++-6.dll rendera.exe
mkisofs -o Rendera.iso ./Rendera.7z
