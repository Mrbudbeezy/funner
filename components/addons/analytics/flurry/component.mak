###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := ANALYTICS.FLURRY.SOURCES ANALYTICS.FLURRY.TESTS  
TARGETS.android += ANALYTICS.FLURRY_ANDROID.JAVA

#���� sources
ANALYTICS.FLURRY.SOURCES.TYPE                := static-lib
ANALYTICS.FLURRY.SOURCES.NAME                := funner.analytics.flurry
ANALYTICS.FLURRY.SOURCES.INCLUDE_DIRS        := include sources/platform
ANALYTICS.FLURRY.SOURCES.SOURCE_DIRS         := sources/shared sources/platform/default
ANALYTICS.FLURRY.SOURCES.iphone.SOURCE_DIRS  := sources/platform/ios
ANALYTICS.FLURRY.SOURCES.android.SOURCE_DIRS := sources/platform/android
ANALYTICS.FLURRY.SOURCES.IMPORTS             := compile.common
ANALYTICS.FLURRY.SOURCES.iphone.IMPORTS      := compile.extern.flurry compile.system
ANALYTICS.FLURRY.SOURCES.android.IMPORTS     := compile.system

#���� - java
ANALYTICS.FLURRY_ANDROID.JAVA.TYPE        := android-jar
ANALYTICS.FLURRY_ANDROID.JAVA.NAME        := funner.analytics.flurry
ANALYTICS.FLURRY_ANDROID.JAVA.SOURCE_DIRS := java_sources
ANALYTICS.FLURRY_ANDROID.JAVA.JARS        := funner.application
ANALYTICS.FLURRY_ANDROID.JAVA.IMPORTS     := compile.extern.flurry

#���� - CommonLib tests
ANALYTICS.FLURRY.TESTS.TYPE           := test-suite
ANALYTICS.FLURRY.TESTS.SOURCE_DIRS    := tests
ANALYTICS.FLURRY.TESTS.IMPORTS        := compile.analytics.flurry link.analytics.flurry
ANALYTICS.FLURRY.TESTS.iphone.IMPORTS := link.extern.flurry
