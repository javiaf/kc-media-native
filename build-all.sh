#!/bin/sh

export EXTERNAL=$PWD/external
TARGET_DIR=$PWD/target
export FFMPEG_INSTALL=$TARGET_DIR/ffmpeg_install
export AMR_INSTALL=$TARGET_DIR/opencore-amr_install



iphone_version=4.3
gccvers=4.2


function space {
  for i in `seq 1 3`; do echo ""; done
}


### opencore-amr ###############################################################
echo "### build opencore-amr ##################################################"
rm -fr $AMR_INSTALL
mkdir -p $AMR_INSTALL

if [ 0 -eq 0 ]; then
##################################################################
arch=armv6
echo "+++ build opencore-amr for $arch +++"
ARCHS="$arch "
platform=iPhoneOS

PLATFORM_DIR="/Developer/Platforms/${platform}.platform/Developer"
export PLATFORM_BIN_DIR="${PLATFORM_DIR}/usr/bin"
export PLATFORM_SDK_DIR="${PLATFORM_DIR}/SDKs/${platform}${iphone_version}.sdk"

export MY_CFLAGS="-arch $arch --sysroot=$PLATFORM_SDK_DIR "
export DIST=$arch

export MY_LD="ld"
export MY_CC="gcc"
export MY_CXX="g++"

host="arm-apple-darwin"
target="darwin"
export OPTS="--host=$host --target=$target"

./build-opencore-amr.sh
echo "+++ build opencore-amr for $arch OK +++"
##################################################################
fi
space
if [ 0 -eq 0 ]; then
##################################################################
arch=armv7
echo "+++ build opencore-amr for $arch +++"
ARCHS="$ARCHS $arch "
platform=iPhoneOS

PLATFORM_DIR="/Developer/Platforms/${platform}.platform/Developer"
export PLATFORM_BIN_DIR="${PLATFORM_DIR}/usr/bin"
export PLATFORM_SDK_DIR="${PLATFORM_DIR}/SDKs/${platform}${iphone_version}.sdk"

export MY_CFLAGS="-arch $arch --sysroot=$PLATFORM_SDK_DIR "
export DIST=$arch

export MY_LD="ld"
export MY_CC="gcc"
export MY_CXX="g++"

host="arm-apple-darwin"
target="darwin"
export OPTS="--host=$host --target=$target"

./build-opencore-amr.sh
echo "+++ build opencore-amr for $arch OK +++"
##################################################################
fi
space
if [ 0 -eq 0 ]; then
##################################################################
arch=i386
echo "+++ build opencore-amr for $arch +++"
ARCHS="$ARCHS $arch "
platform=iPhoneSimulator

PLATFORM_DIR="/Developer/Platforms/${platform}.platform/Developer"
export PLATFORM_BIN_DIR="${PLATFORM_DIR}/usr/bin"
export PLATFORM_SDK_DIR="${PLATFORM_DIR}/SDKs/${platform}${iphone_version}.sdk"

export MY_CFLAGS="-arch $arch --sysroot=$PLATFORM_SDK_DIR "
export DIST=$arch

export MY_LD="ld"
export MY_CC="gcc"
export MY_CXX="g++"

host="i686-apple-darwin"
target="darwin"
export OPTS="--host=$host --target=$target"

./build-opencore-amr.sh
echo "+++ build opencore-amr for $arch OK +++"
##################################################################
fi

echo "+++ combine opencore-amr libs +++"
./combine-opencore-amr-libs.sh
echo "+++ combine opencore-amr libs OK +++"

export AMR_C_EXTRA="-I$AMR_INSTALL/include "
export AMR_LD_EXTRA="-L$AMR_INSTALL/lib "
export AMR_L="-lopencore-amrnb"
export AMR_CONFIGURE_OPTS="--enable-version3 \
	--enable-libopencore-amrnb --enable-encoder=libopencore_amrnb";

echo "### build opencore-amr OK ###############################################"
### opencore-amr ###############################################################



space



### ffmpeg #####################################################################
rm -fr $FFMPEG_INSTALL
mkdir -p $FFMPEG_INSTALL

GAS_PREPROCESSOR_DIR=$EXTERNAL/gas-preprocessor
PATH=$PATH:$GAS_PREPROCESSOR_DIR
export CPP="gas-preprocessor.pl"

if [ 0 -eq 0 ]; then
##################################################################
echo Build armv6
ARCHS="armv6 "
armarch=armv6
arch=arm
cpu=arm1176jzf-s
export OPTS="--enable-cross-compile --target-os=darwin --arch=$arch --cpu=$cpu "

platform=iPhoneOS

PLATFORM_DIR="/Developer/Platforms/${platform}.platform/Developer"
export PLATFORM_BIN_DIR="${PLATFORM_DIR}/usr/bin"
export PLATFORM_SDK_DIR="${PLATFORM_DIR}/SDKs/${platform}${iphone_version}.sdk"

export CC=gcc-4.2
export MY_CFLAGS="-arch ${armarch} "
export MY_LDFLAGS="-arch ${armarch} \
                -L${PLATFORM_SDK_DIR}/usr/lib/system "
export DIST=armv6

./build-ffmpeg.sh
##################################################################
fi
if [ 0 -eq 0 ]; then
##################################################################
echo build armv7
ARCHS="$ARCHS armv7 "
armarch=armv7
arch=arm
cpu=cortex-a8
export OPTS="--enable-cross-compile --target-os=darwin --arch=$arch --cpu=$cpu "

platform=iPhoneOS

PLATFORM_DIR="/Developer/Platforms/${platform}.platform/Developer"
export PLATFORM_BIN_DIR="${PLATFORM_DIR}/usr/bin"
export PLATFORM_SDK_DIR="${PLATFORM_DIR}/SDKs/${platform}${iphone_version}.sdk"

export CC=gcc-4.2
export MY_CFLAGS="-arch ${armarch} "
export MY_LDFLAGS="-arch ${armarch} \
                -L${PLATFORM_SDK_DIR}/usr/lib/system "
export DIST=armv7


./build-ffmpeg.sh
##################################################################
fi
if [ 0 -eq 0 ]; then
##################################################################
echo build i386
ARCHS="$ARCHS i386 "
export OPTS="--disable-yasm "

platform=iPhoneSimulator

PLATFORM_DIR="/Developer/Platforms/${platform}.platform/Developer"
export PLATFORM_BIN_DIR="${PLATFORM_DIR}/usr/bin"
export PLATFORM_SDK_DIR="${PLATFORM_DIR}/SDKs/${platform}${iphone_version}.sdk"

export CC=i686-apple-darwin10-gcc-4.2.1
export MY_CFLAGS="-mdynamic-no-pic "
export MY_LDFLAGS="-L${PLATFORM_SDK_DIR}/usr/lib/system "
export DIST=i386

./build-ffmpeg.sh
##################################################################
fi
### ffmpeg #####################################################################
