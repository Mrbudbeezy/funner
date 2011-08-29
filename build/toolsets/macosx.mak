###################################################################################################
#������ ��� MacOS X g++
###################################################################################################

ifndef MACOSX_VERSION
  $(error "Environment variable 'MACOSX_VERSION' not found (use macosx-version.g++ toolset)")
endif

###################################################################################################
#���������
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += macosx unistd carbon cocoa cocoa_desktop has_windows haswchar pthread_static_library x86 g++x86 has_iconv clang
DLL_PATH := DYLD_LIBRARY_PATH

MACOSX_DEPLOYMENT_TARGET := $(MACOSX_VERSION)

export MACOSX_DEPLOYMENT_TARGET

MACOSX_SDK_PATH := /Developer/SDKs/MacOSX10.6.sdk

COMMON_CFLAGS     += -msse3 -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=$(MACOSX_VERSION) -arch i386
COMMON_LINK_FLAGS += -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=$(MACOSX_VERSION) -arch i386
COMPILER_GCC      := /usr/bin/clang
LINKER_GCC        := /usr/bin/clang++

include $(TOOLSETS_DIR)/g++.mak

SOURCE_FILES_SUFFIXES += mm         #���������� �������� ������

###################################################################################################
#�������� shared-library (��� ��������� �����)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name @loader_path/$(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(LIB_PREFIX)$(notdir $1))
endef

define tools.c++compile
$(call tools.g++.c++compile,$1,$2,$3,$4,$5,$(subst --no-warn,-w,$6),$7,$8,$9)
endef
