###################################################################################################
#���᮪ 楫��
###################################################################################################
TARGETS := EXTERN.ZZIP

#���� �1 - ZZIP
EXTERN.ZZIP.TYPE             := static-lib               #��� 楫�
EXTERN.ZZIP.NAME             := extern.zzip              #��� ��室���� 䠩��
EXTERN.ZZIP.INCLUDE_DIRS     := include ../zlib/include  #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
EXTERN.ZZIP.SOURCE_DIRS      := sources                  #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
EXTERN.ZZIP.LIB_DIRS         :=                          #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
EXTERN.ZZIP.LIBS             :=                          #���᮪ �ᯮ��㥬�� �� �������� ������⥪
EXTERN.ZZIP.COMPILER_CFLAGS  :=                          #����� ᡮન (������� �� ���䨣��樨)
EXTERN.ZZIP.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #��।������ ᡮન
EXTERN.ZZIP.g++.COMPILER_CFLAGS := --no-warn                #����� ᡮન (������� �� ���䨣��樨)
EXTERN.ZZIP.nds.g++.COMPILER_DEFINES := ZZIP_1_H
EXTERN.ZZIP.macosx.COMPILER_DEFINES := ZZIP_1_H
