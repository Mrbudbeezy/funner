###################################################################################################
#���᮪ 楫��
###################################################################################################
TARGETS := PCRE

#���� �1 - PCRE
PCRE.TYPE                  := static-lib               #��� 楫�
PCRE.NAME                  := extern.pcre              #��� ��室���� 䠩��
PCRE.INCLUDE_DIRS          := include                  #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
PCRE.SOURCE_DIRS           := sources                  #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
PCRE.LIB_DIRS              :=                          #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
PCRE.LIBS                  :=                          #���᮪ �ᯮ��㥬�� �� �������� ������⥪
PCRE.COMPILER_DEFINES      := PCRE_STATIC              #��।������ ᡮન
PCRE.COMPILER_CFLAGS       :=                          #����� ᡮન (������� �� ���䨣��樨)
PCRE.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #��।������ ᡮન
PCRE.g++.COMPILER_CFLAGS   := --no-warn                #����� ᡮન (������� �� ���䨣��樨)
