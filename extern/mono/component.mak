###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.MONO_LIB
TARGETS += EXTERN.MONO_CS_INVOKE EXTERN.MONO_TESTS

#���� �1 - MONOLIB
EXTERN.MONO_LIB.TYPE                 := static-lib               #��� ����
EXTERN.MONO_LIB.NAME                 := funner.extern.mono       #��� ��������� �����
EXTERN.MONO_LIB.INCLUDE_DIRS         := sources                  #������������� ���� � ��������� c ������������� �������
EXTERN.MONO_LIB.SOURCE_DIRS          := sources                  #������������� ���� � ��������� c ��������� �������
EXTERN.MONO_LIB.LIB_DIRS             :=                          #������������� ���� � ��������� � ������������
EXTERN.MONO_LIB.LIBS                 :=                          #������ ������������ ��� �������� ���������
#EXTERN.MONOLIB.msvc.COMPILER_CFLAGS := -TP                      #����� ������ (������� �� ������������)
#EXTERN.MONOLIB.g++.COMPILER_CFLAGS  := -x c++
#EXTERN.MONOLIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h
#EXTERN.MONOLIB.wince.COMPILER_CFLAGS   := -FIwince_wrapper.h -DLUA_ANSI

#���� �2 - MONO_CS_INVOKE
EXTERN.MONO_CS_INVOKE.TYPE              := cs-dynamic-lib
EXTERN.MONO_CS_INVOKE.NAME              := funner.extern.mono-test-invoke
EXTERN.MONO_CS_INVOKE.SOURCE_DIRS       := tests/invoke_assembly

#���� �3 - MONO_TESTS
EXTERN.MONO_TESTS.TYPE              := test-suite
EXTERN.MONO_TESTS.SOURCE_DIRS       := tests
EXTERN.MONO_TESTS.USED_APPLICATIONS := mono
EXTERN.MONO_TESTS.INCLUDE_DIRS      := sources
EXTERN.MONO_TESTS.LIBS              := funner.extern.mono