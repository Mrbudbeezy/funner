###################################################################################################
#��������� ������
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := component.mak #������� ��� ����� ������������ ����������
EXPORT_FILE_SHORT_NAME                  := export.mak    #������� ��� ����� ��������
TMP_DIR_SHORT_NAME                      := tmp           #������� ��� �������� � ���������� �������
SOURCE_FILES_SUFFIXES                   := c cpp         #���������� �������� ������
TMP_DIR_SHORT_NAME                      := tmp           #������� ��� �������� � ���������� ������� ������
DIST_DIR_SHORT_NAME                     := dist          #������� ��� �������� � ������������ ������
TOOLSETS_DIR_SHORT_NAME                 := toolsets      #������� ��� �������� � �������������� toolset-��
DOXYGEN_TEMPLATE_DIR_SHORT_NAME         := doxygen       #������� ��� �������� � ��������� doxygen
DOXYGEN_DEFAULT_TOPIC_SHORT_NAME        := default_topic.html #������� ��� �������� �� ��������� doxygen
DOXYGEN_TEMPLATE_CFG_FILE_SHORT_NAME    := template.cfg  #��� ���������� ����� � ������������� doxygen
DOXYGEN_TAGS_DIR_SHORT_NAME             := ~DOXYGEN_TAGS #��� �������� � ������ ������������
PCH_SHORT_NAME                          := pch.h         #������� ��� PCH �����
EXPORT_VAR_PREFIX                       := export        #������� ����� ���������� ��������������� �������� ����������
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := batch-flag    #������� ��� �����-����� �������� ����������
VALID_TARGET_TYPES                      := static-lib dynamic-lib application test-suite package installation doxygen-info #���������� ���� �����
PACKAGE_COMMANDS                        := build clean test check run install #�������, ������������ ����������� ������
COMPILE_TOOL                            := tools.c++compile     #��� ������� ������� ���������� C++ ������
LINK_TOOL                               := tools.link           #��� ������� ������� ��������� ������
LIB_TOOL                                := tools.lib            #��� ������� ������� ������������� ��������� ������
DLL_PATH                                := PATH                 #��� ���������� ����� ��� �������� ����� � ���-������

###################################################################################################
#����������� �������� ������������
###################################################################################################
BUILD_DIR  := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
USER       ?= $(USERNAME)

-include $(ROOT)/$(USER).settings.mak

###################################################################################################
#����������� ���� � ����������
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := $(strip $(COMPONENT_CONFIGURATION_FILE_SHORT_NAME))
EXPORT_FILE_SHORT_NAME                  := $(strip $(EXPORT_FILE_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
SOURCE_FILES_SUFFIXES                   := $(strip $(SOURCE_FILES_SUFFIXES))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
DIST_DIR_SHORT_NAME                     := $(strip $(DIST_DIR_SHORT_NAME))
EXPORT_VAR_PREFIX                       := $(strip $(EXPORT_VAR_PREFIX))
EXPORT_TAR_TMP_FILE_SHORT_NAME          := $(strip $(EXPORT_TAR_TMP_FILE_SHORT_NAME))
EXPORT_EXCLUDE_PATTERN                  := $(strip $(EXPORT_EXCLUDE_PATTERN))
DOXYGEN_TEMPLATE_CFG_FILE_SHORT_NAME    := $(strip $(DOXYGEN_TEMPLATE_CFG_FILE_SHORT_NAME))
DOXYGEN_TEMPLATE_DIR_SHORT_NAME         := $(strip $(DOXYGEN_TEMPLATE_DIR_SHORT_NAME))
DOXYGEN_DEFAULT_TOPIC_SHORT_NAME        := $(strip $(DOXYGEN_DEFAULT_TOPIC_SHORT_NAME))
CURRENT_TOOLSET                         := $(TOOLSET)
TOOLSETS_DIR                            := $(BUILD_DIR)$(strip $(TOOLSETS_DIR_SHORT_NAME))
TOOLSETS                                := $(patsubst $(TOOLSETS_DIR)/%.mak,%,$(wildcard $(TOOLSETS_DIR)/*.mak))
TOOLSET_FILE                            := $(TOOLSETS_DIR)/$(CURRENT_TOOLSET).mak
DIST_DIR                                := $(ROOT)/$(DIST_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)
DIST_LIB_DIR                            := $(DIST_DIR)/lib
DIST_BIN_DIR                            := $(DIST_DIR)/bin
DIST_INFO_DIR                           := $(DIST_DIR)/info
PCH_SHORT_NAME                          := $(strip $(PCH_SHORT_NAME))
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := $(strip $(BATCH_COMPILE_FLAG_FILE_SHORT_NAME))
ROOT_TMP_DIR                            := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)
TMP_DIRS                                := $(ROOT_TMP_DIR)
DIST_DIRS                               := $(DIST_LIB_DIR) $(DIST_BIN_DIR) $(DIST_INFO_DIR)
DIRS                                     = $(TMP_DIRS) $(DIST_DIRS)
COMPILE_TOOL                            := $(strip $(COMPILE_TOOL))
LINK_TOOL                               := $(strip $(LINK_TOOL))
LIB_TOOL                                := $(strip $(LIB_TOOL))
EMPTY                                   :=
SPACE                                   := $(EMPTY) $(EMPTY)
EXTERNAL_FILES                          :=
DLL_PATH                                := $(strip $(DLL_PATH))
DOXYGEN_TEMPLATE_DIR                    := $(BUILD_DIR)$(DOXYGEN_TEMPLATE_DIR_SHORT_NAME)
DOXYGEN_TEMPLATE_CFG_FILE               := $(DOXYGEN_TEMPLATE_DIR)/$(DOXYGEN_TEMPLATE_CFG_FILE_SHORT_NAME)
DOXYGEN_DEFAULT_TOPIC                   := $(DOXYGEN_TEMPLATE_DIR)/$(DOXYGEN_DEFAULT_TOPIC_SHORT_NAME)
DOXYGEN_TOOL                            := $(DOXYGEN_DIR)/doxygen

###################################################################################################
#���� �� ������ ������� - ������������ ��� ���������
###################################################################################################
files ?= %
targets ?= %

###################################################################################################
#���� ������ "�� ���������"
###################################################################################################
default: build

###################################################################################################
#�������������� Windows-�����
###################################################################################################
define convert_path
$(subst ; ,;,$(subst \,/,$1))
endef

###################################################################################################
#������� ������ ������ (��� ���������� � ������ �����, ������ ������, ��������)
###################################################################################################
define for_each_file
$(if $2,for $1 in $2; do $3; if [ "$$?" -ne "0" ]; then exit 1; fi; done,true)
endef

###################################################################################################
#����������� ����� ������������ ������ ������
###################################################################################################
ifeq (,$(filter $(CURRENT_TOOLSET),$(TOOLSETS)))
  $(error Unknown toolset '$(CURRENT_TOOLSET)'. Use one of available toolsets '$(TOOLSETS)')
endif

include $(TOOLSET_FILE)

###################################################################################################
#���������, ��������� �� ������ ������
###################################################################################################
TAGS_DIR := $(strip $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$(TAGS_DIR_SHORT_NAME))
TMP_DIRS += $(TAGS_DIR)

###################################################################################################
#�������� ������� ��������
###################################################################################################
define check_toolset_constant
  $(if $(filter $1, $(.VARIABLES)),,$(warning Build variable '$1' not defined (check file '$(TOOLSET_FILE)')))
endef

$(call check_toolset_constant,LIB_SUFFIX)
$(call check_toolset_constant,OBJ_SUFFIX)
$(call check_toolset_constant,DLL_SUFFIX)
$(call check_toolset_constant,EXE_SUFFIX)

###################################################################################################
#����������� ������ ������������ ���������� � ������ ��������
###################################################################################################
COMPONENT_CONFIGURATION_FILE := $(firstword $(wildcard $(filter %$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME), $(MAKEFILE_LIST:makefile=$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME)))))
EXPORT_FILES                 := $(wildcard $(filter %$(EXPORT_FILE_SHORT_NAME), $(MAKEFILE_LIST:makefile=$(EXPORT_FILE_SHORT_NAME))))
COMPONENT_DIR                := $(dir $(COMPONENT_CONFIGURATION_FILE))

ifeq (,$(COMPONENT_CONFIGURATION_FILE))
  $(error Configuration file '$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME)' not found at project tree (root='$(ROOT)'))
endif

include $(COMPONENT_CONFIGURATION_FILE)

###################################################################################################
#�������� �������������� �������� (���� ��������)
###################################################################################################
define load_exports
#�������� ������� �����
ifeq (,$$(wildcard $1))
  $$(error Export file '$1' not found)
endif

#������� ���������� $(EXPORT_VAR_PREFIX).*
  OLD_VARIABLES  := $$(filter $(EXPORT_VAR_PREFIX).%,$$(.VARIABLES))
  COMPONENT_DIRS :=

#����������� ����� �����������
  include $1

#��������� ��������
  EXPORT_VARIABLES       := $$(filter-out $$(OLD_VARIABLES),$$(filter $(EXPORT_VAR_PREFIX).%,$$(.VARIABLES)))
  EXPORT_COMPONENT_NAMES := $$(patsubst $(EXPORT_VAR_PREFIX).%,%,$$(foreach var,$$(EXPORT_VARIABLES),$$(var:%.$$(word $$(words $$(subst .,$(SPACE),$$(var))),$$(subst .,$(SPACE),$$(var)))=%)))

# ���������� ����� � �����������
  $$(foreach name,$$(EXPORT_COMPONENT_NAMES),$$(eval paths.$$(name) := $$(dir $1)))

  $$(foreach dir,$$(COMPONENT_DIRS),$$(eval $$(call load_exports,$$(dir $1)$$(dir)/$(EXPORT_FILE_SHORT_NAME))))
endef

$(foreach file,$(EXPORT_FILES),$(eval $(call load_exports,$(file))))
#$(foreach var,$(sort $(filter export.%,$(.VARIABLES))),$(warning $(var)=$($(var))))
#$(foreach path,$(sort $(filter paths.%,$(.VARIABLES))),$(warning $(path)=$($(path))))

###################################################################################################
#������������� ����
###################################################################################################
define specialize_paths
$(foreach dir,$(strip $1),$(COMPONENT_DIR)$(dir))
endef

###################################################################################################
#�������������� ����� make � ��������� ����� (��� ����)
###################################################################################################
ifneq (,$(filter Win%,$(OS)))

define get_system_dir
cmd "\\/C" "cd $1 && cd"
endef

else

define get_system_dir
cd "$1" && pwd
endef

endif

###################################################################################################
#���������� � ������� ���������� (������� ������� ����������, ������ ��������� � ������������� ������������)
###################################################################################################
define prepare_to_execute
export PATH="$(subst ;,:,$(call convert_path,$(CURDIR)/$(DIST_BIN_DIR);$(foreach dir,$(ADDITIONAL_PATHS),$(dir);)$$PATH))" \
$(DLL_PATH)="$(subst ;,:,$(call convert_path,$(foreach dir,$2,$(CURDIR)/$(dir);)$$$(DLL_PATH)))" \
BIN_DIR=`$(call get_system_dir,$(DIST_BIN_DIR))`/ && cd "$1"
endef

###################################################################################################
#��������� ����� ����������
###################################################################################################

#�������� ����������� ���������� ����� �� ����-����� �������� ���������� (��� ��������� �����, ��������� �������, ����-����)
define create_object_file_dependency
$2/$$(notdir $$(basename $1))$(OBJ_SUFFIX): $1 $3
	@
endef

#������������ ������� ��������� �������� ������ � �����. �� ��������� (������ �������� ������, ��������� �������)
define test_source_and_object_files
$(foreach src,$1,if [ $(src) -nt $2/$(notdir $(basename $(src)))$(OBJ_SUFFIX) ]; then echo $(src); fi &&) true 
endef

#������� �������� ���������� (��� ����, ��� ������)
define batch-compile
  $2.FLAG_FILE  := $$($2.TMP_DIR)/$$(BATCH_COMPILE_FLAG_FILE_SHORT_NAME)
  $1.FLAG_FILES := $$($1.FLAG_FILES) $$($2.FLAG_FILE)
  
  ifneq (,$$(strip $$($2.PCH)))
  
  $$($2.FLAG_FILE): $$($2.PCH_OUTPUT)
  
  endif  

  ifeq (,$$(wildcard $$($2.FLAG_FILE).incomplete-build))  

    $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
			@echo build-start > $$@.incomplete-build
			$$(call $(COMPILE_TOOL),$$(sort $$(filter-out force,$$?)),$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS),$$($1.INCLUDE_FILES),$$($2.TMP_DIR),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS),$$($2.PCH),$$($1.DLL_DIRS))
			@echo batch-flag-file > $$@
			@$(RM) $$@.incomplete-build

  else  #� ������ ���� ������ ���� �� ���������
  
    $$($2.FLAG_FILE): $2.UPDATED_SOURCE_FILES := $$(shell $$(call test_source_and_object_files,$$($2.SOURCE_FILES),$$($2.TMP_DIR)))
  
    $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
			$$(if $$($2.UPDATED_SOURCE_FILES),$$(call $(COMPILE_TOOL),$$(sort $$($2.UPDATED_SOURCE_FILES)),$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS),$$($1.INCLUDE_FILES),$$($2.TMP_DIR),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS),$$($2.PCH),$$($1.DLL_DIRS)))
			@echo batch-flag-file > $$@
			@$(RM) $$@.incomplete-build

  endif  

  ifneq (,$$($2.NEW_SOURCE_FILES))
    $$($2.FLAG_FILE): force
  endif
  
  $$(foreach source,$$($2.SOURCE_FILES),$$(eval $$(call create_object_file_dependency,$$(source),$$($2.TMP_DIR),$$($2.FLAG_FILE))))
endef

#��������� �������� � ��������� ������� (��� ����, ���� � �������� � ��������� �������, ������ ����������� ��������)
define process_source_dir
  ifeq (,$$(wildcard $2))
    $$(error Source dir '$2' not found at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  MODULE_PATH := $$(subst ./,,$$(subst ../,,$2))
  MODULE_NAME := $1.$$(subst /,-,$$(MODULE_PATH))
  
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $$(MODULE_NAME).SOURCE_FILES := $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $$(MODULE_NAME).SOURCE_FILES := $$(wildcard $$(SOURCE_FILES_SUFFIXES:%=$2/*.%))
  endif  
  
  $$(MODULE_NAME).SOURCE_DIR := $2
  $$(MODULE_NAME).TMP_DIR    := $$($1.TMP_DIR)/$$(MODULE_PATH)
  $1.TMP_DIRS                := $$($$(MODULE_NAME).TMP_DIR) $$($1.TMP_DIRS)

  ifneq (,$$($$(MODULE_NAME).SOURCE_FILES))
    $$(MODULE_NAME).OBJECT_FILES := $$(patsubst %,$$($$(MODULE_NAME).TMP_DIR)/%$(OBJ_SUFFIX),$$(notdir $$(basename $$($$(MODULE_NAME).SOURCE_FILES))))
    $$(MODULE_NAME).PCH          := $$(if $$(wildcard $2/$(PCH_SHORT_NAME)),$(PCH_SHORT_NAME))
    $1.OBJECT_FILES              := $$($1.OBJECT_FILES) $$($$(MODULE_NAME).OBJECT_FILES)    
    
    $$(eval $$(call batch-compile,$1,$$(MODULE_NAME)))
  endif

  $$(foreach macros,$3,$$(eval $$(call $$(macros),$1,$$(MODULE_NAME))))
endef

#�������� ����������� ��� ����������� ������� ������ (��� �������� �����, �������� ������)
define create_extern_file_dependency
  ifeq (,$$(filter $1,$$(EXTERNAL_FILES)))
    DEPENDENCY_SOURCE := $$(firstword $$(wildcard $$(patsubst %,%/$$(notdir $1),$2)))
  
    ifeq (,$$(strip $$(DEPENDENCY_SOURCE)))
      DEPENDENCY_SOURCE := $(DIST_BIN_DIR)/$$(notdir $1)
    endif

    ifneq ($$(strip $1),$$(strip $$(DEPENDENCY_SOURCE)))

      $1: $$(DEPENDENCY_SOURCE)
				@cp -fv $$< $$@
				@chmod ug+rwx $$@
    endif
    
    EXTERNAL_FILES := $$(EXTERNAL_FILES) $1
  endif  
endef

#����� ��� ����� � ��������� ������� (��� ����, ������ �������� ����������� ��� ��������� ��������� � ��������� �������)
define process_target_with_sources
#���������� ��������� �� ���������
  $$(foreach lib,$$($1.EXCLUDE_DEFAULT_LIBS),$$(eval $1.LIBS := $$(filter-out $$(lib),$$($1.LIBS))))

  $1.TMP_DIR            := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.TMP_DIRS           := $$($1.TMP_DIR)
  $1.INCLUDE_DIRS       := $$(call specialize_paths,$$($1.INCLUDE_DIRS))
  $1.SOURCE_DIRS        := $$(call specialize_paths,$$($1.SOURCE_DIRS))
  $1.DOCUMENTATION_DIRS := $$(call specialize_paths,$$($1.DOCUMENTATION_DIRS))
  $1.LIB_DIRS           := $$(call specialize_paths,$$($1.LIB_DIRS)) $(DIST_LIB_DIR)
  $1.DLL_DIRS           := $$(call specialize_paths,$$($1.DLL_DIRS)) $(DIST_BIN_DIR)
  $1.EXECUTION_DIR      := $$(strip $$($1.EXECUTION_DIR))
  $1.EXECUTION_DIR      := $$(if $$($1.EXECUTION_DIR),$(COMPONENT_DIR)$$($1.EXECUTION_DIR))
  $1.LIBS               := $$($1.LIBS:%=$(LIB_PREFIX)%$(LIB_SUFFIX))
  $1.LIB_DEPS           := $$(filter $$(addprefix %/,$$($1.LIBS)),$$(wildcard $$($1.LIB_DIRS:%=%/*)))  

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_source_dir,$1,$$(dir),$2)))

  TMP_DIRS := $$($1.TMP_DIRS) $$(TMP_DIRS)
  TMP_CLEAN_DIRS := $(TMP_CLEAN_DIRS) $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1

  build: $$($1.TARGET_DLLS)  

  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))  
endef

#��������� ���� static-lib (��� ����)
define process_target.static-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty static name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.LIB_FILE                      := $(DIST_LIB_DIR)/$(LIB_PREFIX)$$($1.NAME)$(LIB_SUFFIX)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.LIB_FILE)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)

  build: $$($1.LIB_FILE)

  $$(eval $$(call process_target_with_sources,$1))  

  $$($1.LIB_FILE): $$($1.FLAG_FILES)
		@echo Create library $$(notdir $$@)...
		@$$(call $(LIB_TOOL),$$@,$$($1.OBJECT_FILES))
endef

#��������� ���� dynamic-lib (��� ����)
define process_target.dynamic-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty dynamic library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif
  
  $1.DLL_FILE                      := $(DIST_BIN_DIR)/$$($1.NAME)$(DLL_SUFFIX)
  $1.LIB_FILE                      := $(DIST_LIB_DIR)/$(LIB_PREFIX)$$(notdir $$(basename $$($1.DLL_FILE)))$(DLL_LIB_SUFFIX)
  $1.LIB_TMP_FILE                  := $$(dir $$($1.DLL_FILE))$(LIB_PREFIX)$$(notdir $$(basename $$($1.DLL_FILE)))$(DLL_LIB_SUFFIX)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.DLL_FILE) $$($1.LIB_FILE)
  $1.TARGET_DLLS                   := $$($1.DLLS:%=$(DIST_BIN_DIR)/%$(DLL_SUFFIX))
  DIST_DIRS                        := $$(DIST_DIRS) $$(dir $$($1.DLL_FILE))
  $1.SOURCE_INSTALLATION_DLL_FILES := $$($1.TARGET_DLLS) $$($1.DLL_FILE)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)  

  build: $$($1.DLL_FILE) $$($1.LIB_FILE)

  $$(eval $$(call process_target_with_sources,$1))
  
  $$($1.LIB_FILE): $$($1.DLL_FILE)

  $$($1.DLL_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Create dynamic library $$(notdir $$($1.DLL_FILE))...
		@$$(call $(LINK_TOOL),$$($1.DLL_FILE),$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))
		@$(RM) $$(basename $$($1.DLL_FILE)).exp
		@if [ -e $$($1.LIB_TMP_FILE) ]; then mv -f $$($1.LIB_TMP_FILE) $(DIST_LIB_DIR); fi
endef

#��������� ���� application (��� ����)
define process_target.application
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty application name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.EXE_FILE                      := $(DIST_BIN_DIR)/$$($1.NAME)$$(if $$(suffix $$($1.NAME)),,$(EXE_SUFFIX))
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.EXE_FILE)
  $1.TARGET_DLLS                   := $$($1.DLLS:%=$(DIST_BIN_DIR)/%$(DLL_SUFFIX))
  DIST_DIRS                        := $$(DIST_DIRS) $$(dir $$($1.EXE_FILE))
  $1.SOURCE_INSTALLATION_DLL_FILES := $$($1.TARGET_DLLS)
  $1.SOURCE_INSTALLATION_EXE_FILES := $$($1.EXE_FILE)

  build: $$($1.EXE_FILE)

  $$(eval $$(call process_target_with_sources,$1))
  
  ifeq (,$$($1.EXECUTION_DIR))
    $1.EXECUTION_DIR := $(DIST_BIN_DIR)
  endif
  
  $$($1.EXE_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Linking $$(notdir $$@)...
		@$$(call $(LINK_TOOL),$$@,$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))

  RUN.$1: $$($1.EXE_FILE)
		@echo Running $$(notdir $$<)...
		@$$(call prepare_to_execute,$$($1.EXECUTION_DIR),$$(dir $$($1.EXE_FILE)) $$($1.DLL_DIRS)) && $$(patsubst %,"$(CURDIR)/%",$$<) $(args)

  ifneq (,$$(filter $$(files:%=$(DIST_BIN_DIR)/%$(EXE_SUFFIX)),$$($1.EXE_FILE)))
    run: RUN.$1
  endif
endef

#��������� �������� � ��������� ������� ������ (��� ����, ��� ������)
define process_tests_source_dir
  ifeq (,$$($1.TARGET_DIR))
    $2.TARGET_DIR := $$($2.TMP_DIR)
  else
    $2.TARGET_DIR := $$($1.TARGET_DIR)
  endif

  $2.TEST_EXE_FILES    := $$(filter $$(files:%=$$($2.TARGET_DIR)/%$(EXE_SUFFIX)),$$(patsubst $$($2.TMP_DIR)/%$(OBJ_SUFFIX),$$($2.TARGET_DIR)/%$(EXE_SUFFIX),$$($2.OBJECT_FILES)))
  $2.TEST_RESULT_FILES := $$(patsubst $$($2.SOURCE_DIR)/%,$$($2.TMP_DIR)/%,$$(wildcard $$($2.SOURCE_DIR)/*.result))
  $2.EXECUTION_DIR     := $$(if $$($1.EXECUTION_DIR),$$($1.EXECUTION_DIR),$$($2.SOURCE_DIR))
  $1.TARGET_DLLS       := $$($1.TARGET_DLLS) $$($1.DLLS:%=$$($2.TARGET_DIR)/%$(DLL_SUFFIX))

  build: $$($2.TEST_EXE_FILES)
  test: TEST_MODULE.$2
  check: CHECK_MODULE.$2
  .PHONY: TEST_MODULE.$2 CHECK_MODULE.$2    
  
#������� ������ �����
  $$($2.TARGET_DIR)/%$(EXE_SUFFIX): $$($2.TMP_DIR)/%$(OBJ_SUFFIX) $$($1.LIB_DEPS)
		@echo Linking $$(notdir $$@)...
		@$$(call $(LINK_TOOL),$$@,$$(filter %$(OBJ_SUFFIX),$$<) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))

#������� ��������� �����-���������� ������������
  $$($2.TMP_DIR)/%.result: $$($2.TARGET_DIR)/%$(EXE_SUFFIX)
		@echo Running $$(notdir $$<)...
		@$$(call prepare_to_execute,$$($2.EXECUTION_DIR),$$($2.TARGET_DIR) $$($1.DLL_DIRS)) && $$(patsubst %,"$(CURDIR)/%",$$<) $(args) > $$(patsubst %,"$(CURDIR)/%",$$@)
		
#������� ��������� �����-���������� ������������ �� shell-�����
  $$($2.SOURCE_DIR)/%.sh: $$($2.TEST_EXE_FILES)

  $$($2.TMP_DIR)/%.result: $$($2.SOURCE_DIR)/%.sh
		@echo Running $$(notdir $$<)...
		@$$(call prepare_to_execute,$$($2.EXECUTION_DIR),$$($2.TARGET_DIR) $$($1.DLL_DIRS)) && chmod u+x $$(patsubst %,"$(CURDIR)/%",$$<) && $$(patsubst %,"$(CURDIR)/%",$$<) $(args) > $$(patsubst %,"$(CURDIR)/%",$$@)

#������� ������� ������
  TEST_MODULE.$2: $$($2.TEST_EXE_FILES)
		@$$(call prepare_to_execute,$$($2.EXECUTION_DIR),$$($2.TARGET_DIR) $$($1.DLL_DIRS)) && $$(call for_each_file,file,$$(patsubst %,"$(CURDIR)/%",$$(filter $$(files:%=$$($2.SOURCE_DIR)/%.sh),$$(wildcard $$($2.SOURCE_DIR)/*.sh)) $$(filter $$(files:%=$$($2.TARGET_DIR)/%$(EXE_SUFFIX)),$$^)),chmod u+x $$$$file && $$$$file $(args))

#������� �������� ����������� ������������
  CHECK_MODULE.$2: $$($2.TEST_RESULT_FILES)
		@echo Checking results of module '$2'...
		@$$(call for_each_file,file,$$(notdir $$(filter $$(files:%=$$($2.TMP_DIR)/%.result),$$^)),diff --strip-trailing-cr --context=1 $$($2.SOURCE_DIR)/$$$$file $$($2.TMP_DIR)/$$$$file)
endef

#��������� ���� test-suite (��� ����)
define process_target.test-suite
  $$(eval $$(call process_target_with_sources,$1,process_tests_source_dir))
  
  build: $$($1.FLAG_FILES)
endef

#��������� ����������� ������ (��� ������, ������� make)
define process_package_components
  $2: PACKAGE_$2.$1

  .PHONY: PACKAGE_$2.$1

  PACKAGE_$2.$1:
		@$$(foreach component,$$($1.COMPONENTS),$(MAKE) -C $$(subst :, ,$(COMPONENT_DIR)/$$(component)) $2 && ) true
endef

#��������� ���� package (��� ����)
define process_target.package
  ifneq (,$$($1.COMPONENTS))
    $$(foreach command,$(PACKAGE_COMMANDS),$$(eval $$(call process_package_components,$1,$$(command))))
  else
    $$(warning Empty package at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif  
endef

#��������� ����������� ��������� ���������� (��� �������� ����������, ��� �������������� ����������)
define process_subdir_installation
  DIST_DIRS := $(DIST_DIRS) $2

  $$(eval $$(call process_file_installation,$1/,*,$2))		
endef

#��������� ����������� �����(��) (��� �������� ����������, ���/����� ������, ��� �������������� ����������)
define process_file_installation
  ifneq (,$$(filter %*,$2)$$(filter *%,$2))	
    #��������� �������� �����

    SOURCE_INSTALLATION_DIRS       := $$(patsubst %/,%,$$(filter %/,$$(wildcard $1$2/)))
    SOURCE_INSTALLATION_FILES      := $$(filter-out $$(INSTALLATION_DIRS),$$(wildcard $1$2))
    DESTINATION_INSTALLATION_FILES := $$(SOURCE_INSTALLATION_FILES:$1%=$3/%)
  else
    ifneq (,$$(wildcard $1$2/*))
      #��������� ����������
      SOURCE_INSTALLATION_DIRS := $1$2
    else
      #��������� ������

      DESTINATION_INSTALLATION_FILES := $3/$2
    endif
  endif

  install: $$(DESTINATION_INSTALLATION_FILES)

  $3/%: $1%
		@cp -v "$$<" "$$@"

  #��������� ��������
  $$(foreach dir,$$(SOURCE_INSTALLATION_DIRS),$$(eval $$(call process_subdir_installation,$$(dir),$3/$$(dir:$1%=%))))
endef

#��������� ������������ ����������� ����� ������ (��� ����, ��� ���� �����������, �������������� ����������)
define process_target_installation_dependency
  $1.SOURCE_INSTALLATION_FILES      := $$($2.SOURCE_INSTALLATION_DLL_FILES) $$($2.SOURCE_INSTALLATION_LIB_FILES) $$($2.SOURCE_INSTALLATION_EXE_FILES)
  $1.DESTINATION_INSTALLATION_FILES := $$(foreach file,$$($1.SOURCE_INSTALLATION_FILES),$3/$$(notdir $$(file)))

  install: $$($1.DESTINATION_INSTALLATION_FILES)

  $3/%: $(DIST_LIB_DIR)/%
		@cp "$$<" "$$@" --verbose

  $3/%: $(DIST_BIN_DIR)/%
		@cp "$$<" "$$@" --verbose

  $$(foreach file,$$($1.SOURCE_INSTALLATION_DLL_FILES:%=$3/%),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))
endef

#��������� ���� installation (��� ����)
define process_target.installation

ifneq (,$$(filter install,$$(MAKECMDGOALS)))
  $1.INSTALLATION_DIR := $(DIST_DIR)/$$($1.INSTALLATION_DIR)
  $1.SOURCE_FILES     := $$(sort $$(call specialize_paths,$$($1.SOURCE_FILES)))
  DIST_DIRS           := $(DIST_DIRS) $$($1.INSTALLATION_DIR)  

  $$(foreach file,$$($1.SOURCE_FILES),$$(eval $$(call process_file_installation,$$(dir $$(file)),$$(notdir $$(file)),$$($1.INSTALLATION_DIR))))  		

  $$(foreach target,$$($1.TARGETS),$$(eval $$(call process_target_installation_dependency,$1,$$(target),$$($1.INSTALLATION_DIR))))
endif

endef

#��������� ���� ������������ ������������(��� ����)
define process_target.doxygen-info 
  $1.TMP_DIR               := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.SOURCE_DIRS           := $$(call specialize_paths,$$($1.SOURCE_DIRS))
  $1.DOXYGEN_CFG_FILE      := $$($1.TMP_DIR)/doxygen.cfg
  $1.DOXYGEN_TAG_FILE      := $(TAGS_DIR)/$1.tag
  $1.DOXYGEN_CHM_FILE      := $$(DIST_INFO_DIR)/$$($1.CHM_NAME).chm
  $1.DOXYGEN_SELF_CHM_FILE := $$(if $$(strip $$($1.IMPORT_CHM)),$$($1.TMP_DIR)/index.chm,$$($1.DOXYGEN_CHM_FILE))
  $1.IMPORT_CHM            := $$($1.IMPORT_CHM:%=$$(DIST_INFO_DIR)/%.chm)
  TMP_DIRS                 := $$($1.TMP_DIR) $$(TMP_DIRS)
  TMP_CLEAN_DIRS           := $(TMP_CLEAN_DIRS) $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1 $$($1.DOXYGEN_TAG_FILE) 

  info: INFO.$1

  .PHONY: INFO.$1

  INFO.$1: $$($1.DOXYGEN_CHM_FILE)  

ifneq (,$$($1.SOURCE_DIRS))  

  #�������� ������� ���������� � ���� � ������� ���������������� ����

  ifeq (,$(DOXYGEN_DIR))
    $(error Documentation tool not found (DOXYGEN_DIR not defined))
  endif

  $$($1.DOXYGEN_SELF_CHM_FILE): $(TAGS_DIR) $$(DIST_INFO_DIR) $$($1.TMP_DIR) $$($1.DOXYGEN_CFG_FILE)
		@echo Generate $$(notdir $$@)...
		@"$$(DOXYGEN_DIR)/doxygen" $$($1.DOXYGEN_CFG_FILE)
		@$(MV) $$($1.TMP_DIR)/index.chm "$$@"

  $$($1.DOXYGEN_CFG_FILE): $$($1.TMP_DIR) force  
		@cp "$$(DOXYGEN_TEMPLATE_CFG_FILE)" "$$@"
		@echo "OUTPUT_DIRECTORY = $$($1.TMP_DIR)" >> "$$@"
		@echo "INCLUDE_PATH = $$($1.INCLUDE_DIRS)" >> "$$@"
		@echo "INPUT = $$($1.SOURCE_DIRS)" >> "$$@"
		@echo "GENERATE_TAGFILE =  $$($1.DOXYGEN_TAG_FILE)" >> "$$@"
		@echo TAGFILES = `cd $(TAGS_DIR) && for file in $$$$(ls); do if [ $(TAGS_DIR)/$$$$file != $$($1.DOXYGEN_TAG_FILE) ]; then echo $(TAGS_DIR)/$$$$file=../../../$$$$(basename $$$$file .tag)/html; fi; done` >> "$$@"
		
  $1.IMPORT_CHM := $$($1.IMPORT_CHM) $$($1.DOXYGEN_SELF_CHM_FILE)

endif
  
ifneq ($$($1.DOXYGEN_CHM_FILE),$$($1.DOXYGEN_SELF_CHM_FILE))

ifeq (,$$(HHC_DIR))
    $$(error 'Microsoft Help Workshop not found (set environment variable HHC_DIR to its directory)')
endif

  $1.MERGE_HHP         := $$($1.TMP_DIR)/merge.hhp
  $1.MERGE_HHC         := $$($1.TMP_DIR)/index.hhc
  $1.MERGE_HHK         := $$($1.TMP_DIR)/index.hhk
  $1.DEFAULT_TOPIC     := $$(call specialize_paths,$$($1.DEFAULT_TOPIC))
  $1.DEFAULT_TOPIC     := $$(if $$(wildcard $$($1.DEFAULT_TOPIC)),$$($1.DEFAULT_TOPIC),$$(DOXYGEN_DEFAULT_TOPIC))
	$1.DST_DEFAULT_TOPIC := $$($1.TMP_DIR)/default_topic.html
	$1.DST_IMPORT_CHM    := $$($1.IMPORT_CHM:$$(DIST_INFO_DIR)/%=$$($1.TMP_DIR)/%)  
  
  $$($1.DST_DEFAULT_TOPIC): $$($1.DEFAULT_TOPIC)
		@cp "$$<" "$$@"

  $$($1.TMP_DIR)/%.chm: $$(DIST_INFO_DIR)/%.chm
		@cp "$$<" "$$@"

  $$($1.MERGE_HHC): force
		@echo "<html><body>" > "$$@"
		@echo '<object type="text/site properties"><param name="FrameName" value="right"><param name="Window Styles" value="0x800025"></object>' >> "$$@"
		@$$(foreach file,$$($1.IMPORT_CHM),echo '<ul>' >> "$$@" && echo '<li><object type="text/sitemap"><param name="Name" value="$$(basename $$(notdir $$(file)))"><param name="Local" value="default_topic.html"></object>'  >> "$$@" && echo '<li>' >> "$$@" && echo '<object type="text/sitemap">' >> "$$@" && echo '<param name="Merge" value="$$(notdir $$(file))::\index.hhc">' >> "$$@" && echo '</object>' >>"$$@" && echo '</ul>' >> "$$@" && ) true
		@echo "</body></html>" >> "$$@"

  $$($1.MERGE_HHK): force
		@echo "<html><body>" > "$$@"
		@echo '<object type="text/site properties"><param name="FrameName" value="right">' >> "$$@" && echo '</object>' >> "$$@"
		@echo '<ul>' >> "$$@" && echo '<li> <object type="text/sitemap"><param name="Name" value="default">' >> "$$@" && echo '<param name="Local" value="default_topic.html">' >> "$$@"
		@echo "</object>" >> "$$@" && echo "</ul></body></html>" >> "$$@"

  $$($1.MERGE_HHP): force
		@echo "[OPTIONS]" > "$$@"
		@echo "Auto Index=Yes" >> "$$@"
		@echo "Binary Index=Yes" >> "$$@"
		@echo "Compatibility=1.1" >> "$$@"
		@echo "Compiled file=merged.chm" >> "$$@"
		@echo "Contents file=index.hhc" >> "$$@"
		@echo "Default Window=TP" >> "$$@"
		@echo "Default topic=default_topic.html" >> "$$@"
		@echo "Error log file=error_log.txt" >> "$$@"
		@echo "Full-text search=Yes" >> "$$@"
		@echo "Index file=index.hhk" >> "$$@"
		@echo "Language=0x409 English (United States)" >> "$$@"
		@echo "Title=Master" >> "$$@"
		@echo "[WINDOWS]" >> "$$@"		
		@echo 'TP=,"index.hhc","index.hhk","default_topic.html","default_topic.html",,,,,0x63520,222,0x304e,[0,0,879,615],0xb0000,,,,,,0' >> "$$@"
		@echo "[MERGE FILES]" >> "$$@"
		@$$(foreach file,$$($1.IMPORT_CHM),echo $$(notdir $$(file)) >> "$$@" && ) true
		@echo "[FILES]" >> "$$@"
		@echo default_topic.html >> "$$@"

  $$($1.DOXYGEN_CHM_FILE): $$($1.IMPORT_CHM) $$($1.TMP_DIR) $$($1.DST_DEFAULT_TOPIC) $$($1.DST_IMPORT_CHM) $$($1.MERGE_HHP) $$($1.MERGE_HHK) $$($1.MERGE_HHC)
		@echo Merging $$(notdir $$@)...
ifeq ($$($1.DEFAULT_TOPIC),$$(DOXYGEN_DEFAULT_TOPIC))
		@echo Default topic for doxygen documentation merge not defined. Using default (set target property DEFAULT_TOPIC for use custom default topic page)
endif
		@$$(HHC_DIR)/hhc $$($1.MERGE_HHP) || true
		@mv $$($1.TMP_DIR)/merged.chm "$$@"

endif
  
endef

#�������������� ���������� (������� ���������, ������� ��������, ������������� ���� � ������������� ����������)
define import_variables
#  $$(warning src='$1' dst='$2' path='$3')  

  $2.INCLUDE_DIRS         := $$($2.INCLUDE_DIRS) $$($1.INCLUDE_DIRS:%=$3%)
  $2.INCLUDE_FILES        := $$($2.INCLUDE_FILES) $$($1.INCLUDE_FILES)
  $2.LIB_DIRS             := $$($2.LIB_DIRS) $$($1.LIB_DIRS:%=$3%)
  $2.DLL_DIRS             := $$($2.DLL_DIRS) $$($1.DLL_DIRS:%=$3%)
  $2.DLLS                 := $$($2.DLLS) $$($1.DLLS)
  $2.LIBS                 := $$($2.LIBS) $$($1.LIBS)
  $2.EXCLUDE_DEFAULT_LIBS := $$($2.EXCLUDE_DEFAULT_LIBS) $$($1.EXCLUDE_DEFAULT_LIBS)
  $2.COMPILER_CFLAGS      := $$($2.COMPILER_CFLAGS) $$($1.COMPILER_CFLAGS)
  $2.COMPILER_DEFINES     := $$($2.COMPILER_DEFINES) $$($1.COMPILER_DEFINES)
  $2.LINK_INCLUDES        := $$($2.LINK_INCLUDES) $$($1.LINK_INCLUDES)
  $2.LINK_FLAGS           := $$($2.LINK_FLAGS) $$($1.LINK_FLAGS)
  $2.COMPONENTS           := $$($2.COMPONENTS) $$($1.COMPONENTS)
endef

#�������������� �������� (��� �����������, ��� ����)
define import_settings
# �������� ������������ �������
ifeq (,$$(filter $1,$$(PROCESSED_IMPORTS)))
  PROCESSED_IMPORTS := $$(PROCESSED_IMPORTS) $1

#�������� ������� ����������
ifeq (,$1)
  $$(error Component '$1' not exported (unresolved import))
endif

#��������� ��������
  $$(eval $$(call import_variables,$(EXPORT_VAR_PREFIX).$1,$2,$$(patsubst $(COMPONENT_DIR)%,%,$$(paths.$1))))
  $$(foreach profile,$(PROFILES),$$(eval $$(call import_variables,$(EXPORT_VAR_PREFIX).$1.$$(profile),$2,$$(paths.$1))))

#�������������� ��������� ������������
  DEPENDENCY_IMPORTS := $$($(EXPORT_VAR_PREFIX).$1.IMPORTS) $$(foreach profile,$(PROFILES),$$($(EXPORT_VAR_PREFIX).$1.$$(profile).IMPORTS))  

  $$(foreach imp,$$(DEPENDENCY_IMPORTS),$$(eval $$(call import_settings,$$(imp),$2)))

endif

endef

#������ �������� toolset-�������� (��� ����, ��� �������)
define import_toolset_settings
$$(foreach var,$$(filter $1.$2.%,$$(.VARIABLES)),$$(eval $$(var:$1.$2.%=$1.%) = $$($$(var:$1.$2.%=$1.%)) $$($$(var))))
endef

#��������� ������ ���� (��� ����)
define process_target_common
  PROCESSED_IMPORTS :=

    #���������� toolset-�������� � ������ ������ ��������
  $$(foreach profile,$$(PROFILES),$$(eval $1.IMPORTS := $$($1.IMPORTS) $$($1.$$(profile).IMPORTS)))  

    #������ ��������
  $$(foreach imp,$$($1.IMPORTS),$$(eval $$(call import_settings,$$(imp),$1)))
  
    #���������� toolset-�������� � ������ ������ ��������
  $$(foreach profile,$$(PROFILES),$$(eval $$(call import_toolset_settings,$1,$$(profile))))  

  DUMP.$1:
		@echo Dump target \'$1\' settings
		@$$(foreach var,$$(sort $$(filter $1.%,$(.VARIABLES))),echo '    $$(var:$1.%=%) = $$($$(var))' && ) true

  dump: DUMP.$1

  .PHONY: DUMP.$1
  
  $$(eval $$(call process_target.$$(strip $$($1.TYPE)),$1))  
endef

#�������� ������������ ���� ���� (��� ����)
define test_target_type
  ifeq (,$$(findstring $$(strip $$($1.TYPE)),$(VALID_TARGET_TYPES)))
    $$(error Wrong target type '$$(strip $$($1.TYPE))' at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif
endef

###################################################################################################
#������� ������
###################################################################################################
all: build check
run: build
build: create-dirs
rebuild: clean build
install: build
test: build
check: build
force:

.PHONY: build rebuild clean fullyclean run test check help create-dirs force dump info

#������������� ������ ����� (� ����������� �� �������)
$(foreach profile,$(PROFILES),$(eval TARGETS := $$(TARGETS) $$(TARGETS.$$(profile))))  

#��������� ����� ����������
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call test_target_type,$(target))))
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call process_target_common,$(target))))

#�������� ���������
create-dirs: $(DIRS)

$(DIRS):
	@mkdir -p $@

#�������
clean:
	@cd $(ROOT_TMP_DIR) && $(RM) -r $(TMP_CLEAN_DIRS:$(ROOT_TMP_DIR)/%=%)

fullyclean: clean
	@$(RM) -r $(DIRS)

#�������� ������ � �������������
tar-dist: dist
	@echo Create $(basename $(DIST_DIR)).tar...
	@tar -cf $(basename $(DIST_DIR)).tar $(DIST_DIR)
