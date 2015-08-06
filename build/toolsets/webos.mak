###################################################################################################
#Build for WebOS
###################################################################################################
ifeq ($(strip $(WEBOS_GCC)),)
  $(error "Please set WEBOS_GCC variable in your environment")
endif

ifeq ($(strip $(PALMPDK)),)
  $(error "Please set PalmPDK variable in your environment")
endif

WEBOS_USER     ?= root
WEBOS_PASSWORD ?= ""
WEBOS_HOST     ?= 127.0.0.1
WEBOS_PORT     ?= 10022

###################################################################################################
#Constants
###################################################################################################
PROFILES          += unistd webos egl gles has_windows
REMOTE_DEBUG_DIR  ?= //media/internal/funner
EXE_SUFFIX        :=
DLL_SUFFIX        := .so
DLL_PREFIX        := lib
WEBOS_GCC         := $(subst \,/,$(WEBOS_GCC))
COMPILER_GCC      := $(WEBOS_GCC)/bin/arm-none-linux-gnueabi-gcc
LINKER_GCC        := $(WEBOS_GCC)/bin/arm-none-linux-gnueabi-gcc
LIB_GCC           := $(WEBOS_GCC)/bin/arm-none-linux-gnueabi-ar
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -DWEBOS -DARM "-I$(PALMPDK)/include" "-I$(PALMPDK)/include/SDK" -O2 -funroll-loops -mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
COMMON_LINK_FLAGS += "-L$(PALMPDK)/device/lib" -Wl,--allow-shlib-undefined  -O2 -lm -lpdl -lpthread -lstdc++ -lrt -Wl,-L,$(DIST_BIN_DIR)

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Override utilties calling
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-Wl,-u,$(link)),$1.stderr) && sed "/^.*warning.*lib.*needed by.*libpdl\.so.*not found.*$$/d" $1.stderr && $(RM) $1.stderr
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link.dll
-shared
endef

#Copy files to remote device (source, destination, password, port)
define ssh_copy
scp -r $(if $4,-P $4) $1 $2
endef
#"$(PALMPDK)/bin/pscp" -scp  $(if $4,-P $4) $(if $3,-pw $3,-pw "") $1 $2

###################################################################################################
#Debug on device
###################################################################################################

#Copy file to device (local files name, remote directory name)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(WEBOS_USER)@$(WEBOS_HOST),$(WEBOS_PASSWORD),$(WEBOS_PORT)) && \
$(call ssh_copy,$1,$(WEBOS_USER)@$(WEBOS_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),$(WEBOS_PASSWORD),$(WEBOS_PORT)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(WEBOS_USER)@$(WEBOS_HOST),$(WEBOS_PASSWORD),$(WEBOS_PORT))
endef

#Execute command (command, execution directory, libraries and executables additional search paths, dlls list)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(WEBOS_USER)@$(WEBOS_HOST),$(WEBOS_PASSWORD),$(WEBOS_PORT))
endef
