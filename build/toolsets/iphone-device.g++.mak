###################################################################################################
#������ ��� iPhone Device 2.2 g++
###################################################################################################

IPHONEOS_DEPLOYMENT_TARGET := 2.2
MACOSX_DEPLOYMENT_TARGET   := 10.5

export IPHONEOS_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET

COMMON_CFLAGS     += -arch armv6 -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) #-gdwarf-2 -fobjc-call-cxx-cdtors  #���� fobjc-call-cxx-cdtors ��������� ��� ������������� �++ ������� � objective-c ������� � gcc ������ �� 4,2; gdwarf-2 - ��� ��������������
COMMON_LINK_FLAGS += -arch armv6 -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)

COMPILER_GCC := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++-4.2

IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS$(IPHONEOS_DEPLOYMENT_TARGET).sdk

include $(TOOLSETS_DIR)/iphone.g++.mak
