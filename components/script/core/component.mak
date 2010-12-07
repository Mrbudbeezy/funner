###################################################################################################
#���������� � ���������
###################################################################################################
TARGETS := SCRIPT.CORE.SOURCES SCRIPT.CORE.TESTS SCRIPT.CORE.INFO

#���� - SCRIPT_CORE sources
SCRIPT.CORE.SOURCES.TYPE        := static-lib
SCRIPT.CORE.SOURCES.NAME        := funner.script.core
SCRIPT.CORE.SOURCES.SOURCE_DIRS := sources
SCRIPT.CORE.SOURCES.IMPORTS			:= compile.script.core compile.common

#���� - SCRIPT_CORE tests
SCRIPT.CORE.TESTS.TYPE        := test-suite
SCRIPT.CORE.TESTS.SOURCE_DIRS := tests
SCRIPT.CORE.TESTS.IMPORTS			:= compile.script.core link.script.core compile.common

#���� - ������ ������������
SCRIPT.CORE.INFO.TYPE        := doxygen-info
SCRIPT.CORE.INFO.CHM_NAME    := funner.script.core
SCRIPT.CORE.INFO.SOURCE_DIRS := include
SCRIPT.CORE.INFO.IMPORTS     := compile.script.core
