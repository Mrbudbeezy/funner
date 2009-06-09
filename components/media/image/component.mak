###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := MEDIA.IMAGE.SOURCES MEDIA.XATLAS.SOURCES MEDIA.LEFT_BOTTOM_PACKER.SOURCES MEDIA.IMAGE.TESTS \
           MEDIA.IMAGE.ANIMATION_ENHANCER MEDIA.IMAGE.SPRITE_COMPOSER MEDIA.IMAGE.PSD_EXPORTER

#���� - Image sources
MEDIA.IMAGE.SOURCES.TYPE             := static-lib
MEDIA.IMAGE.SOURCES.NAME             := funner.media.image
MEDIA.IMAGE.SOURCES.INCLUDE_DIRS     := ../../../extern/devil/include
MEDIA.IMAGE.SOURCES.SOURCE_DIRS      := sources/image sources/atlas
MEDIA.IMAGE.SOURCES.LIB_DIRS         :=  
MEDIA.IMAGE.SOURCES.LIBS             := 
MEDIA.IMAGE.SOURCES.COMPILER_CFLAGS  :=
MEDIA.IMAGE.SOURCES.COMPILER_DEFINES := IL_STATIC_LIB
MEDIA.IMAGE.SOURCES.IMPORTS          := compile.static.mak

#���� - XAtlas sources
MEDIA.XATLAS.SOURCES.TYPE             := static-lib
MEDIA.XATLAS.SOURCES.NAME             := funner.media.image.xatlas
MEDIA.XATLAS.SOURCES.INCLUDE_DIRS     := 
MEDIA.XATLAS.SOURCES.SOURCE_DIRS      := sources/xatlas
MEDIA.XATLAS.SOURCES.LIB_DIRS         :=  
MEDIA.XATLAS.SOURCES.LIBS             := 
MEDIA.XATLAS.SOURCES.COMPILER_CFLAGS  :=
MEDIA.XATLAS.SOURCES.COMPILER_DEFINES := 
MEDIA.XATLAS.SOURCES.IMPORTS          := compile.static.mak

#���� - Left-bottom packer sources
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.TYPE             := static-lib
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.NAME             := funner.media.image.left_bottom_packer
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.INCLUDE_DIRS     := ../../bound_volumes/include
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.SOURCE_DIRS      := sources/packers/left_bottom
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.LIB_DIRS         :=  
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.LIBS             := 
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.COMPILER_CFLAGS  :=
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.COMPILER_DEFINES := 
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.IMPORTS          := compile.static.mak

#���� - Image tests
MEDIA.IMAGE.TESTS.TYPE             := test-suite
MEDIA.IMAGE.TESTS.INCLUDE_DIRS     := ../../bound_volumes/include
MEDIA.IMAGE.TESTS.SOURCE_DIRS      := tests/image tests/atlas
MEDIA.IMAGE.TESTS.LIB_DIRS         :=  
MEDIA.IMAGE.TESTS.LIBS             :=
MEDIA.IMAGE.TESTS.COMPILER_CFLAGS  :=
MEDIA.IMAGE.TESTS.COMPILER_DEFINES :=
MEDIA.IMAGE.TESTS.IMPORTS          := compile.static.mak link.static.mak xatlas.link.static.mak left_bottom_packer.link.static.mak

#���� - Animation enhancer sources
MEDIA.IMAGE.ANIMATION_ENHANCER.TYPE             := application
MEDIA.IMAGE.ANIMATION_ENHANCER.NAME             := animation_enhancer
MEDIA.IMAGE.ANIMATION_ENHANCER.SOURCE_DIRS      := utils/animation_enhancer
MEDIA.IMAGE.ANIMATION_ENHANCER.IMPORTS          := compile.static.mak link.static.mak

#���� - Sprite composer sources
MEDIA.IMAGE.SPRITE_COMPOSER.TYPE             := application
MEDIA.IMAGE.SPRITE_COMPOSER.NAME             := sprite_composer
MEDIA.IMAGE.SPRITE_COMPOSER.SOURCE_DIRS      := utils/sprite_composer
MEDIA.IMAGE.SPRITE_COMPOSER.IMPORTS          := compile.static.mak link.static.mak

#���� - PSD exporter
MEDIA.IMAGE.PSD_EXPORTER.TYPE             := application
MEDIA.IMAGE.PSD_EXPORTER.NAME             := psd-exporter
MEDIA.IMAGE.PSD_EXPORTER.INCLUDE_DIRS     :=
MEDIA.IMAGE.PSD_EXPORTER.SOURCE_DIRS      := utils/psd_exporter
MEDIA.IMAGE.PSD_EXPORTER.IMPORTS          := compile.static.mak link.static.mak ../../../extern/libpsd/compile.static.mak ../../../extern/libpsd/link.static.mak
MEDIA.IMAGE.PSD_EXPORTER.EXECUTION_DIR    := utils/psd_exporter
