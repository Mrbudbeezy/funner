###################################################################################################
#���᮪ 楫��
###################################################################################################
TARGETS := ZZIP

#���� �1 - ZZIP
ZZIP.TYPE             := static-lib               #��� 楫�
ZZIP.NAME             := extern.zzip              #��� ��室���� 䠩��
ZZIP.INCLUDE_DIRS     := include ../zlib/include  #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
ZZIP.SOURCE_DIRS      := sources                  #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
ZZIP.LIB_DIRS         :=                          #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
ZZIP.LIBS             :=                          #���᮪ �ᯮ��㥬�� �� �������� ������⥪
ZZIP.COMPILER_CFLAGS  :=                          #����� ᡮન (������� �� ���䨣��樨)
ZZIP.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #��।������ ᡮન
ZZIP.g++.COMPILER_CFLAGS := --no-warn                #����� ᡮન (������� �� ���䨣��樨)
ZZIP.nds.g++.COMPILER_DEFINES := ZZIP_1_H