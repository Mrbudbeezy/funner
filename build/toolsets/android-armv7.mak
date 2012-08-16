###################################################################################################
#���������
###################################################################################################
PROFILES                  += arm
ANDROID_TOOLCHAIN         := arm-linux-androideabi
ANDROID_TOOLS_PREFIX      := arm-linux-androideabi
ANDROID_TOOLCHAIN_VERSION := 4.4.3
ANDROID_ARCH              := arm
ANDROID_ABI               := armeabi
BUSYBOX_FILE              := $(BUILD_DIR)platforms/android/arm/busybox
COMMON_CFLAGS             += -march=armv7-a -mtune=xscale -mfpu=vfpv3 #-mfloat-abi=hard
COMMON_CFLAGS             += -D__ARM_ARCH_7__ -D__ARM_ARCH_7A__
COMMON_CFLAGS             += -DARM

include $(TOOLSETS_DIR)/common/android.mak
