###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := MEDIA.SOUND.SOURCES MEDIA.SOUND.DEFAULT_DECODERS MEDIA.SOUND.SNDDECL MEDIA.SOUND.TESTS MEDIA.SOUND.INFO

#���� �1 - Sound sources
MEDIA.SOUND.SOURCES.TYPE             := static-lib
MEDIA.SOUND.SOURCES.NAME             := funner.media.sound
MEDIA.SOUND.SOURCES.INCLUDE_DIRS     := 
MEDIA.SOUND.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.SOUND.SOURCES.LIB_DIRS         :=  
MEDIA.SOUND.SOURCES.LIBS             := 
MEDIA.SOUND.SOURCES.COMPILER_CFLAGS  := 
MEDIA.SOUND.SOURCES.COMPILER_DEFINES := 
MEDIA.SOUND.SOURCES.IMPORTS          := compile.media.sound compile.media.shared

#���� �2 - Sound sources
MEDIA.SOUND.DEFAULT_DECODERS.TYPE             := static-lib
MEDIA.SOUND.DEFAULT_DECODERS.NAME             := funner.media.sound.default_decoders
MEDIA.SOUND.DEFAULT_DECODERS.INCLUDE_DIRS     := ../../../extern/vorbisfile/include ../../../extern/ogg/include
MEDIA.SOUND.DEFAULT_DECODERS.SOURCE_DIRS      := sources/default_decoders
MEDIA.SOUND.DEFAULT_DECODERS.LIB_DIRS         :=  
MEDIA.SOUND.DEFAULT_DECODERS.LIBS             := 
MEDIA.SOUND.DEFAULT_DECODERS.COMPILER_DEFINES := 
MEDIA.SOUND.DEFAULT_DECODERS.IMPORTS          := compile.media.sound
MEDIA.SOUND.DEFAULT_DECODERS.msvc.COMPILER_CFLAGS  := -wd4244

#���� �3 - Sound sources
MEDIA.SOUND.SNDDECL.TYPE             := static-lib
MEDIA.SOUND.SNDDECL.NAME             := funner.media.sound.snddecl
MEDIA.SOUND.SNDDECL.INCLUDE_DIRS     := 
MEDIA.SOUND.SNDDECL.SOURCE_DIRS      := sources/snddecl
MEDIA.SOUND.SNDDECL.LIB_DIRS         :=  
MEDIA.SOUND.SNDDECL.LIBS             := 
MEDIA.SOUND.SNDDECL.COMPILER_CFLAGS  := 
MEDIA.SOUND.SNDDECL.COMPILER_DEFINES := 
MEDIA.SOUND.SNDDECL.IMPORTS          := compile.media.sound

#���� �4 - Sound tests
MEDIA.SOUND.TESTS.TYPE             := test-suite
MEDIA.SOUND.TESTS.INCLUDE_DIRS     :=
MEDIA.SOUND.TESTS.SOURCE_DIRS      := tests
MEDIA.SOUND.TESTS.LIB_DIRS         :=  
MEDIA.SOUND.TESTS.LIBS             :=
MEDIA.SOUND.TESTS.COMPILER_CFLAGS  :=
MEDIA.SOUND.TESTS.COMPILER_DEFINES :=
MEDIA.SOUND.TESTS.IMPORTS          := compile.media.sound link.media.sound link.media.sound.default_decoders link.media.sound.snddecl

#���� - ������ ������������
MEDIA.SOUND.INFO.TYPE        := doxygen-info
MEDIA.SOUND.INFO.CHM_NAME    := funner.media.sound
MEDIA.SOUND.INFO.SOURCE_DIRS := include
MEDIA.SOUND.INFO.IMPORTS     := compile.media.sound
