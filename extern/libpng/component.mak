###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := EXTERN.PNG

#���� - PNG sources
EXTERN.PNG.TYPE                 := static-lib
EXTERN.PNG.NAME                 := funner.extern.libpng
EXTERN.PNG.INCLUDE_DIRS         := include ../zlib/include
EXTERN.PNG.SOURCE_DIRS          := sources
EXTERN.PNG.msvc.COMPILER_CFLAGS := -wd4018
EXTERN.PNG.clang.COMPILER_CFLAGS := --no-warn
