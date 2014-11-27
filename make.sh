#!/bin/sh
#
# libaitk: script to build all distributables.
#
# Warning: ANDROID_SDK/ANDROID_NDK environment variables must be set properly
#

# Android SDK configuration
#
ANDROID_SDK_LEVEL=19 
ANDROID_JAR=$ANDROID_SDK/platforms/android-$ANDROID_SDK_LEVEL/android.jar
#
ANDROID_BUILDTOOLS_LEVEL=19.1.0
ANDROID_DX=$ANDROID_SDK/build-tools/$ANDROID_BUILDTOOLS_LEVEL/dx


################### SHOULD NOT EDIT BELLOW ###################

# Directories setup
#
PROJECT_DIR=`pwd`
TMP_DIR=$PROJECT_DIR/tmp
rm -rf $TMP_DIR
mkdir -p $TMP_DIR
DIST_DIR=$PROJECT_DIR/dist
rm -rf $DIST_DIR
mkdir -p $DIST_DIR


################### libaitk.jar ###################
LIBAITK_JAR="${DIST_DIR}/libaitk.jar"

# native libraries
#
AITK_JNI_DIR="${PROJECT_DIR}/jni"
cd $AITK_JNI_DIR
ndk-build
cd $PROJECT_DIR
cp -R libs $DIST_DIR/lib

# Java API
#
AITK_API_SRC_PATH="${PROJECT_DIR}/src/main"
LIBSUSRV_JAR="${PROJECT_DIR}/jni/libs/libsusrv.jar"
CC_FLAGS="-classpath ${ANDROID_JAR}:${LIBSUSRV_JAR}"
cd $AITK_API_SRC_PATH
find . -name "*.java" -print | xargs javac $CC_FLAGS -d $TMP_DIR
cd $PROJECT_DIR

# libaitk.jar
#
cd $TMP_DIR
zip -r ${LIBAITK_JAR} .
cd $PROJECT_DIR
echo "Aitk ARM-eabi library archive: ${LIBAITK_JAR}" 


################### aitkclasses.dex ###################
LIBAITK_DEX="${DIST_DIR}/libaitk_classes.dex"

# Java classes
#
AITK_DEX_SRC_PATH="${PROJECT_DIR}/src/dex"
LIBINTERNAL_JAR="${PROJECT_DIR}/jni/libs/telephony-common.d2j.jar:${PROJECT_DIR}/jni/libs/framework2.d2j.jar"
CC_FLAGS="-classpath ${ANDROID_JAR}:${LIBINTERNAL_JAR}"

rm -rf $TMP_DIR/*
cd $AITK_DEX_SRC_PATH
find . -name "*.java" -print | xargs javac $CC_FLAGS -d $TMP_DIR
cd $PROJECT_DIR

# Dex file
$ANDROID_DX --dex --keep-classes --output $LIBAITK_DEX $TMP_DIR
echo "Aitk DEX library archive: ${LIBAITK_DEX}" 

################### clean tmp objects ###################
rm -rf $TMP_DIR
rm -rf $PROJECT_DIR/libs
rm -rf $PROJECT_DIR/obj

