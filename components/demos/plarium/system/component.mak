###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS

#���� �1 - System library sources
SYSTEMLIB.SOURCES.TYPE               := static-lib
SYSTEMLIB.SOURCES.NAME               := plarium.system
SYSTEMLIB.SOURCES.IMPORTS            := compile.plarium.system
SYSTEMLIB.SOURCES.SOURCE_DIRS        := sources/utility
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS  := sources/windows
SYSTEMLIB.SOURCES.unistd.SOURCE_DIRS := sources/pthread

#���� �2 - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests
SYSTEMLIB.TESTS.IMPORTS          := compile.plarium.system link.plarium.system
