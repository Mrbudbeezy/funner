###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.FREETYPE

#���� �1 - Freetype
EXTERN.FREETYPE.TYPE                  := static-lib
EXTERN.FREETYPE.NAME                  := extern.freetype
EXTERN.FREETYPE.INCLUDE_DIRS          := include
EXTERN.FREETYPE.SOURCE_DIRS           := sources/autofit sources/base sources/bdf sources/cache sources/cff sources/cid sources/gzip \
                                         sources/lzw sources/pcf sources/pfr sources/psaux sources/pshinter sources/psnames sources/raster \
                                         sources/sfnt sources/smooth sources/truetype sources/type1 sources/type42 sources/winfonts
EXTERN.FREETYPE.LIB_DIRS              :=
EXTERN.FREETYPE.LIBS                  :=
EXTERN.FREETYPE.COMPILER_DEFINES      := FT2_BUILD_LIBRARY
EXTERN.FREETYPE.msvc.COMPILER_DEFINES := WIN32
EXTERN.FREETYPE.msvc.COMPILER_CFLAGS  := -wd4244