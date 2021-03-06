###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.GEOMETRY.SOURCES MEDIA.GEOMETRY.XMESH MEDIA.GEOMETRY.BINMESH MEDIA.GEOMETRY.TESTS \
           MEDIA.GEOMETRY.XMESH.TESTS MEDIA.GEOMETRY.BINMESH.TESTS MEDIA.GEOMETRY.UTILS MEDIA.GEOMETRY.INFO

#Цель - sources
MEDIA.GEOMETRY.SOURCES.TYPE             := static-lib
MEDIA.GEOMETRY.SOURCES.NAME             := funner.media.geometry
MEDIA.GEOMETRY.SOURCES.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.GEOMETRY.SOURCES.LIB_DIRS         :=
MEDIA.GEOMETRY.SOURCES.LIBS             :=
MEDIA.GEOMETRY.SOURCES.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.SOURCES.COMPILER_DEFINES :=
MEDIA.GEOMETRY.SOURCES.IMPORTS          := compile.media.geometry compile.media.shared

#Цель - xmesh serializer sources
MEDIA.GEOMETRY.XMESH.TYPE             := static-lib
MEDIA.GEOMETRY.XMESH.NAME             := funner.media.geometry.xmesh
MEDIA.GEOMETRY.XMESH.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.XMESH.SOURCE_DIRS      := sources/xmesh
MEDIA.GEOMETRY.XMESH.LIB_DIRS         :=
MEDIA.GEOMETRY.XMESH.LIBS             :=
MEDIA.GEOMETRY.XMESH.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.XMESH.COMPILER_DEFINES :=
MEDIA.GEOMETRY.XMESH.IMPORTS          := compile.media.geometry

#Цель - binmesh serializer sources
MEDIA.GEOMETRY.BINMESH.TYPE                   := static-lib
MEDIA.GEOMETRY.BINMESH.NAME                   := funner.media.geometry.binmesh
MEDIA.GEOMETRY.BINMESH.SOURCE_DIRS            := sources/binmesh
MEDIA.GEOMETRY.BINMESH.ubuntu.COMPILER_CFLAGS := -Wno-multichar
MEDIA.GEOMETRY.BINMESH.IMPORTS                := compile.media.geometry

#Цель - tests
MEDIA.GEOMETRY.TESTS.TYPE             := test-suite
MEDIA.GEOMETRY.TESTS.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.TESTS.SOURCE_DIRS      := tests/core
MEDIA.GEOMETRY.TESTS.LIB_DIRS         :=
MEDIA.GEOMETRY.TESTS.LIBS             :=
MEDIA.GEOMETRY.TESTS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.TESTS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.TESTS.IMPORTS          := compile.media.geometry link.media.geometry

#Цель - xmesh serializer tests
MEDIA.GEOMETRY.XMESH.TESTS.TYPE             := test-suite
MEDIA.GEOMETRY.XMESH.TESTS.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.XMESH.TESTS.SOURCE_DIRS      := tests/xmesh
MEDIA.GEOMETRY.XMESH.TESTS.LIB_DIRS         :=
MEDIA.GEOMETRY.XMESH.TESTS.LIBS             :=
MEDIA.GEOMETRY.XMESH.TESTS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.XMESH.TESTS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.XMESH.TESTS.IMPORTS          := compile.media.geometry link.media.geometry.xmesh

#Цель - binmesh serializer tests
MEDIA.GEOMETRY.BINMESH.TESTS.TYPE             := test-suite
MEDIA.GEOMETRY.BINMESH.TESTS.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.BINMESH.TESTS.SOURCE_DIRS      := tests/binmesh
MEDIA.GEOMETRY.BINMESH.TESTS.LIB_DIRS         :=
MEDIA.GEOMETRY.BINMESH.TESTS.LIBS             :=
MEDIA.GEOMETRY.BINMESH.TESTS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.BINMESH.TESTS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.BINMESH.TESTS.IMPORTS          := compile.media.geometry link.media.geometry.binmesh

#Цель - utils
MEDIA.GEOMETRY.UTILS.TYPE             := application
MEDIA.GEOMETRY.UTILS.NAME             := mesh-converter
MEDIA.GEOMETRY.UTILS.SOURCE_DIRS      := utils
MEDIA.GEOMETRY.UTILS.LIB_DIRS         :=
MEDIA.GEOMETRY.UTILS.LIBS             :=
MEDIA.GEOMETRY.UTILS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.UTILS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.UTILS.IMPORTS          := compile.media.geometry link.media.geometry.xmesh link.media.geometry.binmesh

#Цель - сборка документации
MEDIA.GEOMETRY.INFO.TYPE        := doxygen-info
MEDIA.GEOMETRY.INFO.CHM_NAME    := funner.media.geometry
MEDIA.GEOMETRY.INFO.SOURCE_DIRS := include
MEDIA.GEOMETRY.INFO.IMPORTS     := compile.media.geometry
