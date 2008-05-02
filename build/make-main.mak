###################################################################################################
#��������� ������
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := component.mak #������� ��� ����� ������������ ����������
TMP_DIR_SHORT_NAME                      := tmp           #������� ��� �������� � ���������� �������
SOURCE_FILES_SUFFIXES                   := c cpp         #���������� �������� ������
BUILD_DIR_SHORT_NAME                    := build         #������� ��� �������� �� ��������� ������
TMP_DIR_SHORT_NAME                      := tmp           #������� ��� �������� � ���������� ������� ������
DIST_DIR_SHORT_NAME                     := dist          #������� ��� �������� � ������������ ������
PCH_SHORT_NAME                          := pch.h         #������� ��� PCH �����
EXPORT_VAR_PREFIX                       := export        #������� ����� ���������� ��������������� �������� ����������
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := batch-flag    #������� ��� �����-����� �������� ����������
VALID_TARGET_TYPES                      := static-lib dynamic-lib application test-suite package #���������� ���� �����
PACKAGE_COMMANDS                        := build clean test check run #�������, ������������ ����������� ������

###################################################################################################
#����������� ���� � ����������
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := $(strip $(COMPONENT_CONFIGURATION_FILE_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
SOURCE_FILES_SUFFIXES                   := $(strip $(SOURCE_FILES_SUFFIXES))
BUILD_DIR_SHORT_NAME                    := $(strip $(BUILD_DIR_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
DIST_DIR_SHORT_NAME                     := $(strip $(DIST_DIR_SHORT_NAME))
EXPORT_VAR_PREFIX                       := $(strip $(EXPORT_VAR_PREFIX))
DIST_DIR                                := $(ROOT)/$(DIST_DIR_SHORT_NAME)
DIST_LIB_DIR                            := $(DIST_DIR)/lib
DIST_BIN_DIR                            := $(DIST_DIR)/bin
BUILD_DIR                               := $(ROOT)/$(BUILD_DIR_SHORT_NAME)
PCH_SHORT_NAME                          := $(strip $(PCH_SHORT_NAME))
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := $(strip $(BATCH_COMPILE_FLAG_FILE_SHORT_NAME))
ROOT_TMP_DIR                            := $(ROOT)/$(TMP_DIR_SHORT_NAME)
TMP_DIRS                                := $(ROOT_TMP_DIR)
DIRS                                     = $(TMP_DIRS) $(DIST_DIR) $(DIST_LIB_DIR) $(DIST_BIN_DIR)
EMPTY                                   :=
SPACE                                   := $(EMPTY) $(EMPTY)

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
#����������� ����� ������������ ����������
###################################################################################################
COMPONENT_CONFIGURATION_FILE := $(firstword $(wildcard $(filter %$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME), $(MAKEFILE_LIST:makefile=$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME)))))
COMPONENT_DIR                := $(dir $(COMPONENT_CONFIGURATION_FILE))

ifeq (,$(COMPONENT_CONFIGURATION_FILE))
  $(error Configuration file '$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME)' not found at project tree (root='$(ROOT)'))
endif

include $(COMPONENT_CONFIGURATION_FILE)

###################################################################################################
#������������� ����
###################################################################################################
define specialize_paths
$(foreach dir,$1,$(COMPONENT_DIR)$(dir))
endef

###################################################################################################
#���������� ���������� (������ ����������, ������ ������������ ���������, ������� � ���������� �������,
#������ ��������, ����� ����������, pch ����)
###################################################################################################
define tools.msvc.compile
cl -nologo -c -EHsc -W3 -wd4996 $(if $(analyze),-analyze) -FC -Fo"$3\\" $(patsubst %,-I"%",$2) $5 $(patsubst %,-D%,$4) $1 $(if $6,-FI"$6" -Yc"$6" -Fp"$3\\")
endef

###################################################################################################
#��������� ���� ����������
###################################################################################################

#�������� ����������� ���������� ����� �� ����-����� �������� ���������� (��� ��������� �����, ��������� �������, ����-����)
define create_object_file_dependency
$2/$$(notdir $$(basename $1)).obj: $1 $3
	@
endef

#������������ ������� ��������� �������� ������ � �����. �� ��������� (������ �������� ������, ��������� �������)
define test_source_and_object_files
$(foreach src,$1,if [ $(src) -nt $2/$(notdir $(basename $(src))).obj ]; then echo $(src); fi &&) true
endef

#������� �������� ���������� (��� ����, ��� ������)
define batch-compile
  $2.FLAG_FILE  := $$($2.TMP_DIR)/$$(BATCH_COMPILE_FLAG_FILE_SHORT_NAME)
  $1.FLAG_FILES := $$($1.FLAG_FILES) $$($2.FLAG_FILE)

  ifeq (,$$(wildcard $$($2.FLAG_FILE).incomplete-build))
  
    $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
			@echo build-start > $$@.incomplete-build
			@$$(call tools.msvc.compile,$$(sort $$(filter-out force,$$?) $$($2.NEW_SOURCE_FILES)),$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS),$$($2.TMP_DIR),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS),$$($2.PCH))
			@echo batch-flag-file > $$@
			@$(RM) $$@.incomplete-build

  else  #� ������ ���� ������ ���� �� ���������
  
    $$($2.FLAG_FILE): $2.UPDATED_SOURCE_FILES := $$(shell $$(call test_source_and_object_files,$$($2.SOURCE_FILES),$$($2.TMP_DIR)))
  
    $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
			@$$(call tools.msvc.compile,$$(sort $$($2.UPDATED_SOURCE_FILES) $$($2.NEW_SOURCE_FILES)),$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS),$$($2.TMP_DIR),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS),$$($2.PCH))
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

  $$(MODULE_NAME).SOURCE_DIR       := $2
  $$(MODULE_NAME).TMP_DIR          := $$($1.TMP_DIR)/$$(MODULE_PATH)
  $$(MODULE_NAME).OBJECT_FILES     := $$(patsubst %,$$($$(MODULE_NAME).TMP_DIR)/%.obj,$$(notdir $$(basename $$($$(MODULE_NAME).SOURCE_FILES))))
  $$(MODULE_NAME).NEW_SOURCE_FILES := $$(patsubst ./%,%,$$(strip $$(foreach file,$$($$(MODULE_NAME).SOURCE_FILES),$$(if $$(wildcard $$(patsubst %,$$($$(MODULE_NAME).TMP_DIR)/%.obj,$$(notdir $$(basename $$(file))))),,$$(file)))))
  $$(MODULE_NAME).PCH              := $$(if $$(wildcard $2/$(PCH_SHORT_NAME)),$(PCH_SHORT_NAME))
  $1.TMP_DIRS                      := $$($$(MODULE_NAME).TMP_DIR) $$($1.TMP_DIRS)
  $1.OBJECT_FILES                  := $$($1.OBJECT_FILES) $$($$(MODULE_NAME).OBJECT_FILES)

  $$(foreach macros,batch-compile $3,$$(eval $$(call $$(macros),$1,$$(MODULE_NAME))))
endef

#�������� ����������� ��� ����������� ������� ������ (��� �������� �����, �������� ������)
define create_extern_file_dependency
  $1: $$(firstword $$(wildcard $$(patsubst %,%/$$(notdir $1),$2)) $$(notdir $1))
		@cp -fv $$< $$@
		@chmod ug+rwx $$@
endef

#����� ��� ����� � ��������� ������� (��� ����, ������ �������� ����������� ��� ��������� ��������� � ��������� �������)
define process_target_with_sources
  $1.INCLUDE_DIRS  := $$(call specialize_paths,$$($1.INCLUDE_DIRS))
  $1.SOURCE_DIRS   := $$(call specialize_paths,$$($1.SOURCE_DIRS))
  $1.LIB_DIRS      := $$(call specialize_paths,$$($1.LIB_DIRS)) $(DIST_LIB_DIR)
  $1.DLL_DIRS      := $$(call specialize_paths,$$($1.DLL_DIRS))
  $1.EXECUTION_DIR := $$(strip $$($1.EXECUTION_DIR))
  $1.EXECUTION_DIR := $$(if $$($1.EXECUTION_DIR),$(COMPONENT_DIR)$$($1.EXECUTION_DIR))
  $1.LIBS          := $$($1.LIBS:%=%.lib)
  $1.TARGET_DLLS   := $$($1.DLLS:%=$(DIST_BIN_DIR)/%.dll)
  $1.LIB_DEPS      := $$(filter $$(addprefix %/,$$($1.LIBS)),$$(TARGET_FILES))
  $1.TMP_DIR       := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$1
  $1.TMP_DIRS      := $$($1.TMP_DIR)

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_source_dir,$1,$$(dir),$2)))

  TMP_DIRS := $$($1.TMP_DIRS) $$(TMP_DIRS)
  TMP_CLEAN_DIRS := $(TMP_CLEAN_DIRS) $(ROOT)/$(TMP_DIR_SHORT_NAME)/$1

  build: $$($1.TARGET_DLLS)

  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))
endef

#��������� ���� static-lib (��� ����)
define process_target.static-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty static name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.LIB_FILE  := $(DIST_LIB_DIR)/$$($1.NAME)$$(if $$(suffix $$($1.NAME)),,.lib)
  TARGET_FILES := $$(TARGET_FILES) $$($1.LIB_FILE)
  
  build: $$($1.LIB_FILE)

  $$(eval $$(call process_target_with_sources,$1))  

  $$($1.LIB_FILE): $$($1.FLAG_FILES)
		@echo Create library $$(notdir $$@)...
		@lib -nologo -out:$$@ $$($1.OBJECT_FILES)
endef

#��������� ���� dynamic-lib (��� ����)
define process_target.dynamic-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty dynamic library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.DLL_FILE  := $(DIST_BIN_DIR)/$$($1.NAME)$$(if $$(suffix $$($1.NAME)),,.dll)
  TARGET_FILES := $$(TARGET_FILES) $$($1.DLL_FILE) $(DIST_LIB_DIR)/$$(notdir $$(basename $$($1.DLL_FILE))).lib

  build: $$($1.DLL_FILE)  

  $$(eval $$(call process_target_with_sources,$1))

  $$($1.DLL_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Create dynamic library $$(notdir $$@)...
		@link $$($1.OBJECT_FILES) $$($1.LIBS) -nologo -dll -out:"$$@" $$($1.LIB_DIRS:%=-libpath:"%") $$($1.LINK_FLAGS)
		@$(RM) $$(basename $$@).exp
		@mv -f $$(basename $$@).lib $(DIST_LIB_DIR)
endef

#��������� ���� application (��� ����)
define process_target.application
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty application name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.EXE_FILE  := $(DIST_BIN_DIR)/$$($1.NAME)$$(if $$(suffix $$($1.NAME)),,.exe)
  TARGET_FILES := $$(TARGET_FILES) $$($1.EXE_FILE)

  build: $$($1.EXE_FILE)
  run: RUN.$1

  $$(eval $$(call process_target_with_sources,$1))
  
  ifeq (,$$($1.EXECUTION_DIR))
    $1.EXECUTION_DIR := $(DIST_BIN_DIR)
  endif

  $$($1.EXE_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Linking $$(notdir $$@)...
		@link $$($1.OBJECT_FILES) $$($1.LIBS) -nologo -out:"$$@" $$($1.LIB_DIRS:%=-libpath:"%") $$($1.LINK_FLAGS)
		
  RUN.$1: $$($1.EXE_FILE)
		@echo Running $$(notdir $$<)...
		@export PATH="$(CURDIR)/$(DIST_BIN_DIR):$$(PATH)" && cd $$($1.EXECUTION_DIR) && $$(patsubst %,"$(CURDIR)/%",$$<)
endef

#��������� �������� � ��������� ������� ������ (��� ����, ��� ������)
define process_tests_source_dir
  $2.TEST_EXE_FILES    := $$(filter $$(files:%=$$($2.TMP_DIR)/%.exe),$$($2.OBJECT_FILES:%.obj=%.exe))
  $2.TEST_RESULT_FILES := $$(patsubst $$($2.SOURCE_DIR)/%,$$($2.TMP_DIR)/%,$$(wildcard $$($2.SOURCE_DIR)/*.result))
  $2.EXECUTION_DIR     := $$(if $$($1.EXECUTION_DIR),$$($1.EXECUTION_DIR),$$($2.SOURCE_DIR))

  build: $$($2.TEST_EXE_FILES)
  test: TEST_MODULE.$2
  check: CHECK_MODULE.$2
  .PHONY: TEST_MODULE.$2 CHECK_MODULE.$2
  
#������� ������ �����
  $$($2.TMP_DIR)/%.exe: $$($2.TMP_DIR)/%.obj $$($1.LIB_DEPS)
		@echo Linking $$(notdir $$@)...
		@link $$(filter %.obj,$$<) $$($1.LIBS) -nologo -out:"$$@" $$($1.LIB_DIRS:%=-libpath:"%") $$($1.LINK_FLAGS)

#������� ��������� �����-���������� ������������
  $$($2.TMP_DIR)/%.result: $$($2.TMP_DIR)/%.exe
		@echo Running $$(notdir $$<)...
		@export PATH="$(CURDIR)/$(DIST_BIN_DIR):$$(PATH)" && cd $$($2.EXECUTION_DIR) && $$(patsubst %,"$(CURDIR)/%",$$<) > $$(patsubst %,"$(CURDIR)/%",$$@)

#������� ������� ������
  TEST_MODULE.$2: $$($2.TEST_EXE_FILES)
		@export PATH="$(CURDIR)/$(DIST_BIN_DIR):$$(PATH)" && cd $$($2.EXECUTION_DIR) && for file in $$(patsubst %,"$(CURDIR)/%",$$(filter $$(files:%=$$($2.TMP_DIR)/%.exe),$$^)); do $$$$file; done

#������� �������� ����������� ������������
  CHECK_MODULE.$2: $$($2.TEST_RESULT_FILES)
		@echo Checking results of module '$2'...
		@for file in $$(notdir $$(filter $$(files:%=$$($2.TMP_DIR)/%.result),$$^)); do diff --strip-trailing-cr --context=1 $$($2.SOURCE_DIR)/$$$$file $$($2.TMP_DIR)/$$$$file; done
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

#�������������� ��������
define import_settings
#  $$(warning exporter=$1 importer=$2)

#��������� �������������� ���� � ������������� ����������
  DEPENDENCY_EXPORT_FILE   := $$(COMPONENT_DIR)$1
  DEPENDENCY_COMPONENT_DIR := $$(dir $$(DEPENDENCY_EXPORT_FILE))

#�������� ������� ����� �����������
  $$(if $$(wildcard $$(DEPENDENCY_EXPORT_FILE)),,$$(error Component export file '$$(DEPENDENCY_EXPORT_FILE)' not found))

#������� ���������� $(EXPORT_VAR_PREFIX).*
  $$(foreach var,$$(filter $$(EXPORT_VAR_PREFIX).%,$$(.VARIABLES)),$$(eval $$(var) :=))

#����������� ����� �����������
  include $$(DEPENDENCY_EXPORT_FILE)

#��������� ��������
  $2.INCLUDE_DIRS     := $$($2.INCLUDE_DIRS) $$($$(EXPORT_VAR_PREFIX).INCLUDE_DIRS:%=$$(DEPENDENCY_COMPONENT_DIR)%)
  $2.LIB_DIRS         := $$($2.LIB_DIRS) $$($$(EXPORT_VAR_PREFIX).LIB_DIRS:%=$$(DEPENDENCY_COMPONENT_DIR)%)
  $2.DLL_DIRS         := $$($2.DLL_DIRS) $$($$(EXPORT_VAR_PREFIX).DLL_DIRS:%=$$(DEPENDENCY_COMPONENT_DIR)%)
  $2.LIBS             := $$($2.LIBS) $$($$(EXPORT_VAR_PREFIX).LIBS)
  $2.COMPILER_CFLAGS  := $$($2.COMPILER_CFLAGS) $$($$(EXPORT_VAR_PREFIX).COMPILER_CFLAGS)
  $2.COMPILER_DEFINES := $$($2.COMPILER_DEFINES) $$($$(EXPORT_VAR_PREFIX).COMPILER_DEFINES)
  DEPENDENCY_IMPORTS  := $$($$(EXPORT_VAR_PREFIX).IMPORTS:%=$(dir $1)%)  

#�������������� ��������� ������������
  $$(foreach imp,$$(DEPENDENCY_IMPORTS),$$(eval $$(call import_settings,$$(imp),$2)))
endef

#��������� ������ ����
define process_target_common
  $$(foreach imp,$$($1.IMPORTS),$$(eval $$(call import_settings,$$(imp),$1)))
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
build: create_dirs
rebuild: clean build
test: build
check: build
force:

.PHONY: build rebuild clean fullyclean run test check help create_dirs force

#��������� ����� ����������
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call test_target_type,$(target))))
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call process_target_common,$(target))))

#�������� ���������
create_dirs: $(DIRS)

$(DIRS):
	@mkdir -p --mode=777 $@

#�������
clean:
	@cd $(ROOT_TMP_DIR) && $(RM) -r $(TMP_CLEAN_DIRS:$(ROOT_TMP_DIR)/%=%)

fullyclean: clean
	@$(RM) -r $(DIRS)
