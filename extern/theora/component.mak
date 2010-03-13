###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := EXTERN.THEORA.SOURCES EXTERN.THEORA.TESTS

#���� - THEORA sources
EXTERN.THEORA.SOURCES.TYPE                    := static-lib
EXTERN.THEORA.SOURCES.NAME                    := funner.extern.theora
EXTERN.THEORA.SOURCES.SOURCE_DIRS             := sources sources/x86
EXTERN.THEORA.SOURCES.IMPORTS                 := compile.extern.theora
EXTERN.THEORA.SOURCES.g++.COMPILER_CFLAGS     := --no-warn
EXTERN.THEORA.SOURCES.macosx.COMPILER_DEFINES := OC_X86_ASM
EXTERN.THEORA.SOURCES.win32.COMPILER_DEFINES  := OC_X86_ASM

#���� - THEORA tests
EXTERN.THEORA.TESTS.TYPE                := test-suite
EXTERN.THEORA.TESTS.SOURCE_DIRS         := tests
EXTERN.THEORA.TESTS.IMPORTS             := compile.extern.theora link.extern.theora
EXTERN.THEORA.TESTS.g++.COMPILER_CFLAGS := --no-warn
