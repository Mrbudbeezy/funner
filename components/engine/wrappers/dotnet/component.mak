###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS.msvc   := ENGINE.DOT_NET_WRAPPER

#���� - ������ ��� �������������� � .Net
ENGINE.DOT_NET_WRAPPER.TYPE            := dynamic-lib
ENGINE.DOT_NET_WRAPPER.NAME            := funner.net
ENGINE.DOT_NET_WRAPPER.SOURCE_DIRS     := sources
ENGINE.DOT_NET_WRAPPER.COMPILER_CFLAGS := -clr -wd4793
ENGINE.DOT_NET_WRAPPER.IMPORTS         := compile.xtl compile.launcher
