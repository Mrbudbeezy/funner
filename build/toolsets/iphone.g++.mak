###################################################################################################
#������ ��� iPhone
###################################################################################################

ifndef IPHONE_SDK_PATH
  $(error "Environment variable 'IPHONE_SDK_PATH' not found (use iphone-version.g++ toolset)")
endif

###################################################################################################
#���������
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += iphone unistd has_windows haswchar gles
DLL_PATH := DYLD_LIBRARY_PATH

COMMON_CFLAGS     += -Os -isysroot $(IPHONE_SDK_PATH) -DIPHONE
COMMON_LINK_FLAGS += -isysroot $(IPHONE_SDK_PATH) -mmacosx-version-min=10.5

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

###################################################################################################
#������ ������ ����������� ����������
###################################################################################################
VALID_TARGET_TYPES += fat-static-lib

#����� ���������� (��� ����������, ���� ������, ���������� ����������)
define find_library
  LIPO_LIBRARY_SOURCE := $$(strip $$(firstword $$(wildcard $$(patsubst %,%/$$(notdir $$(strip $1)),$2))))
  
  ifeq (,$$(LIPO_LIBRARY_SOURCE))
    $$(error Library '$1' not found)
  endif
  
  $3 := $$($3) $$(LIPO_LIBRARY_SOURCE)
endef

#��������� ���� ������������ ���������(��� ����)
define process_target.fat-static-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty fat static library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif  
  
  $1.LIB_FILE                      := $(DIST_LIB_DIR)/$$($1.NAME)$(LIB_SUFFIX)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.LIB_FILE)
  DIST_DIRS                        := $$(DIST_DIRS) $$(DIST_LIB_DIR)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)
  $1.TMP_DIR                       := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  TMP_DIRS                         := $$(TMP_DIRS) $$($1.TMP_DIR)

  $$(warning tmp=$$($1.TMP_DIR))
  
  $$(eval $$(call process_target_with_sources,$1))
  
  $$(foreach lib,$$($1.LIBS),$$(eval $$(call find_library,$$(lib),$$($1.LIB_DIRS),$1.LIBS_FULL_PATH)))

  $1.LIBS := $$($1.LIBS_FULL_PATH)

  build: $$($1.TMP_DIR) $$($1.LIB_FILE)

  $$($1.LIB_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Create fat static library $$(notdir $$($1.LIB_FILE))..
#		@$(RM) -Rf 
endef

