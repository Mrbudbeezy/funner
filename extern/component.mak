###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#���� - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip pcre jpeg tiff libpng devil ogg vorbis vorbisfile lua freetype libpsd bullet

ifeq (,$(filter iphone,$(PROFILES)))
  COMPONENT_DIRS += curl
endif
