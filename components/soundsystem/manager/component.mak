###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SOUND_SYSTEM_SOURCES

#���� �1 - Sound system sources
SOUND_SYSTEM_SOURCES.TYPE             := static-lib
SOUND_SYSTEM_SOURCES.NAME             := soundsystem
SOUND_SYSTEM_SOURCES.INCLUDE_DIRS     := include ../../commonlib/include ../../mathlib/include ../../xtl/include ../../systemlib/include ../../medialib/include
SOUND_SYSTEM_SOURCES.SOURCE_DIRS      := sources/device sources/manager
SOUND_SYSTEM_SOURCES.LIB_DIRS         :=  
SOUND_SYSTEM_SOURCES.LIBS             := 
SOUND_SYSTEM_SOURCES.COMPILER_CFLAGS  :=
SOUND_SYSTEM_SOURCES.COMPILER_DEFINES := 
