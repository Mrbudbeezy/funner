###################################################################################################
#������ ��� iPhone Device 3.2 g++ armv7 (iPhone 3GS/4/4S, iPod Touch 3G/4G, iPad 1/2)
###################################################################################################

COMMON_CFLAGS     += -DARM7 -arch armv7 -mfpu=neon -mthumb
COMMON_LINK_FLAGS += -arch armv7

include $(TOOLSETS_DIR)/iphone-device.mak
