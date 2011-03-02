###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := MEDIA.PHYSICS.CORE.SOURCES MEDIA.PHYSICS.CORE.TESTS MEDIA.PHYSICS.INFO

#���� - core sources
MEDIA.PHYSICS.CORE.SOURCES.TYPE        := static-lib
MEDIA.PHYSICS.CORE.SOURCES.NAME        := funner.media.physics
MEDIA.PHYSICS.CORE.SOURCES.SOURCE_DIRS := sources/core
MEDIA.PHYSICS.CORE.SOURCES.IMPORTS     := compile.media.physics compile.media.shared

#���� - core tests
MEDIA.PHYSICS.CORE.TESTS.TYPE        := test-suite
MEDIA.PHYSICS.CORE.TESTS.SOURCE_DIRS := tests/core
MEDIA.PHYSICS.CORE.TESTS.IMPORTS     := compile.media.physics link.media.physics

#���� - ������ ������������
MEDIA.PHYSICS.INFO.TYPE        := doxygen-info
MEDIA.PHYSICS.INFO.CHM_NAME    := funner.media.physics
MEDIA.PHYSICS.INFO.SOURCE_DIRS := include
MEDIA.PHYSICS.INFO.IMPORTS     := compile.media.physics
