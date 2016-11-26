TARGETS := NETWORK.SOURCES NETWORK.TESTS

#Цель №1 - System library sources
NETWORK.SOURCES.TYPE                 := static-lib
NETWORK.SOURCES.NAME                 := funner.network
NETWORK.SOURCES.INCLUDE_DIRS         := sources
NETWORK.SOURCES.SOURCE_DIRS          := sources/wrappers
NETWORK.SOURCES.unistd.SOURCE_DIRS   := sources/platform/unistd
NETWORK.SOURCES.win32.SOURCE_DIRS    := sources/platform/win32 sources/platform/curl
NETWORK.SOURCES.wp8.SOURCE_DIRS      := sources/platform/win32 sources/platform/curl
NETWORK.SOURCES.macosx.SOURCE_DIRS   := sources/platform/curl
NETWORK.SOURCES.iphone.SOURCE_DIRS   := sources/platform/curl
NETWORK.SOURCES.tabletos.SOURCE_DIRS := sources/platform/curl
NETWORK.SOURCES.android.SOURCE_DIRS  := sources/platform/curl
NETWORK.SOURCES.linux.SOURCE_DIRS    := sources/platform/curl
NETWORK.SOURCES.IMPORTS              := compile.network compile.system compile.common
NETWORK.SOURCES.win32.IMPORTS        := compile.extern.curl
NETWORK.SOURCES.wp8.IMPORTS          := compile.extern.curl
NETWORK.SOURCES.macosx.IMPORTS       := compile.extern.curl
NETWORK.SOURCES.iphone.IMPORTS       := compile.extern.curl
NETWORK.SOURCES.android.IMPORTS      := compile.extern.curl
NETWORK.SOURCES.tabletos.IMPORTS     := compile.extern.curl
NETWORK.SOURCES.linux.IMPORTS        := compile.extern.curl

#Цель №2 - System library tests
NETWORK.TESTS.TYPE        := test-suite
NETWORK.TESTS.SOURCE_DIRS := tests/wrappers
NETWORK.TESTS.IMPORTS     := compile.system compile.network link.network compile.common link.common.zip_file_system compile.extern.mongoose link.extern.mongoose
