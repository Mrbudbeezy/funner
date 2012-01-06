###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SOCIAL.CORE.SOURCES SOCIAL.CORE.TESTS SOCIAL.CORE.INFO

#���� - sources
SOCIAL.CORE.SOURCES.TYPE        := static-lib
SOCIAL.CORE.SOURCES.NAME        := funner.social.core
SOCIAL.CORE.SOURCES.SOURCE_DIRS := sources
SOCIAL.CORE.SOURCES.IMPORTS			:= compile.social.core

#���� - tests
SOCIAL.CORE.TESTS.TYPE        := test-suite
SOCIAL.CORE.TESTS.SOURCE_DIRS := tests
SOCIAL.CORE.TESTS.IMPORTS			:= compile.social.core link.social.core

#���� - ������ ������������
SOCIAL.CORE.INFO.TYPE        := doxygen-info
SOCIAL.CORE.INFO.CHM_NAME    := funner.script.core
SOCIAL.CORE.INFO.SOURCE_DIRS := include
SOCIAL.CORE.INFO.IMPORTS     := compile.script.core
