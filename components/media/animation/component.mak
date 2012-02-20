###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := MEDIA.ANIMATION.SOURCES MEDIA.ANIMATION.XANIM.SOURCES MEDIA.ANIMATION.BINANIM.SOURCES \
           MEDIA.ANIMATION.TESTS MEDIA.ANIMATION.XANIM.TESTS MEDIA.ANIMATION.BINANIM.TESTS MEDIA.ANIMATION.INFO

#���� - sources
MEDIA.ANIMATION.SOURCES.TYPE        := static-lib
MEDIA.ANIMATION.SOURCES.NAME        := funner.media.animation
MEDIA.ANIMATION.SOURCES.SOURCE_DIRS := sources/core
MEDIA.ANIMATION.SOURCES.IMPORTS     := compile.media.animation compile.media.shared compile.math.vecmath

#���� - xanim serializer sources
MEDIA.ANIMATION.XANIM.SOURCES.TYPE        := static-lib
MEDIA.ANIMATION.XANIM.SOURCES.NAME        := funner.media.animation.xanim
MEDIA.ANIMATION.XANIM.SOURCES.SOURCE_DIRS := sources/xanim
MEDIA.ANIMATION.XANIM.SOURCES.IMPORTS     := compile.media.animation compile.math.curves 

#���� - binanim serializer sources
MEDIA.ANIMATION.BINANIM.SOURCES.TYPE        := static-lib
MEDIA.ANIMATION.BINANIM.SOURCES.NAME        := funner.media.animation.binanim
MEDIA.ANIMATION.BINANIM.SOURCES.SOURCE_DIRS := sources/binanim
MEDIA.ANIMATION.BINANIM.SOURCES.IMPORTS     := compile.media.animation compile.math.curves 

#���� - tests
MEDIA.ANIMATION.TESTS.TYPE        := test-suite
MEDIA.ANIMATION.TESTS.SOURCE_DIRS := tests/core
MEDIA.ANIMATION.TESTS.IMPORTS     := compile.media.animation link.media.animation compile.math.vecmath compile.math.curves

#���� - xanim serializer tests
MEDIA.ANIMATION.XANIM.TESTS.TYPE        := test-suite
MEDIA.ANIMATION.XANIM.TESTS.SOURCE_DIRS := tests/xanim
MEDIA.ANIMATION.XANIM.TESTS.IMPORTS     := compile.media.animation compile.math.curves link.media.animation.xanim

#���� - binanim serializer tests
MEDIA.ANIMATION.BINANIM.TESTS.TYPE        := test-suite
MEDIA.ANIMATION.BINANIM.TESTS.SOURCE_DIRS := tests/binanim
MEDIA.ANIMATION.BINANIM.TESTS.IMPORTS     := compile.media.animation compile.math.curves link.media.animation.binanim

#���� - ������ ������������
MEDIA.ANIMATION.INFO.TYPE        := doxygen-info
MEDIA.ANIMATION.INFO.CHM_NAME    := funner.media.animation
MEDIA.ANIMATION.INFO.SOURCE_DIRS := include
MEDIA.ANIMATION.INFO.IMPORTS     := compile.media.animation
