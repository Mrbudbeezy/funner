###################################################################################################
#������ �����
###################################################################################################
TARGETS.linux := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS

#���� �1 - OPENALLIB
EXTERN.OPENALLIB.TYPE                 := dynamic-lib
EXTERN.OPENALLIB.NAME                 := funner.extern.openal
EXTERN.OPENALLIB.INCLUDE_DIRS         := include sources
EXTERN.OPENALLIB.SOURCE_DIRS          := sources/openal32 sources/alc
EXTERN.OPENALLIB.LIB_DIRS             :=
EXTERN.OPENALLIB.LIBS                 := 

#���� �2 - OPENAL_TESTS
EXTERN.OPENAL_TESTS.TYPE         := test-suite
EXTERN.OPENAL_TESTS.SOURCE_DIRS  := tests
