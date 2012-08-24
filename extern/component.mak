###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#���� - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip lib64 pcre jpeg tiff libpng devil ogg vorbis vorbisfile lua \
                                 freetype libpsd theora shiny openssl

ifeq (,$(filter iphone,$(PROFILES)))
  FUNNER_EXTERN_LIBS.x86.COMPONENTS := geekinfo
endif

ifeq (,$(filter beagleboard,$(PROFILES))$(filter webos,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += curl
endif

#ifneq (,$(filter win32,$(PROFILES)))
#  FUNNER_EXTERN_LIBS.COMPONENTS += luajit
#endif

ifneq (,$(filter linux,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifneq (,$(filter android,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifneq (,$(filter android-arm,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += android_ndk_profiler
endif

ifneq (,$(filter psp,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := psp_addons $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif

ifneq (,$(filter bada,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := bada $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif

ifneq (,$(filter wince,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := wcecompat wince $(FUNNER_EXTERN_LIBS.COMPONENTS) pthreads_wince
else
  FUNNER_EXTERN_LIBS.COMPONENTS += bullet mongoose
endif

ifeq (,$(filter has_iconv,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += libiconv
endif
