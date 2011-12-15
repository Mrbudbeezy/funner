###################################################################################################
#���� ������ "�� ���������"
###################################################################################################
default: build

.PHONY: default

###################################################################################################
#��������� ������
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := component.mak #������� ��� ����� ������������ ����������
EXPORT_FILE_SHORT_NAME                  := export.mak    #������� ��� ����� ��������
PROCESS_DIR_CONFIG_FILE_SHORT_NAME      := config.mak    #������� ��� ����� ������������ ��������� ����������
TMP_DIR_SHORT_NAME                      := tmp           #������� ��� �������� � ���������� �������
DIST_DIR_SHORT_NAME                     := dist          #������� ��� �������� � ������������ ������
PROFILES_DIR_SHORT_NAME                 := profiles      #������� ��� �������� � �������������� toolset-��
VALID_TARGET_TYPES                      := package lua-module test-suite zip-package nd-info #���������� ���� �����
PACKAGE_COMMANDS                        := build clean test check run install info #�������, ������������ ����������� ������
DEFAULT_CONFIGURATION                    = $(CURDIR)/$(BUILD_DIR)default_config.xml
DEFAULT_EXECUTION_FLAGS                 := --no-main-loop --config=$$(DEFAULT_CONFIGURATION)

###################################################################################################
#����������� �������� ������������
###################################################################################################
BUILD_DIR  := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
USER       ?= $(USERNAME)

-include $(ROOT)/$(USER).settings.mak

###################################################################################################
#�������� ������� ���������� FUNNER_SDK
###################################################################################################
ifeq (,$(FUNNER_SDK))
  $(error Funner SDK not defined. Please set FUNNER_SDK variable)
endif

###################################################################################################
#����������� ���� � ����������
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := $(strip $(COMPONENT_CONFIGURATION_FILE_SHORT_NAME))
EXPORT_FILE_SHORT_NAME                  := $(strip $(EXPORT_FILE_SHORT_NAME))
PROCESS_DIR_CONFIG_FILE_SHORT_NAME      := $(strip $(PROCESS_DIR_CONFIG_FILE_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
DIST_DIR_SHORT_NAME                     := $(strip $(DIST_DIR_SHORT_NAME))
PROFILES_DIR                            := $(BUILD_DIR)$(strip $(PROFILES_DIR_SHORT_NAME))
AVAILABLE_PROFILES                      := $(patsubst $(PROFILES_DIR)/%.mak,%,$(wildcard $(PROFILES_DIR)/*.mak))
EXPORT_VAR_PREFIX                       := export
CURRENT_PROFILE                         := $(PROFILE)
PROFILE_FILE                            := $(PROFILES_DIR)/$(CURRENT_PROFILE).mak
DIST_DIR                                := $(ROOT)/$(DIST_DIR_SHORT_NAME)/lua.$(CURRENT_PROFILE)
DIST_LUA_MODULES_DIR                    := $(DIST_DIR)/scripts
DIST_BIN_DIR                            := $(DIST_DIR)/bin
DIST_INFO_DIR                           := $(DIST_DIR)/info
ROOT_TMP_DIR                            := $(ROOT)/$(TMP_DIR_SHORT_NAME)/lua.$(CURRENT_PROFILE)
TMP_DIRS                                := $(ROOT_TMP_DIR)
DIST_DIRS                               :=
DIRS                                     = $(TMP_DIRS) $(DIST_DIRS)
TOOLS_DIR                               := $(FUNNER_SDK)/bin
EMPTY                                   := 
SPACE                                   := $(EMPTY) $(EMPTY)
COMMA                                   := ,
LUA_COMPILER                            := $(TOOLS_DIR)/luac
CLAUNCHER                               := $(TOOLS_DIR)/clauncher
NATURAL_DOCS_CONFIG_DIR                 := $(BUILD_DIR)nd

###################################################################################################
#���� �� ������ ������� - ������������ ��� ���������
###################################################################################################
files ?= %
targets ?= %

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

EXPORT_FILES  := $(wildcard $(filter %$(EXPORT_FILE_SHORT_NAME), $(MAKEFILE_LIST:makefile=$(EXPORT_FILE_SHORT_NAME))))

$(foreach file,$(EXPORT_FILES),$(eval $(call load_exports,$(file))))
#$(foreach var,$(sort $(filter export.%,$(.VARIABLES))),$(warning $(var)=$($(var))))
#$(foreach path,$(sort $(filter paths.%,$(.VARIABLES))),$(warning $(path)=$($(path))))

###################################################################################################
#����������� ��������� ������ ������
###################################################################################################

#��������� ��������� ���������� (���������������� ��������, ��� �������� ����������, ��� �������������� ����������, ������ � ����� ��� ������� ��������)
define process_subdir
  $$(eval $$(call process_files,$1,$2,*,$3,$4))
endef

#��������� �����(��) (���������������� ��������, ��� ������, ��� �������� ����������, ���/����� ������, ��� �������������� ����������, ������ � ����� ��� ������� ��������)
define process_files_impl
  $2.CONFIG_FILE := $3/$(PROCESS_DIR_CONFIG_FILE_SHORT_NAME)
  
  FILE_LIST :=

  -include $$($2.CONFIG_FILE)

  ifneq (,$$(filter %*,$4))
    #��������� �������� �����

    $2.SOURCE_INSTALLATION_DIRS := $$(patsubst %/,%,$$(filter %/,$$(wildcard $3$4/)))

    ifeq (,$$(FILE_LIST))
      $2.SOURCE_INSTALLATION_FILES := $$(filter-out %/,$$(wildcard $3$4/))
    else
      $2.SOURCE_INSTALLATION_FILES := $$(FILE_LIST:%=$3%)
    endif
  else
#    ifneq (,$$(filter $3$4/,$$(wildcard $3$4/)))
    ifneq (,$$(wildcard $3$4/*))
      #��������� ����������
      $2.SOURCE_INSTALLATION_DIRS  := $3$4
      $2.SOURCE_INSTALLATION_FILES :=
    else
      $2.SOURCE_INSTALLATION_DIRS :=
      #��������� ������
      ifeq (,$$(FILE_LIST))
        $2.SOURCE_INSTALLATION_FILES := $3$4
      else
        $2.SOURCE_INSTALLATION_FILES := $$(FILE_LIST:%=$3%)
      endif
    endif
  endif    

#��������� ��������
  $$(foreach dir,$$($2.SOURCE_INSTALLATION_DIRS),$$(eval $$(call process_subdir,$1,$$(dir),$5/$$(dir:$3%=%),$6)))
  
  #��������� ������ ������ � ������� ����������
  ifneq (,$$($2.SOURCE_INSTALLATION_FILES))
    $$(eval $$(call $6,$1,$$($2.SOURCE_INSTALLATION_FILES),$5))
  endif
endef

#��������� �����(��) (���������������� ��������, ��� �������� ����������, ���/����� ������, ��� �������������� ����������, ������ � ����� ��� ������� ��������)
define process_files

ifneq (,$5)
  ifneq (,$3)
    $$(foreach file,$3,$$(eval $$(call process_files_impl,$1,process.$$(subst /,-,$$(subst ./,,$$(subst ../,,$$(patsubst %/,%,$2)))),$2/,$$(file),$4,$5)))
  endif
endif

endef

###################################################################################################
#����������� ����� ������������ ������ ������
###################################################################################################
ifeq (,$(filter $(CURRENT_PROFILE),$(AVAILABLE_PROFILES)))
  $(error Unknown profile '$(CURRENT_PROFILE)'. Use one of available profiles '$(AVAILABLE_PROFILES)')
endif

include $(PROFILE_FILE)

###################################################################################################
#����������� ����������� ������������� NaturalDocs
###################################################################################################
ifneq (,$(NATURAL_DOCS_HOME))

ifneq (,$(filter Win%,$(OS)))
  NATURAL_DOCS_HOME := $(subst /,\,$(NATURAL_DOCS_HOME))
  NATURAL_DOCS      := /bin/perl -I"/$(subst :,,$(call convert_path,$(NATURAL_DOCS_HOME)))/Modules" "$(NATURAL_DOCS_HOME)\NaturalDocs"  
else
  NATURAL_DOCS := /bin/perl -I$(call convert_path,$(NATURAL_DOCS_HOME)\Modules) $(call convert_path,$(NATURAL_DOCS_HOME)\NaturalDocs)
endif

NATURAL_DOCS_FLAGS := -s Small -q

endif

###################################################################################################
#����������� ������ ������������ ���������� � ������ ��������
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
$(foreach dir,$(strip $1),$(COMPONENT_DIR)$(dir))
endef

###################################################################################################
#��������� ����� ����������
###################################################################################################

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

#�������������� ���������� (������� ���������, ������� ��������, ������������� ���� � ������������� ����������)
define import_variables
#  $$(warning src='$1' dst='$2' path='$3')  

  $2.SOURCE_DIRS := $$($2.SOURCE_DIRS) $$($1.SOURCE_DIRS:%=$3%)
  $2.COMPONENTS  := $$($2.COMPONENTS) $$($1.COMPONENTS)  
endef

#��������������� ������ �������� - ���������� ������ ������� (��� �����������, ��� ����, ��� ���������� �� �������)
define prepare_to_import_settings

# �������� ������������ �������
ifneq (,$$(filter $1,$$($3)))
  $3 := $$(foreach imp,$$($3),$$(if $$(filter $1,$$(imp)),,$$(imp)))
endif

  $3 := $$($3) $1

#�������� ������� ����������
ifeq (,$$(paths.$1))
  ifeq (,$$(strip $$(foreach profile,$$(PROFILES),$$(paths.$1.$$(profile)))))
    $$(error Component '$1' not exported (unresolved import))
  endif
endif

#�������������� ��������� ������������
  DEPENDENCY_IMPORTS := $$($(EXPORT_VAR_PREFIX).$1.IMPORTS) $$(foreach profile,$(PROFILES),$$($(EXPORT_VAR_PREFIX).$1.$$(profile).IMPORTS))

  $$(foreach imp,$$(DEPENDENCY_IMPORTS),$$(eval $$(call prepare_to_import_settings,$$(imp),$2,$3)))
endef

#�������������� �������� (��� �����������, ��� ����)
define import_settings
#��������� ��������  
  $$(eval $$(call import_variables,$(EXPORT_VAR_PREFIX).$1,$2,$$(patsubst $(COMPONENT_DIR)%,%,$$(paths.$1))))
  $$(foreach profile,$(PROFILES),$$(eval $$(call import_variables,$(EXPORT_VAR_PREFIX).$1.$$(profile),$2,$$(patsubst $(COMPONENT_DIR)%,%,$$(paths.$1)))))
endef

#������ �������� toolset-�������� (��� ����, ��� �������)
define import_profile_settings
$$(foreach var,$$(filter $1.$2.%,$$(.VARIABLES)),$$(eval $$(var:$1.$2.%=$1.%) = $$($$(var:$1.$2.%=$1.%)) $$($$(var))))
endef

#��������� ������ ���� (��� ����)
define process_target_common
  PROCESSED_IMPORTS :=

    #���������� toolset-�������� � ������ ������ ��������
  $$(foreach profile,$$(PROFILES),$$(eval $1.IMPORTS := $$($1.IMPORTS) $$($1.$$(profile).IMPORTS)))  

    #������ ��������

  $1.PROCESSED_IMPORTS :=
 
#  $$(foreach imp,$$($1.IMPORTS),$$(eval $$(call import_settings,$$(imp),$1)))
  $$(foreach imp,$$($1.IMPORTS),$$(eval $$(call prepare_to_import_settings,$$(imp),$1,$1.PROCESSED_IMPORTS)))
  $$(foreach imp,$$($1.PROCESSED_IMPORTS),$$(eval $$(call import_settings,$$(imp),$1)))

    #���������� toolset-�������� � ������ ������ ��������
  $$(foreach profile,$$(PROFILES),$$(eval $$(call import_profile_settings,$1,$$(profile))))

  DUMP.$1:
		@echo Dump target \'$1\' settings
		@$$(foreach var,$$(sort $$(filter $1.%,$(.VARIABLES))),echo '    $$(var:$1.%=%) = $$($$(var))' && ) true

  dump: DUMP.$1

  .PHONY: DUMP.$1

#��������� ������������� ������ ��� ������� ���� �����

  $$(eval $$(call process_target.$$(strip $$($1.TYPE)),$1))
endef

#�������� ������������ ���� ���� (��� ����)
define test_target_type
  ifeq (,$$(findstring $$(strip $$($1.TYPE)),$(VALID_TARGET_TYPES)))
    $$(error Wrong target type '$$(strip $$($1.TYPE))' at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif
endef

###################################################################################################
#C����� ��� ������
###################################################################################################
#��������� �������� � lua ����������� (��� ����, ��� ��������)
define process_lua_source_dir 
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $2/*.lua)
  endif
endef

#��������� ���� lua-module (��� ����)
define process_target.lua-module
  ifeq (,$$($1.NAME))
    $$(error Lua module '$1' must have name)
  endif

  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)
  
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_lua_source_dir,$1,$$(dir))))

  $1.TARGET       := $(DIST_LUA_MODULES_DIR)/$$($1.NAME).luac
  $1.TMP_DIR      := $$(ROOT_TMP_DIR)/$1
  DIST_DIRS       := $$(DIST_DIRS) $$(dir $$($1.TARGET))  
  TARGET_BINARIES := $$(TARGET_BINARIES) $$($1.TARGET)
  TMP_DIRS        := $$(TMP_DIRS)

  $$($1.TARGET): $$($1.SOURCE_FILES)
		@echo Compile $$(notdir $$@)...
		@$(LUA_COMPILER) -o "$$@" $$($1.SOURCE_FILES)

  BUILD.$1: $$($1.DOC_TMP_DIR) $$(dir $$($1.TARGET)) $$($1.TARGET)

  .PHONY: BUILD.$1

  build: BUILD.$1  
  
endef

#��������� ���� nd-info (��� ����)
define process_target.nd-info
  ifeq (,$$($1.NAME))
    $$(error Documentation target '$1' must have name)
  endif

  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)
  
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_lua_source_dir,$1,$$(dir))))

  $1.TMP_DIR       := $$(ROOT_TMP_DIR)/$1
  $1.DOC_OUT_DIR   := $(DIST_INFO_DIR)/$$($1.NAME)
  $1.DOC_MENU_FILE := $$(if $$($1.DOC_MENU_FILE),$(COMPONENT_DIR)$$($1.DOC_MENU_FILE))
  DIST_DIRS        := $$(DIST_DIRS) $$($1.DOC_OUT_DIR)  
  TMP_DIRS         := $$(TMP_DIRS) $$($1.TMP_DIR)
  
  info: INFO.$1
  
  .PHONY: INFO.$1  
  
ifneq (,$(NATURAL_DOCS))  

  INFO.$1: $$($1.TMP_DIR)
  
ifneq (,$$($1.DOC_MENU_FILE))
  INFO.$1: $(CURDIR)/$$($1.TMP_DIR)/Menu.txt
  
  $(CURDIR)/$$($1.TMP_DIR)/Menu.txt: $$($1.DOC_MENU_FILE)
		@cp -vf $$< $$@
endif
  
  INFO.$1:
		@echo Build documentation $$($1.NAME)...
		@mkdir -p $$($1.DOC_OUT_DIR)
		@cd $(NATURAL_DOCS_CONFIG_DIR) && $(NATURAL_DOCS) $$(NATURAL_DOCS_FLAGS) $$($1.SOURCE_DIRS:%=-i $(CURDIR)/%) -o HTML $(CURDIR)/$$($1.DOC_OUT_DIR) -p $(CURDIR)/$$($1.TMP_DIR)

else

  INFO.$1:
		@echo "No NaturalDocs found (set NATURAL_DOCS_HOME variable)..."
		@exit 1

endif

endef

#����� ����� (��� ����, ��� �������, ��� �����)
define process_target_tests_dir_run_test
  RUN.$2:: $$($2.TEST_BINARIES)
		@cd $$($2.EXECUTION_DIR) && export LUA_SEARCH_PATHS=$$($1.INCLUDE_PATTERNS:%=$(CURDIR)/%) && $(CURDIR)/$(CLAUNCHER) $$($2.EXECUTION_FLAGS) $(args) $(CURDIR)/$3

endef

#��������� �������� � ����������� ���� test-suite (��� ����, ��� �������, ��� ��������)
define process_target_tests_dir
  ifneq (,$$(wildcard $3/sources.mak))
    SOURCE_FILES :=
  
    include $3/sources.mak    

    $2.SOURCE_FILES := $$(wildcard $$(SOURCE_FILES:%=$3/%))
  else
    $2.SOURCE_FILES := $$(wildcard $3/*.lua)
  endif
  
  $2.TMP_DIR             := $$(ROOT_TMP_DIR)/$1/$$(patsubst $(COMPONENT_DIR)%,%,$3)
  $2.TEST_BINARIES       := $$(filter $$(files:%=$$($2.TMP_DIR)/%.luac),$$(foreach file,$$($2.SOURCE_FILES),$$($2.TMP_DIR)/$$(notdir $$(basename $$(file)).luac)))
  $2.SOURCE_RESULT_FILES := $$(wildcard $3/*.result)
  $2.TARGET_RESULT_FILES := $$(foreach file,$$($2.SOURCE_RESULT_FILES),$$($2.TMP_DIR)/$$(notdir $$(file)))
  $2.EXECUTION_DIR       := $$(if $$($1.EXECUTION_DIR),$$(COMPONENT_DIR)$$($1.EXECUTION_DIR),$3)
  $2.EXECUTION_FLAGS     := $$(if $$($2.EXECUTION_FLAGS),$$($2.EXECUTION_FLAGS),$(DEFAULT_EXECUTION_FLAGS))
  TMP_DIRS               := $$(TMP_DIRS) $$($2.TMP_DIR)

  build: BUILD.$2
  run: RUN.$2
  check: CHECK.$2

  .PHONY: BUILD.$2 RUN.$2 CHECK.$2
  
  BUILD.$2: $$($2.TMP_DIR) $$($2.TEST_BINARIES)  

  $$($2.TMP_DIR)/%.luac: $3/%.lua
		@echo Compile $$(notdir $$<)...
		@$(LUA_COMPILER) -o "$$@" $$<

  $$(foreach file,$$($2.TEST_BINARIES),$$(eval $$(call process_target_tests_dir_run_test,$1,$2,$$(file))))

  CHECK.$2: $$($2.TMP_DIR) $$($2.TARGET_RESULT_FILES)
  
  $$($2.TMP_DIR)/%.result: $$($2.TMP_DIR)/%.luac $$(TARGET_BINARIES)
		@echo Running $$(basename $$(notdir $$<))...
		@cd $$($2.EXECUTION_DIR) && export LUA_SEARCH_PATHS=$$($1.INCLUDE_PATTERNS:%=$(CURDIR)/%) && $(CURDIR)/$(CLAUNCHER) $$($2.EXECUTION_FLAGS) $(args) $(CURDIR)/$$< > $(CURDIR)/$$@

  CHECK.$2: $$($2.TARGET_RESULT_FILES)
		@echo Checking results of module '$2'...
		@$$(call for_each_file,file,$$(notdir $$(filter $$(files:%=$$($2.TMP_DIR)/%.result),$$^)),diff --strip-trailing-cr --context=1 $3/$$$$file $$($2.TMP_DIR)/$$$$file)
endef

#��������� ���� test-suite (��� ����)
define process_target.test-suite
  $1.SOURCE_DIRS      := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)
  $1.INCLUDE_DIRS     := $$($1.INCLUDE_DIRS:%=$(COMPONENT_DIR)%) $$(DIST_LUA_MODULES_DIR)
  $1.INCLUDE_PATTERNS := $$($1.INCLUDE_DIRS:%=%/?.luac)
  
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_target_tests_dir,$1,$1.SUBDIR.$$(subst /,-,$$(subst ./,,$$(subst ../,,$$(patsubst %/,%,$$(dir))))),$$(dir))))  
endef

###################################################################################################
#������� ������
###################################################################################################
all: build check
run: build
rebuild: clean build
run: build
check: build
force:

.PHONY: build rebuild clean fullyclean run check help force dump info

#������������� ������ ����� (� ����������� �� �������)
$(foreach profile,$(PROFILES),$(eval TARGETS := $$(TARGETS) $$(TARGETS.$$(profile))))  

#��������� ����� ����������
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call test_target_type,$(target))))
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call process_target_common,$(target))))

#�������� ���������

$(sort $(DIRS)):
	@mkdir -p $@

#�������
clean:
	@$(if $(wildcard $(ROOT_TMP_DIR)),cd $(ROOT_TMP_DIR) && $(RM) -r $(TMP_CLEAN_DIRS:$(ROOT_TMP_DIR)/%=%))

fullyclean: clean
	@$(RM) -r $(DIRS)

install:
	@echo Installation not supported...
