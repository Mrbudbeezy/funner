###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.ZLIB

#���� �1 - ZLIB
EXTERN.ZLIB.TYPE                  := static-lib  #��� ����
EXTERN.ZLIB.NAME                  := funner.extern.zlib #��� ��������� �����
EXTERN.ZLIB.SOURCE_DIRS           := sources     #������������� ���� � ��������� c ��������� �������
EXTERN.ZLIB.IMPORTS               := compile.extern.zlib
