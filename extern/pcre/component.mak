###################################################################################################
#���᮪ 楫��
###################################################################################################
TARGETS := EXTERN.PCRE

#���� �1 - PCRE
EXTERN.PCRE.TYPE                  := static-lib               #��� 楫�
EXTERN.PCRE.NAME                  := funner.extern.pcre              #��� ��室���� 䠩��
EXTERN.PCRE.INCLUDE_DIRS          := include                  #�⭮�⥫�� ��� � ��⠫���� c ���������묨 䠩����
EXTERN.PCRE.SOURCE_DIRS           := sources                  #�⭮�⥫�� ��� � ��⠫���� c ��室�묨 䠩����
EXTERN.PCRE.LIB_DIRS              :=                          #�⭮�⥫�� ��� � ��⠫���� � ������⥪���
EXTERN.PCRE.LIBS                  :=                          #���᮪ �ᯮ��㥬�� �� �������� ������⥪
EXTERN.PCRE.COMPILER_DEFINES      := PCRE_STATIC              #��।������ ᡮન
EXTERN.PCRE.COMPILER_CFLAGS       :=                          #����� ᡮન (������� �� ���䨣��樨)
EXTERN.PCRE.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #��।������ ᡮન
EXTERN.PCRE.g++.COMPILER_CFLAGS   := --no-warn                #����� ᡮન (������� �� ���䨣��樨)
