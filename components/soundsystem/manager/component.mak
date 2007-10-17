###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SOUND_SYSTEM_SOURCES SOUND_SYSTEM_TESTS

#���� �1 - Sound system sources
SOUND_SYSTEM_SOURCES.TYPE             := static-lib
SOUND_SYSTEM_SOURCES.NAME             := soundsystem
SOUND_SYSTEM_SOURCES.INCLUDE_DIRS     := include ../../commonlib/include ../../mathlib/include ../../xtl/include ../../systemlib/include ../../medialib/include
SOUND_SYSTEM_SOURCES.SOURCE_DIRS      := sources/device sources/manager
SOUND_SYSTEM_SOURCES.LIB_DIRS         :=  
SOUND_SYSTEM_SOURCES.LIBS             := 
SOUND_SYSTEM_SOURCES.COMPILER_CFLAGS  :=
SOUND_SYSTEM_SOURCES.COMPILER_DEFINES := 

#���� �2 - Sound system tests
SOUND_SYSTEM_TESTS.TYPE             := test-suite
SOUND_SYSTEM_TESTS.INCLUDE_DIRS     := include ../../commonlib/include ../../mathlib/include ../../xtl/include ../../systemlib/include ../openaldevice/include
SOUND_SYSTEM_TESTS.SOURCE_DIRS      := tests/device tests/manager
SOUND_SYSTEM_TESTS.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
SOUND_SYSTEM_TESTS.LIBS             := commonlib soundsystem system user32 openaldevice medialib zzip zlib vorbisfile_static vorbis_static ogg openal32
SOUND_SYSTEM_TESTS.DLLS             :=
SOUND_SYSTEM_TESTS.DLL_DIRS         :=
SOUND_SYSTEM_TESTS.COMPILER_CFLAGS  :=
SOUND_SYSTEM_TESTS.COMPILER_DEFINES :=
