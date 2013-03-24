###################################################################################################
#������ ��� WindowsRT
###################################################################################################

###################################################################################################
#����� ������������ MSVC
###################################################################################################
ifneq (,$(VS110COMNTOOLS))
  MSVC_PATH         ?= $(VS110COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS110COMNTOOLS)../../Common7/Ide
  PROFILES          += haswchar
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

ifeq (,$(WINRT_SDK))
  $(error 'Microsoft WindowsRT SDK not detected (empty WINRT_SDK)')
endif

ifeq (,$(TARGET_PLATFORM))
  $(error 'TARGET_PLATFORM has not been defined')
endif

ifneq (,$(WINRT_SDK))
  SDK_DIR ?= $(call convert_path,$(WINRT_SDK))
endif

MSVC_BIN_PATH      := $(MSVC_PATH)/bin$(if $(filter x86,$(CPU_ARCH)),)
COMMON_CFLAGS      += -W3 -Ox -wd4996 -nologo -FC -D "WINRT" -D "WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP" -MD -AI "$(WINRT_SDK)\References\CommonConfiguration\Neutral"
COMMON_CFLAGS      += -AI "$(MSVC_PATH)\vcpackages"
COMMON_LINK_FLAGS  += -DYNAMICBASE

###################################################################################################
#���������
###################################################################################################
LIB_SUFFIX     := .lib
OBJ_SUFFIX     := .obj
EXE_SUFFIX     := .exe
DLL_SUFFIX     := .dll
DLL_LIB_SUFFIX := .lib
DLL_PREFIX     :=
PROFILES       += msvc has_windows win8 winrt
FRAMEWORK_DIR  := ${SYSTEMROOT}/Microsoft.NET/Framework/v4.0.30319
SOURCE_FILES_SUFFIXES += asm
VALID_TARGET_TYPES += win8-msbuild-appx

###################################################################################################
#������������ ���������� ������������ ���������
###################################################################################################
INCLUDE := $(WINRT_SDK)/include;$(WINRT_SDK)/include/shared;$(WINRT_SDK)/include/um;$(WINRT_SDK)/include/winrt;$(MSVC_PATH)/include;$(INCLUDE)
LIB     := $(WINRT_SDK)/lib/win8/um/$(CPU_ARCH);$(MSVC_PATH)/lib$(if $(filter x86,$(CPU_ARCH)),);$(LIB)

export INCLUDE
export LIB

###################################################################################################
#���������� ���������� (������ ����������, ������ ������������ ���������, ������ ������������ ������, ������� � ���������� �������,
#������ ��������, ����� ����������, pch ����, ������ ��������� � dll)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
$(if $(filter %.c,$1)$(filter %.cpp,$1),&& "$(MSVC_BIN_PATH)/cl" -c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(COMMON_CFLAGS) $6 $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)),-EHsc) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")) \
$(if $(filter %.asm,$1),&& "$(MSVC_BIN_PATH)/ml" -nologo -c -Fo"$4\\" $(patsubst %,-I"%",$2) $6 $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.asm,$1))
endef

###################################################################################################
#�������� ������ (��� ��������� �����, ������ ������, ������ ��������� �� ������������ ������������,
#������ ������������ �������� ��������, ����� ��������, def ����)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"_%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS) $(if $6,-DEF:"$6")
endef

###################################################################################################
#������ ���������� (��� ��������� �����, ������ ������)
###################################################################################################
define tools.lib.generic
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && export FILE_COUNTER=0 FILE_LIST="" && for file in $2; do export FILE_COUNTER=$$(($$FILE_COUNTER + 1)) && FILE_LIST="$$FILE_LIST $$file"; if [ $$FILE_COUNTER -eq 256 ]; then $(call tools.lib.generic,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9); export FILE_COUNTER=0 FILE_LIST="$1"; fi; done && $(call tools.lib.generic,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9)
endef

#��������� ���� win8-msbuild-appx (��� ����)
define process_target.win8-msbuild-appx
  $1.SOLUTION     := $(COMPONENT_DIR)$$($1.SOLUTION)
  $1.PACKAGE_NAME := $(DIST_LIB_DIR)/$$($1.NAME).appx
  $1.SOURCE_DIR   := $$(dir $$($1.SOLUTION))
  $1.DEPS         := $$(foreach ext,$$(SOURCE_FILES_SUFFIXES),$$(wildcard $$($1.SOURCE_DIR)/*.$$(ext)) )
  $1.TMP_DIR      := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1

  build: BUILD.$1

  .PHONY: BUILD.$1

  BUILD.$1: $$($1.PACKAGE_NAME)

  $$($1.PACKAGE_NAME): $$($1.SOLUTION) $$($1.DEPS)
	@echo Building $$(notdir $$($1.SOLUTION))...
	@test -d "$$($1.TMP_DIR)" || mkdir -p "$$($1.TMP_DIR)"
	@chcp.com 850 > $$($1.TMP_DIR)/chcp.stdout && $(call convert_path,$(FRAMEWORK_DIR))/msbuild.exe $$($1.SOLUTION) -p:VisualStudioVersion=11.0 -property:Configuration="Release" -property:Platform="$(TARGET_PLATFORM)" > $$($1.TMP_DIR).msbuild-out
endef
