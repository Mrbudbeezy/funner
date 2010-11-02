###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.LUALIB EXTERN.LUA_UTILS EXTERN.LUA_TESTS

#���� �1 - LUALIB
EXTERN.LUALIB.TYPE                 := static-lib        #��� ����
EXTERN.LUALIB.NAME                 := funner.extern.lua        #��� ��������� �����
EXTERN.LUALIB.INCLUDE_DIRS         := include				   #������������� ���� � ��������� c ������������� �������
EXTERN.LUALIB.SOURCE_DIRS          := sources           #������������� ���� � ��������� c ��������� �������
EXTERN.LUALIB.LIB_DIRS             :=                   #������������� ���� � ��������� � ������������
EXTERN.LUALIB.LIBS                 :=                   #������ ������������ ��� �������� ���������
#EXTERN.LUALIB.COMPILER_DEFINES     := __STRICT_ANSI__   #����������� ������
EXTERN.LUALIB.msvc.COMPILER_CFLAGS := -TP               #����� ������ (������� �� ������������)
EXTERN.LUALIB.g++.COMPILER_CFLAGS  := -x c++
EXTERN.LUALIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h

#���� �2 - LUA_UTILS
EXTERN.LUA_UTILS.TYPE                 := test-suite     	 #��� ����
EXTERN.LUA_UTILS.INCLUDE_DIRS         := include	sources  #������������� ���� � ��������� c ������������� �������
EXTERN.LUA_UTILS.SOURCE_DIRS          := utils          	 #������������� ���� � ��������� c ��������� �������
EXTERN.LUA_UTILS.LIB_DIRS             :=                  #������������� ���� � ��������� � ������������
EXTERN.LUA_UTILS.LIBS                 := funner.extern.lua       #������ ������������ ��� �������� ���������
#EXTERN.LUA_UTILS.COMPILER_DEFINES     := __STRICT_ANSI__  #����������� ������
EXTERN.LUA_UTILS.msvc.COMPILER_CFLAGS := -TP              #����� ������ (������� �� ������������)
EXTERN.LUA_UTILS.g++.COMPILER_CFLAGS  := -x c++
EXTERN.LUA_UTILS.TARGET_DIR            = $(DIST_BIN_DIR)
EXTERN.LUA_UTILS.USED_APPLICATIONS    := lua

#���� �3 - LUA_TESTS
EXTERN.LUA_TESTS.TYPE         := test-suite
EXTERN.LUA_TESTS.SOURCE_DIRS  := tests
