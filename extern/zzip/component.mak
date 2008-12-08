###################################################################################################
#������ �����
###################################################################################################
TARGETS := EXTERN.ZZIP

#���� �1 - ZZIP
EXTERN.ZZIP.TYPE             := static-lib                      #��� ����
EXTERN.ZZIP.NAME             := funner.extern.zzip              #��� ��������� �����
EXTERN.ZZIP.INCLUDE_DIRS     := include ../zlib/include         #������������� ���� � ��������� c ������������� �������
EXTERN.ZZIP.SOURCE_DIRS      := sources                         #������������� ���� � ��������� c ��������� �������
EXTERN.ZZIP.LIB_DIRS         :=                                 #������������� ���� � ��������� � ������������
EXTERN.ZZIP.LIBS             :=                                 #������ ������������ ��� �������� ���������
EXTERN.ZZIP.msvc.COMPILER_CFLAGS  :=  -wd4244                   #����� ������ (������� �� ������������)
EXTERN.ZZIP.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE   #����������� ������
EXTERN.ZZIP.g++.COMPILER_CFLAGS := --no-warn                    #����� ������ (������� �� ������������)
EXTERN.ZZIP.nds.g++.COMPILER_DEFINES := ZZIP_1_H
EXTERN.ZZIP.macosx.COMPILER_DEFINES := ZZIP_1_H
