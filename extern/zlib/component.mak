###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.ZLIB

#���� �1 - ZLIB
EXTERN.ZLIB.TYPE             := static-lib  #��� ����
EXTERN.ZLIB.NAME             := extern.zlib #��� ��������� �����
EXTERN.ZLIB.INCLUDE_DIRS     := include     #������������� ���� � ��������� c ������������� �������
EXTERN.ZLIB.SOURCE_DIRS      := sources     #������������� ���� � ��������� c ��������� �������
EXTERN.ZLIB.LIB_DIRS         :=             #������������� ���� � ��������� � ������������
EXTERN.ZLIB.LIBS             :=             #������ ������������ ��� �������� ���������
EXTERN.ZLIB.COMPILER_CFLAGS  :=             #����� ������ (������� �� ������������)
EXTERN.ZLIB.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #����������� ������
