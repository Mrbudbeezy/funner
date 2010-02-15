###################################################################################################
#������ ��� iPhone Device 2.2 g++
###################################################################################################

IPHONEOS_DEPLOYMENT_TARGET := 2.2.1
MACOSX_DEPLOYMENT_TARGET   := 10.5

export IPHONEOS_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET

COMMON_CFLAGS     += -arch armv6 -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) #-gdwarf-2 -fobjc-call-cxx-cdtors  #���� fobjc-call-cxx-cdtors ��������� ��� ������������� �++ ������� � objective-c ������� � gcc ������ �� 4,2; gdwarf-2 - ��� ��������������
COMMON_LINK_FLAGS += -arch armv6 -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)

COMPILER_GCC := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.0
LINKER_GCC   := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++-4.0

IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.0.sdk

include $(TOOLSETS_DIR)/iphone.mak
