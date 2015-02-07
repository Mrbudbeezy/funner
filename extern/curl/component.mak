TARGETS := EXTERN.CURL.SOURCES EXTERN.CURL.TESTS

EXTERN.CURL.SOURCES.TYPE                     := static-lib
EXTERN.CURL.SOURCES.NAME                     := funner.extern.curl
EXTERN.CURL.SOURCES.INCLUDE_DIRS             := include
EXTERN.CURL.SOURCES.SOURCE_DIRS              := sources
EXTERN.CURL.SOURCES.COMPILER_DEFINES         := CURL_STATICLIB BUILDING_LIBCURL $(if $(filter emscripten,$(PROFILES)),,USE_SSLEAY)
EXTERN.CURL.SOURCES.msvc.COMPILER_CFLAGS     := -wd4005 -wd4244
EXTERN.CURL.SOURCES.vcx86-64.COMPILER_CFLAGS := -wd4267
EXTERN.CURL.SOURCES.iphone.COMPILER_CFLAGS   := -Wno-uninitialized
EXTERN.CURL.SOURCES.iphone.COMPILER_DEFINES  := __IPHONE__
EXTERN.CURL.SOURCES.IMPORTS                  := compile.extern.zlib compile.extern.openssl
EXTERN.CURL.SOURCES.clang.COMPILER_CFLAGS    += -w

EXTERN.CURL.TESTS.TYPE                     := test-suite
EXTERN.CURL.TESTS.SOURCE_DIRS              := tests
EXTERN.CURL.TESTS.INCLUDE_DIRS             := sources
EXTERN.CURL.TESTS.INCLUDE_FILES            := first.c testutil.c
EXTERN.CURL.TESTS.IMPORTS                  := compile.extern.curl link.extern.curl
EXTERN.CURL.TESTS.msvc.COMPILER_CFLAGS     := -wd4700 -wd4005
EXTERN.CURL.TESTS.vcx86-64.COMPILER_CFLAGS := -wd4267
EXTERN.CURL.TESTS.g++.COMPILER_CFLAGS      := --no-warn
