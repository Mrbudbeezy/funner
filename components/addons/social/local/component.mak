###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := SOCIAL.LOCAL.SOURCES SOCIAL.LOCAL.TESTS

#Target - sources
SOCIAL.LOCAL.SOURCES.TYPE        := static-lib
SOCIAL.LOCAL.SOURCES.NAME        := funner.social.local
SOCIAL.LOCAL.SOURCES.SOURCE_DIRS := sources
SOCIAL.LOCAL.SOURCES.IMPORTS			:= compile.social.core

#Target - tests
SOCIAL.LOCAL.TESTS.TYPE        := test-suite
SOCIAL.LOCAL.TESTS.SOURCE_DIRS := tests
SOCIAL.LOCAL.TESTS.IMPORTS			:= compile.social.core link.social.local
