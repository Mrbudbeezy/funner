TARGETS := EXTERN.CURL.SOURCES EXTERN.CURL.TESTS

EXTERN.CURL.SOURCES.TYPE                     := static-lib
EXTERN.CURL.SOURCES.NAME                     := funner.extern.curl
EXTERN.CURL.SOURCES.INCLUDE_DIRS             := include sources
EXTERN.CURL.SOURCES.SOURCE_DIRS              := sources sources/vtls
EXTERN.CURL.SOURCES.COMPILER_DEFINES         := CURL_STATICLIB BUILDING_LIBCURL $(if $(filter emscripten,$(PROFILES)),,USE_SSLEAY)
EXTERN.CURL.SOURCES.cocoa.COMPILER_DEFINES   := HAVE_CONFIG_H
EXTERN.CURL.SOURCES.macx86.INCLUDE_DIRS      := addons/macosx-i386
EXTERN.CURL.SOURCES.macx86-64.INCLUDE_DIRS   := addons/macosx-x86-64
EXTERN.CURL.SOURCES.IMPORTS                  := compile.extern.zlib compile.extern.openssl

EXTERN.CURL.TESTS.TYPE                     := test-suite
EXTERN.CURL.TESTS.SOURCE_DIRS              := tests
EXTERN.CURL.TESTS.INCLUDE_DIRS             := sources
EXTERN.CURL.TESTS.INCLUDE_FILES            := first.c testutil.c
EXTERN.CURL.TESTS.cocoa.COMPILER_DEFINES   := HAVE_CONFIG_H
EXTERN.CURL.TESTS.macx86.INCLUDE_DIRS      := addons/macosx-i386
EXTERN.CURL.TESTS.macx86-64.INCLUDE_DIRS   := addons/macosx-x86-64
EXTERN.CURL.TESTS.IMPORTS                  := compile.extern.curl link.extern.curl
EXTERN.CURL.TESTS.vcx86-64.COMPILER_CFLAGS := -wd4244
