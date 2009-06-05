###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := COMMON.SOURCES COMMON.WXF_PARSER COMMON.XML_PARSER COMMON.ZIP_FILE_SYSTEM COMMON.CONFIGURATOR COMMON.AES COMMON.TESTS \
  COMMON.UTILS.FILE_CRYPTER

#���� - CommonLib sources
COMMON.SOURCES.TYPE             := static-lib
COMMON.SOURCES.NAME             := funner.common
COMMON.SOURCES.INCLUDE_DIRS     := include ../xtl/include ../../extern/pcre/include sources
COMMON.SOURCES.SOURCE_DIRS      := sources/file_system/core sources/streams sources/hash sources/crypto/core sources/memory \
                                   sources/parselib/tree sources/parselib/manager sources/strlib sources/utils sources/log \
                                   sources/var_registry sources/platform/default
COMMON.SOURCES.LIB_DIRS           :=  
COMMON.SOURCES.LIBS               := 
COMMON.SOURCES.COMPILER_CFLAGS    :=
COMMON.SOURCES.COMPILER_DEFINES   := PCRE_STATIC
COMMON.SOURCES.unistd.SOURCE_DIRS := sources/platform/unistd
COMMON.SOURCES.macosx.SOURCE_DIRS := sources/platform/macosx
COMMON.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32

#���� - WxfParser
COMMON.WXF_PARSER.TYPE             := static-lib
COMMON.WXF_PARSER.NAME             := funner.common.parsers.wxf
COMMON.WXF_PARSER.INCLUDE_DIRS     := include ../xtl/include
COMMON.WXF_PARSER.SOURCE_DIRS      := sources/parselib/parsers/wxf
COMMON.WXF_PARSER.LIB_DIRS         :=  
COMMON.WXF_PARSER.LIBS             := 
COMMON.WXF_PARSER.COMPILER_CFLAGS  :=
COMMON.WXF_PARSER.COMPILER_DEFINES :=

#���� - XmlParser
COMMON.XML_PARSER.TYPE             := static-lib
COMMON.XML_PARSER.NAME             := funner.common.parsers.xml
COMMON.XML_PARSER.INCLUDE_DIRS     := include ../xtl/include
COMMON.XML_PARSER.SOURCE_DIRS      := sources/parselib/parsers/xml
COMMON.XML_PARSER.LIB_DIRS         :=  
COMMON.XML_PARSER.LIBS             := 
COMMON.XML_PARSER.COMPILER_CFLAGS  :=
COMMON.XML_PARSER.COMPILER_DEFINES :=

#���� - CommonLib zip file system sources
COMMON.ZIP_FILE_SYSTEM.TYPE                    := static-lib
COMMON.ZIP_FILE_SYSTEM.NAME                    := funner.common.zip_file_system
COMMON.ZIP_FILE_SYSTEM.INCLUDE_DIRS            := include ../xtl/include ../../extern/zzip/include
COMMON.ZIP_FILE_SYSTEM.SOURCE_DIRS             := sources/file_system/zip
COMMON.ZIP_FILE_SYSTEM.LIB_DIRS                :=
COMMON.ZIP_FILE_SYSTEM.LIBS                    :=
COMMON.ZIP_FILE_SYSTEM.COMPILER_CFLAGS         :=
COMMON.ZIP_FILE_SYSTEM.COMPILER_DEFINES        :=
COMMON.ZIP_FILE_SYSTEM.macosx.COMPILER_DEFINES := ZZIP_1_H

#���� - CommonLib zip file system sources
COMMON.CONFIGURATOR.TYPE             := static-lib
COMMON.CONFIGURATOR.NAME             := funner.common.configurator
COMMON.CONFIGURATOR.INCLUDE_DIRS     := 
COMMON.CONFIGURATOR.SOURCE_DIRS      := sources/var_registry/configurator
COMMON.CONFIGURATOR.LIB_DIRS         :=
COMMON.CONFIGURATOR.LIBS             :=
COMMON.CONFIGURATOR.COMPILER_CFLAGS  :=
COMMON.CONFIGURATOR.COMPILER_DEFINES :=
COMMON.CONFIGURATOR.IMPORTS          := compile.static.mak

#���� - CommonLib zip file system sources
COMMON.AES.TYPE             := static-lib
COMMON.AES.NAME             := funner.common.aes
COMMON.AES.INCLUDE_DIRS     := 
COMMON.AES.SOURCE_DIRS      := sources/crypto/aes
COMMON.AES.LIB_DIRS         :=
COMMON.AES.LIBS             :=
COMMON.AES.COMPILER_CFLAGS  :=
COMMON.AES.COMPILER_DEFINES :=
COMMON.AES.IMPORTS          := compile.static.mak

#���� - CommonLib tests
COMMON.TESTS.TYPE             := test-suite
COMMON.TESTS.INCLUDE_DIRS     :=
COMMON.TESTS.SOURCE_DIRS      := tests/file_system tests/streams tests/hash tests/strlib tests/utils \
                                 tests/memory tests/log tests/parselib tests/crypto
COMMON.TESTS.EXECUTION_DIR    :=
COMMON.TESTS.LIB_DIRS         :=
COMMON.TESTS.LIBS             :=
COMMON.TESTS.LINK_INCLUDES    :=
COMMON.TESTS.COMPILER_CFLAGS  :=
COMMON.TESTS.COMPILER_DEFINES :=
COMMON.TESTS.IMPORTS          := compile.static.mak link.static.mak zip_file_system.link.static.mak \
                                 wxf.link.static.mak xml.link.static.mak configurator.link.static.mak \
                                 default_console_handler.link.static.mak aes.link.static.mak

#���� - CommonLib crypter
COMMON.UTILS.FILE_CRYPTER.TYPE             := application
COMMON.UTILS.FILE_CRYPTER.NAME             := file-crypter
COMMON.UTILS.FILE_CRYPTER.INCLUDE_DIRS     :=
COMMON.UTILS.FILE_CRYPTER.SOURCE_DIRS      := utils/file_crypter
COMMON.UTILS.FILE_CRYPTER.EXECUTION_DIR    :=
COMMON.UTILS.FILE_CRYPTER.LIB_DIRS         :=
COMMON.UTILS.FILE_CRYPTER.LIBS             :=
COMMON.UTILS.FILE_CRYPTER.LINK_INCLUDES    :=
COMMON.UTILS.FILE_CRYPTER.COMPILER_CFLAGS  :=
COMMON.UTILS.FILE_CRYPTER.COMPILER_DEFINES :=
COMMON.UTILS.FILE_CRYPTER.IMPORTS          := compile.static.mak link.static.mak aes.link.static.mak
