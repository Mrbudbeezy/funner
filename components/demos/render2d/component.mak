###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := DEMOS.RENDER2D.SHARED DEMOS.RENDER2D.TESTS

#PROFILES += egl gles

#���� �1 - ����� ��� 2D �������
DEMOS.RENDER2D.SHARED.TYPE             := static-lib
DEMOS.RENDER2D.SHARED.NAME             := funner.demos.render2d.shared
DEMOS.RENDER2D.SHARED.INCLUDE_DIRS     :=
DEMOS.RENDER2D.SHARED.SOURCE_DIRS      := shared
DEMOS.RENDER2D.SHARED.IMPORTS          := compile.system compile.render.scene_render2d compile.render.mid_level.window_driver compile.render.low_level \
                                          compile.scene_graph.core

#���� �2 - ����� 2D �������
DEMOS.RENDER2D.TESTS.TYPE             := test-suite
DEMOS.RENDER2D.TESTS.INCLUDE_DIRS     := shared
DEMOS.RENDER2D.TESTS.SOURCE_DIRS      := sources
DEMOS.RENDER2D.TESTS.LIB_DIRS         :=  
DEMOS.RENDER2D.TESTS.LIBS             := funner.demos.render2d.shared
DEMOS.RENDER2D.TESTS.LINK_INCLUDES    :=
DEMOS.RENDER2D.TESTS.IMPORTS          := link.system compile.common compile.scene_graph.core compile.render.scene_render2d link.render.scene_render2d \
                                         link.render.mid_level.window_driver link.render.low_level.opengl_driver \
                                         link.media.rfx.xmtl.obsolete link.render.mid_level.debug_renderer_driver link.common.wxf \
                                         link.media.image.pvr run.render.low_level.opengl_driver link.media.video.theora
