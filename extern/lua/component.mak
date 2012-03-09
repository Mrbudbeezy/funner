###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.LUALIB

ifeq (,$(filter wince,$(PROFILES)))
TARGETS += EXTERN.LUA_UTILS EXTERN.LUA_TESTS
endif

#���� �1 - LUALIB
EXTERN.LUALIB.TYPE                 := static-lib        #��� ����
EXTERN.LUALIB.NAME                 := funner.extern.lua        #��� ��������� �����
EXTERN.LUALIB.INCLUDE_DIRS         := include				   #������������� ���� � ��������� c ������������� �������
EXTERN.LUALIB.SOURCE_DIRS          := sources           #������������� ���� � ��������� c ��������� �������
EXTERN.LUALIB.msvc.COMPILER_CFLAGS := -TP               #����� ������ (������� �� ������������)
EXTERN.LUALIB.g++.COMPILER_CFLAGS  := -x c++
EXTERN.LUALIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h
EXTERN.LUALIB.wince.COMPILER_CFLAGS   := -FIwince_wrapper.h -DLUA_ANSI

#���� �2 - LUA_UTILS
EXTERN.LUA_UTILS.TYPE                   := test-suite     	 #��� ����
EXTERN.LUA_UTILS.INCLUDE_DIRS           := include	sources  #������������� ���� � ��������� c ������������� �������
EXTERN.LUA_UTILS.SOURCE_DIRS            := utils          	 #������������� ���� � ��������� c ��������� �������
EXTERN.LUA_UTILS.LIBS                   := funner.extern.lua       #������ ������������ ��� �������� ���������
EXTERN.LUA_UTILS.TARGET_DIR              = $(DIST_BIN_DIR)
EXTERN.LUA_UTILS.msvc.COMPILER_CFLAGS   := -TP              #����� ������ (������� �� ������������)
EXTERN.LUA_UTILS.g++.COMPILER_CFLAGS    := -x c++
EXTERN.LUA_UTILS.bada_simulator.IMPORTS := link.extern.bada_addons

#���� �3 - LUA_TESTS
EXTERN.LUA_TESTS.TYPE              := test-suite
EXTERN.LUA_TESTS.SOURCE_DIRS       := tests
EXTERN.LUA_TESTS.USED_APPLICATIONS := lua