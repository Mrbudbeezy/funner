TARGETS += EXTERN.LIBFFI.SOURCES
#TARGETS += EXTERN.LIBFFI.TESTS

#���� - ������ FFI
EXTERN.LIBFFI.SOURCES.TYPE            := static-lib
EXTERN.LIBFFI.SOURCES.NAME            := funner.extern.libffi
EXTERN.LIBFFI.SOURCES.SOURCE_DIRS     := sources
EXTERN.LIBFFI.SOURCES.x86.SOURCE_DIRS := sources/x86
EXTERN.LIBFFI.SOURCES.IMPORTS         := compile.extern.libffi

#���� - ������ ������ FFI
EXTERN.LIBFFI.TESTS.TYPE                := test-suite
EXTERN.LIBFFI.TESTS.SOURCE_DIRS         := tests/call tests/special
EXTERN.LIBFFI.TESTS.IMPORTS             := compile.extern.libffi link.extern.libffi
