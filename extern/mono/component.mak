###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.MONOLIB

ifeq (,$(filter wince,$(PROFILES)))
TARGETS += EXTERN.MONO_TESTS
endif

#���� �1 - MONOLIB
EXTERN.MONOLIB.TYPE                 := static-lib        				#��� ����
EXTERN.MONOLIB.NAME                 := funner.extern.mono       #��� ��������� �����
EXTERN.MONOLIB.INCLUDE_DIRS         := include                  #������������� ���� � ��������� c ������������� �������
EXTERN.MONOLIB.SOURCE_DIRS          := sources           			  #������������� ���� � ��������� c ��������� �������
EXTERN.MONOLIB.LIB_DIRS             :=                          #������������� ���� � ��������� � ������������
EXTERN.MONOLIB.LIBS                 :=                   				#������ ������������ ��� �������� ���������
EXTERN.MONOLIB.msvc.COMPILER_CFLAGS := -TP               				#����� ������ (������� �� ������������)
EXTERN.MONOLIB.g++.COMPILER_CFLAGS  := -x c++
EXTERN.MONOLIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h
EXTERN.MONOLIB.wince.COMPILER_CFLAGS   := -FIwince_wrapper.h -DLUA_ANSI

#���� �2 - MONO_TESTS
EXTERN.MONO_TESTS.TYPE              := test-suite
EXTERN.MONO_TESTS.SOURCE_DIRS       := tests
EXTERN.MONO_TESTS.USED_APPLICATIONS := mono