###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := COMMONLIB_EXTERN_LIBS COMMONLIB_COMPONENTS

#���� �1 - CommonLib external libraries
COMMONLIB_EXTERN_LIBS.TYPE       := package
COMMONLIB_EXTERN_LIBS.COMPONENTS := extern/zlib extern/zzip extern/pcre

#���� �1 - CommonLib sources
COMMONLIB_COMPONENTS.TYPE       := package
COMMONLIB_COMPONENTS.COMPONENTS := sources/filesys sources/hash sources/memory sources/parser sources/strlib sources/utils
