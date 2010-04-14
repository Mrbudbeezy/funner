PROFILES              += bada_device
BADA_TOOLCHAIN        := ARM
BADA_TOOLCHAIN_PREFIX := arm-samsung-nucleuseabi-
COMMON_CFLAGS         += -DBADA_DEVICE -Wno-psabi -Wno-uninitialized
COMMON_LINK_FLAGS     += #-nostdlib
DLL_LIB_SUFFIX        := .so
DLL_PREFIX            := lib
EXE_SUFFIX            :=

include $(TOOLSETS_DIR)/bada.mak

###################################################################################################
#�������� 䠩��� (��� ��室���� 䠩��, ᯨ᮪ 䠩���, ᯨ᮪ ��⠫���� � ����᪨�� ������⥪���,
#ᯨ᮪ ������砥��� ᨬ����� ��������, 䫠�� ��������)
###################################################################################################

define tools.link.dll
-shared -Wl,-undefined -Wl,error
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR))
endef
