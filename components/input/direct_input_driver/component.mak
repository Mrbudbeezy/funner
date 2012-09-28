###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := INPUT.DIRECT_INPUT_DRIVER.SOURCES INPUT.DIRECT_INPUT_DRIVER.TESTS

#���� �1 - Window driver sources
INPUT.DIRECT_INPUT_DRIVER.SOURCES.TYPE                 := static-lib
INPUT.DIRECT_INPUT_DRIVER.SOURCES.NAME                 := funner.input.direct_input_driver
INPUT.DIRECT_INPUT_DRIVER.SOURCES.INCLUDE_DIRS         :=
INPUT.DIRECT_INPUT_DRIVER.SOURCES.SOURCE_DIRS          := sources
INPUT.DIRECT_INPUT_DRIVER.SOURCES.LIB_DIRS             :=  
INPUT.DIRECT_INPUT_DRIVER.SOURCES.LIBS                 := 
INPUT.DIRECT_INPUT_DRIVER.SOURCES.COMPILER_CFLAGS      :=
INPUT.DIRECT_INPUT_DRIVER.SOURCES.COMPILER_DEFINES     := DIRECT_INPUT_KEYBOARD_DISABLED
INPUT.DIRECT_INPUT_DRIVER.SOURCES.IMPORTS              := compile.input.manager compile.system compile.input.low_level compile.extern.dxsdk
INPUT.DIRECT_INPUT_DRIVER.SOURCES.msvc.COMPILER_CFLAGS := -wd4503

#���� �2 - Window driver tests
INPUT.DIRECT_INPUT_DRIVER.TESTS.TYPE             := test-suite
INPUT.DIRECT_INPUT_DRIVER.TESTS.INCLUDE_DIRS     :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.SOURCE_DIRS      := tests
INPUT.DIRECT_INPUT_DRIVER.TESTS.LIB_DIRS         := 
INPUT.DIRECT_INPUT_DRIVER.TESTS.LIBS             :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.DLLS             :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.DLL_DIRS         :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.COMPILER_CFLAGS  :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.COMPILER_DEFINES :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.IMPORTS          := link.input.direct_input_driver compile.input.low_level compile.system
