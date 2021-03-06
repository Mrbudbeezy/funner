###################################################################################################
#Build for BeagleBoard
###################################################################################################
ifeq ($(strip $(BEAGLEBOARD_SDK)),)
  $(error "Please set BEAGLEBOARD_SDK variable in your environment")
endif

ifeq ($(strip $(BEAGLE_BOARD_USER)),)
  $(error "Please set BEAGLE_BOARD_USER variable in your environment")
endif

ifeq ($(strip $(BEAGLE_BOARD_PASSWORD)),)
  $(error "Please set BEAGLE_BOARD_PASSWORD variable in your environment")
endif

ifeq ($(strip $(BEAGLE_BOARD_HOST)),)
  $(error "Please set BEAGLE_BOARD_HOST variable in your environment")
endif

###################################################################################################
#Constants
###################################################################################################
PROFILES          += unistd beagleboard x11 linux egl gles has_windows
REMOTE_DEBUG_DIR  ?= //work/funner
EXE_SUFFIX        :=
DLL_SUFFIX        := .so
DLL_PREFIX        := lib
BEAGLEBOARD_SDK   := $(subst \,/,$(BEAGLEBOARD_SDK))
COMPILER_GCC      := $(BEAGLEBOARD_SDK)/bin/arm-none-linux-gnueabi-gcc
LINKER_GCC        := $(BEAGLEBOARD_SDK)/bin/arm-none-linux-gnueabi-g++
LIB_GCC           := $(BEAGLEBOARD_SDK)/bin/arm-none-linux-gnueabi-ar
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -DBEAGLEBOARD -DARM -DARM7 -Wno-uninitialized -Wno-psabi -fPIC -O2 -funroll-loops -march=armv7-a
COMMON_LINK_FLAGS += -fPIC -O2 -lm -lpthread -lstdc++ -lrt -Wl,-L,$(DIST_BIN_DIR) -funroll-loops -Wl,--no-undefined -march=armv7-a -rdynamic

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Override utilities calling
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9)
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link.dll
-shared
endef

###################################################################################################
#Debug on device
###################################################################################################

#Copy file to device (local files name, remote directory name)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST),$(BEAGLE_BOARD_PASSWORD)) && \
$(call ssh_copy,$1,$(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),$(BEAGLE_BOARD_PASSWORD)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST),$(BEAGLE_BOARD_PASSWORD))
endef

#Execute command (command, launch directory, additional paths for libraries and executables, dlls list)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export DISPLAY=$(BEAGLE_BOARD_DISPLAY) PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST),$(BEAGLE_BOARD_PASSWORD))
endef
